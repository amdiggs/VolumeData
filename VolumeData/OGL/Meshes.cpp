//
//  Simulation.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 10/21/22.
//

#include "Meshes.hpp"
#include "Shapes.hpp"
#include "Atomic.hpp"
#include "Texture.hpp"
#include "Operations.hpp"
#include "FrameBuffer.hpp"
#include "Computes.hpp"
#include "Hist2D.hpp"
#include <map>
#include "CubeMarchTable.h"
#include "FileIO.hpp"

extern std::map<char, Character> CHAR_MAP;
extern Operator* op;
extern Simulation* Sim;

AMD::Vec3 Get_Basis(char hat){
    switch (hat) {
        case 'x':
            return AMD::Vec3(1.0, 0.0, 0.0);
            break;
            
        case 'y':
            return AMD::Vec3(0.0, 1.0, 0.0);
            break;
        
        case 'z':
            return AMD::Vec3(0.0, 0.0, 1.0);
            break;
            
        default:
            return AMD::Vec3(0.0, 0.0, 0.0);
            break;
    }
}

Axis::Axis(char axis, float lo, float high, AMD::Vec3 start,AMD::Vec3 end, int num_ticks, const char* plane, const char* type)
:m_dir(Get_Basis(axis)) ,m_origin(start), m_num_ticks(num_ticks),m_lo(lo),m_range(high - lo), m_data_type(type)
{
    AMD::Vec3 tmp = end - start;
    m_length = tmp.dot(m_dir);
    m_plane[0] = plane[0]; m_plane[1] = plane[1];
}


Axis::Axis(char axis, float* data,int num_vals, AMD::Vec3 start,AMD::Vec3 end, int num_ticks, const char* plane, const char* type)
:m_dir(Get_Basis(axis)) ,m_origin(start), m_num_ticks(num_ticks), m_data_type(type)
{
    AMD::Vec3 tmp = end - start;
    m_length = tmp.dot(m_dir);
    m_plane[0] = plane[0]; m_plane[1] = plane[1];
    float min = 1000.0; float max = -1000.0;
    for (int i = 0; i < num_vals; i++){
        min = AMD::Min(min, data[i]);
        max = AMD::Max(max, data[i]);
    }
    m_lo = min;
    m_range = max - min;
}

Axis::~Axis(){}



void Axis::Set_Values(){
    if(m_num_ticks == 0){std::cout << "class Axis: number of ticks == 0! \n"; exit(27);}
    if(m_num_ticks > 25){std::cout << "class Axis: number of ticks > 25! \n"; exit(27);}
    
    float dx = m_range/m_num_ticks;
    float val = m_lo;
    for ( int i = 0; i<m_num_ticks; i++){
        m_values[i] = val;
        val+=dx;
    }
    
}


void Axis::Set_Positions(){
    if(m_num_ticks == 0){std::cout << "class Axis: number of ticks == 0! \n"; exit(27);}
    if(m_num_ticks > 25){std::cout << "class Axis: number of ticks > 25! \n"; exit(27);}
    
    float dx = m_length/m_num_ticks;
    AMD::Vec3 tmp;
    for ( int i = 0; i<m_num_ticks; i++){
        tmp = m_dir*(i*dx);
        m_positions[i] = m_origin + tmp;
    }
    
}



AMD::Vec3* Axis::Get_Positions(){
    return (AMD::Vec3*)m_positions;
}


float* Axis::Get_Values(){
    return m_values;
}

void Axis::Draw(){
    Text_Mesh text;
    
    text.Render_Tick_Labels(m_positions, m_values, m_num_ticks, 2.0, m_plane);
    
}


//##############################################################################################
Atoms_Mesh::Atoms_Mesh()
:m_sh(shader_file)
{
    Sphere sp(1.0);
    VertexBuffer vb(sp.basic_verts, sp.num_verts()*sizeof(AMD::Vertex_Basic));
    m_VAO.Add_Basic_Vertex_Buffer(vb);
    m_IBO.Gen_Buffer(sp.indices,sp.num_idx());
    m_offset_vbo = m_VAO.Add_Dynamic_Instance_Buffer(sizeof(AMD::Vec3));
    m_types_vbo = m_VAO.Add_Dynamic_Instance_Buffer(sizeof(float));
    
    m_offsets = (AMD::Vec3*)malloc(10000*sizeof(AMD::Vec3));
    m_types = (float*)malloc(10000*sizeof(float));
    //Set_Data();
    
    
}



Atoms_Mesh::~Atoms_Mesh()
{
    free(m_types);
    free(m_offsets);
}



void Atoms_Mesh::Set_Data(){
    m_num_atoms = Sim->Num_Atoms();
    Atom* ats = Sim->Atoms();
    AMD::Vec3 center = Sim->Sim_Box()*(-0.5);
    for(int i = 0; i < m_num_atoms; i++){
        m_offsets[i] = ats[i].Get_Coords();
        m_offsets[i]+=center;
        m_types[i] = ats[i].Get_Type();
    }
    
}


void Atoms_Mesh::Set_Data(RHO_3D& rho){
    m_num_atoms = rho.Get_Num_Ats();
    AMD::Vec3* ats_coords = rho.Get_At_Coords();
    float* at_typs = rho.Get_At_Types();
    AMD::Vec3 center = -0.500*rho.Get_Center();
    float typ;
    for(int i = 0; i < m_num_atoms; i++){
        m_offsets[i] = ats_coords[i];
        m_offsets[i]+=center;
        switch ((int)at_typs[i]) {
            case 1:
                typ = 2.0;
                break;
            case 14:
                typ = 1.0;
                break;
            default:
                typ = 0.0;
                break;
        }
        m_types[i] = typ;
    }
    
}


void Atoms_Mesh::Set_Shader(){
    this->m_sh.bind();
    
    
    
}


void Atoms_Mesh::Set_Uniforms(){
    this->m_sh.Set_Uniform_MVP();
    this->m_sh.Set_Uniform_Normal();
    
}

void Atoms_Mesh::Set_Uniforms(Light_Src& src){
    this->m_sh.Set_Uniform_MVP();
    this->m_sh.Set_Uniform_Normal();
    this->m_sh.Set_Uinform_LightSource(src);
    
}

void Atoms_Mesh::Sort(char n){
    AMD::Vec3 dir = Get_Basis(n);
    AMD::Vec3 box = Sim->Sim_Box();
    int num_slices = ceil(box.y/1.35748);
    AMD::Vec3 slices[num_slices][1000];
    int types[num_slices][1000];
    int counts[num_slices];
    int prev = m_num_atoms;
    for(int n = 0; n< num_slices; n++){
        counts[n] = 0;
    }
    for(int i = 0; i < m_num_atoms; i++){
        AMD::Vec3 coords = m_offsets[i] + 0.5*box;
        int r_id = floor(coords.y/1.3575);
        int c_id = counts[r_id];
        slices[r_id][c_id] = m_offsets[i];
        types[r_id][c_id] = m_types[i];
        counts[r_id] ++;
        
    }
    
    int count = 0;
    for(int i = 0; i < num_slices - 2; i++){
        for (int j = 0; j < counts[i]; j++){
            m_offsets[count] = slices[i][j];
            m_types[count] = types[i][j];
            count ++;
        }
    }
    
    if(count != prev){
        ;
    }
    m_num_atoms = count;
}


void Atoms_Mesh::Draw(){
    glBindBuffer(GL_ARRAY_BUFFER,m_offset_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_num_atoms*sizeof(AMD::Vec3), (void*)m_offsets, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,m_types_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_num_atoms*sizeof(float), (void*)m_types, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    
    
    this->m_VAO.bind();
    this->m_IBO.bind();
    glDrawElementsInstanced(GL_TRIANGLES,m_IBO.get_num(), GL_UNSIGNED_INT,0, m_num_atoms);
    
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}



//##############################################################3
Quad_Mesh::Quad_Mesh()
{
    Quad qd(2.0);
    VertexBuffer vb(qd.verts, qd.num_verts()*sizeof(AMD::Vertex_TX));
    m_VAO.Add_Vertex_Buffer(vb);
    m_IBO.Gen_Buffer(qd.indices,qd.num_idx());
}

Quad_Mesh::Quad_Mesh(AMD::Vec3 LL, AMD::Vec3 LR, AMD::Vec3 UR, AMD::Vec3 UL)
{
    Quad qd(LL,LR,UR,UL, "cw");
    VertexBuffer vb(qd.verts, qd.num_verts()*sizeof(AMD::Vertex_TX));
    m_VAO.Add_Vertex_Buffer(vb);
    m_IBO.Gen_Buffer(qd.indices,qd.num_idx());
}

Quad_Mesh::~Quad_Mesh() {}


unsigned int Quad_Mesh::num_idx() {
    return this->m_IBO.get_num();
}


void Quad_Mesh::Bind() {
    this->m_VAO.bind();
    this->m_IBO.bind();
}

void Quad_Mesh::UnBind() {
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}




//============================================================================================

Environment::Environment()
:m_sh(shader_file)
{
    Environment_Cube cb;
    VertexBuffer vb(cb.verts, cb.m_num_verts*sizeof(AMD::Vertex_TX));
    m_VAO.Add_Vertex_Buffer(vb);
    m_IBO.Gen_Buffer(cb.indices,cb.m_num_idx);
    
}

Environment::~Environment(){}

void Environment::Bind(){
    this->m_sh.bind();
    this->m_VAO.bind();
    this->m_IBO.bind();
}


void Environment::UnBind(){
    this->m_sh.unbind();
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}

void Environment::Set_Uniforms(Light_Src& light){
    AMD::Mat4 m_VP = op->Get_VP();
    this->m_sh.Set_Uniform_Mat4("u_MVP", &m_VP[0][0]);
    //this->m_sh.Set_Uinform_LightSource(light);
}

void Environment::Set_Shader(){
    this->m_sh.bind();
}

void Environment::Draw(){
    this->m_VAO.bind();
    this->m_IBO.bind();
    glDrawElements(GL_TRIANGLES,m_IBO.get_num(), GL_UNSIGNED_INT,0);
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}

void Environment::Attach_Texture(Texture& tx){
    m_sh.Set_Texture(m_sampler, tx);
}







//##########################################################################################

Hist_2D_Grid_Mesh::Hist_2D_Grid_Mesh()
:m_sh(shader_file)
{}



Hist_2D_Grid_Mesh::Hist_2D_Grid_Mesh(Hist_2D& hist)
:m_sh(shader_file),  m_rows(hist.m_rows), m_row_width(hist.m_bin_width_z), m_cols(hist.m_cols), m_col_width(hist.m_bin_width_x)
{
    m_counts = (float**)malloc(m_rows*sizeof(float*));
    for (int i =0; i<m_rows; i++){
        m_counts[i] = (float*)malloc(m_cols*sizeof(float));
    }
    Grid gr(m_rows,m_row_width, m_cols,m_col_width);
    for (int i = 0; i< gr.num_verts(); i++){
        m_verts[i] = gr.verts[i].pos;
    }
    Set_Data(hist);
    m_inst_vb_counts = m_VAO.Add_Dynamic_Buffer(sizeof(AMD::Vec3));
    m_inst_vb_colors = m_VAO.Add_Dynamic_Buffer(sizeof(AMD::Vec4));
    m_line_IBO.Gen_Buffer(gr.L_indices,gr.num_line_idx());
    m_tri_IBO.Gen_Buffer(gr.T_indices,gr.num_tri_idx());
}

Hist_2D_Grid_Mesh::~Hist_2D_Grid_Mesh() {
    for (int i = 0; i< m_rows; i++){
        free(m_counts[i]);
    }
    free(m_counts);
}



void Hist_2D_Grid_Mesh::Set_Data(Hist_2D& hist){
    int count = 0;
    float min = hist.m_min;
    float max = hist.m_max;
    float val = 0;

    // X AMU / Vol A^3 = (1.66*X) / Vol g/cm^3
    float offset = -1.0*min; //-0.5*(min + max);
    float ave;
    for (int i = 0; i < m_rows; i++){
        for ( int j = 0 ; j < m_cols; j++){
            ave = hist.Get_Counts()[i][j]; 
            val = offset +  ave;
            m_verts[count].y = val;
            float normed_h = (ave - min)/(max - min);
            m_clrs[count] = AMD::Vec4(1.0 - normed_h, 0.0, normed_h, 1.0);
            count ++;
        }
    }
    m_num_bins = count;
}


void Hist_2D_Grid_Mesh::Clear(){
    for (int i = 0; i < m_rows; i++){
        for ( int j = 0 ; j < m_cols; j++){
            m_counts[i][j] = 0;
        }
    }
}


unsigned int Hist_2D_Grid_Mesh::num_idx() {
    return this->m_line_IBO.get_num();
}

void Hist_2D_Grid_Mesh::Bind() {
    this->m_VAO.bind();
    this->m_line_IBO.bind();
}

void Hist_2D_Grid_Mesh::UnBind() {
    this->m_VAO.unbind();
    this->m_line_IBO.unbind();
}

void Hist_2D_Grid_Mesh::Set_Uniforms(Light_Src& light){
    this->m_sh.Set_Uniform_MVP();
}

void Hist_2D_Grid_Mesh::Set_Shader(){
    this->m_sh.bind();
    
}



void Hist_2D_Grid_Mesh::Draw(){
    this->m_sh.Set_Uniform_Float("color_mod", 0.2);
    glBindBuffer(GL_ARRAY_BUFFER,m_inst_vb_colors);
    glBufferData(GL_ARRAY_BUFFER, m_num_bins*sizeof(AMD::Vec4), (void*)m_clrs, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,m_inst_vb_counts);
    glBufferData(GL_ARRAY_BUFFER, m_num_bins*sizeof(AMD::Vec3), (void*)m_verts, GL_DYNAMIC_DRAW);
    this->m_VAO.bind();
    this->m_line_IBO.bind();
    glDrawElements(GL_LINES, m_line_IBO.get_num(), GL_UNSIGNED_INT,0);
    this->m_line_IBO.unbind();
    this->m_sh.Set_Uniform_Float("color_mod", 1.0);
    m_tri_IBO.bind();
    glDrawElements(GL_TRIANGLES,m_tri_IBO.get_num(), GL_UNSIGNED_INT,0);
    this->m_VAO.unbind();
    this->m_tri_IBO.unbind();
}





Hist_2D_Bar_Mesh::Hist_2D_Bar_Mesh()
:m_sh(shader_file)
{}

Hist_2D_Bar_Mesh::Hist_2D_Bar_Mesh(Hist_2D &hist)
:m_sh(shader_file), m_cols(hist.m_cols), m_rows(hist.m_rows)
{
    m_counts = (float**)malloc(m_rows*sizeof(float*));
    for (int i =0; i<m_rows; i++){
        m_counts[i] = (float*)malloc(m_cols*sizeof(float));
    }
    Grid gr(m_rows,2.0, m_cols, 2.0);
    for (int i = 0; i< gr.num_verts(); i++){
        m_coords[i] = gr.verts[i].pos;
    }
    Set_Data(hist);
    Cube cb(0.6*2.715);
    VertexBuffer vb(cb.verts, cb.num_verts()*sizeof(AMD::Vertex_TX));
    m_VAO.Add_Vertex_Buffer(vb);
    m_IBO.Gen_Buffer(cb.indices,cb.num_idx());
    m_inst_vb_counts = m_VAO.Add_Dynamic_Instance_Buffer(sizeof(AMD::Vec3));
    m_inst_vb_colors = m_VAO.Add_Dynamic_Instance_Buffer(sizeof(AMD::Vec4));
}

Hist_2D_Bar_Mesh::~Hist_2D_Bar_Mesh()
{ 
    for (int i = 0; i< m_rows; i++){
        free(m_counts[i]);
    }
    free(m_counts);
}

void Hist_2D_Bar_Mesh::Set_Data(float **data, int num_x, int num_z) { 
    return;
}

void Hist_2D_Bar_Mesh::Set_Data(Hist_2D &hist) { 
    int count = 0;
    float min = hist.m_min;
    float max = hist.m_max;
    float val = 0;

    float mod = (0.5*m_rows)/(max - min);
    float offset = -0.5*mod*(min + max);
    float ave;
    for (int i = 0; i < m_rows; i++){
        for ( int j = 0 ; j < m_cols; j++){
            ave = hist.Get_Counts()[i][j];
            val = offset +  mod*ave;
            m_coords[count].y = ave;
            float normed_h = (ave - min)/(max - min);
            m_colors[count] = AMD::Vec4(1.5*normed_h, 0.0 ,1.5*(1.0 - normed_h), 1.0);
            count ++;
        }
    }
    m_num_bins = count;
}

void Hist_2D_Bar_Mesh::Set_Uniforms(Light_Src& light){
    this->m_sh.Set_Uniform_MVP();
}

void Hist_2D_Bar_Mesh::Set_Shader(){
    this->m_sh.bind();
    
}



void Hist_2D_Bar_Mesh::Draw() {
    glBindBuffer(GL_ARRAY_BUFFER,m_inst_vb_counts);
    glBufferData(GL_ARRAY_BUFFER, m_num_bins*sizeof(AMD::Vec3), (void*)m_coords, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,m_inst_vb_colors);
    glBufferData(GL_ARRAY_BUFFER, m_num_bins*sizeof(AMD::Vec4), (void*)m_colors, GL_DYNAMIC_DRAW);
    this->m_VAO.bind();
    this->m_IBO.bind();
    glDrawElementsInstanced(GL_TRIANGLES,m_IBO.get_num(), GL_UNSIGNED_INT,0, m_num_bins);
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}

void Hist_2D_Bar_Mesh::Clear() { 
    for (int i = 0; i < m_rows; i++){
        for ( int j = 0 ; j < m_cols; j++){
            m_counts[i][j] = 0;
        }
    }
}

unsigned int Hist_2D_Bar_Mesh::Num_Points() {
    return m_num_bins;
}


void Hist_2D_Bar_Mesh::Center() {
    op->m_Cam.Look_At(m_coords[0]);
}


//###########Voxel Mesh Class##########################


Voxel_Mesh::Voxel_Mesh()
:m_sh(shader_file)
{
    Cube cb(1.0);
    VertexBuffer vb(cb.basic_verts, cb.num_verts()*sizeof(AMD::Vertex_Basic));
    m_VAO.Add_Basic_Vertex_Buffer(vb);
    m_IBO.Gen_Buffer(cb.indices,cb.num_idx());
    m_inst_vb_centers = m_VAO.Add_Dynamic_Instance_Buffer(sizeof(AMD::Vec3));
    m_inst_vb_counts = m_VAO.Add_Dynamic_Instance_Buffer(sizeof(float));
}



Voxel_Mesh::~Voxel_Mesh()
{
    if(init){
        free(m_counts);
        free(m_centers);
    }
}


void Voxel_Mesh::Init(int num){
    m_counts = (float*)malloc(num*sizeof(float));
    m_centers = (AMD::Vec3*)malloc(num*sizeof(AMD::Vec3));
    init=true;
}

void Voxel_Mesh::Set_Data(AMD::Voxel* vox, int num){
    if(!init){Init(num);}
    int count = 0;
    m_offset = 0.5*Sim->Sim_Box();
    m_box_dims = AMD::Voxel::lengths;
    for ( int i = 0 ; i < num; i++){
        m_counts[count] = 0.51; //vox[i].value;
        m_centers[count] = vox[i].Get_Center() - m_offset;
        count++;
    }
    m_num_bins = count;
}

void Voxel_Mesh::Set_Data(TOPCon &hist) {
    m_cols = hist.Get_Cols();
    m_rows = hist.Get_Rows();
    m_depth = hist.Get_Depth();
    int tot = m_depth*m_rows*m_cols;
    if(!init){Init(tot);}
    int count = 0;
    float min = hist.Get_Min();
    float max = hist.Get_Max();
    m_box_dims = hist.Get_Bin_Widths();
    AMD::Voxel*** Data = hist.Get_Counts();
    //iface = Find_Interfaces(hist);
    m_offset = 0.5*Sim->Sim_Box();
    
    for ( int i = 5; i< m_depth; i++){
        for (int j = 0; j < m_rows; j++){
            for ( int k = 0 ; k < m_cols; k++){
                m_counts[count] = Data[i][j][k].value / 2.0;
                m_centers[count] = Data[i][j][k].Get_Center() - m_offset;
                count++;
            }
        }
    }
    m_num_bins = count;
}


void Voxel_Mesh::Set_Data(RHO_3D &hist) {
    m_cols = hist.Get_Cols();
    m_rows = hist.Get_Rows();
    m_depth = hist.Get_Depth();
    int tot = m_depth*m_rows*m_cols;
    if(!init){Init(tot);}
    int count = 0;
    float min = hist.Get_Min();
    float max = hist.Get_Max();
    m_box_dims = hist.Get_Bin_Widths();
    AMD::Voxel*** Data = hist.Get_Counts();
    m_offset = 0.5*AMD::Vec3(m_cols*m_box_dims[0], m_rows*m_box_dims[1], m_depth*m_box_dims[2]);
    
    float b = min;
    float m = max - min;
    float d;
    for ( int i = 5; i< m_depth; i++){
        for (int j = 0; j < m_rows; j++){
            for ( int k = 0 ; k < m_cols; k++){
                d = (Data[i][j][k].value - b) / m;
                m_counts[count] = d;
                m_centers[count] = Data[i][j][k].Get_Center() - m_offset;
                count++;
            }
        }
    }
    m_num_bins = count;
}


void Voxel_Mesh::Set_Data(PinHole& ph){
    int count = 0;
    AMD::Voxel* Data = ph.Get_Voxels();
    int num = ph.Get_Num_Vox();
    if(!init){Init(13500);}
    m_offset = 0.5*Sim->Sim_Box();
    m_box_dims = AMD::Voxel::lengths;
    iface = ph.Get_Interface();
    for ( int i = 0 ; i < num; i++){
        m_counts[count] = Data[i].value / 2.0;
        m_centers[count] = Data[i].Get_Center() - m_offset;
        count++;
    }
    m_num_bins = count;
}


void Voxel_Mesh::Set_Uniforms(){
    this->m_sh.Set_Uniform_MVP();
    this->m_sh.Set_Uniform_Vec3("dim", m_box_dims);
}

void Voxel_Mesh::Set_Shader(){
    this->m_sh.bind();
    
}



void Voxel_Mesh::Draw() {
    glBindBuffer(GL_ARRAY_BUFFER,m_inst_vb_counts);
    glBufferData(GL_ARRAY_BUFFER, m_num_bins*sizeof(float), (void*)m_counts, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,m_inst_vb_centers);
    glBufferData(GL_ARRAY_BUFFER, m_num_bins*sizeof(AMD::Vec3), (void*)m_centers, GL_DYNAMIC_DRAW);
    this->m_VAO.bind();
    this->m_IBO.bind();
    glDrawElementsInstanced(GL_TRIANGLES,m_IBO.get_num(), GL_UNSIGNED_INT,0, m_num_bins);
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}

void Voxel_Mesh::Clear() {
    for (int i = 0; i < m_num_bins; i++){
        m_counts[i] = 0.;
    }
}

unsigned int Voxel_Mesh::Num_Points() {
    return m_num_bins;
}


void Voxel_Mesh::Center() {
    op->m_Cam.Look_At(m_centers[0]);
}




//#############################################################################


Voxel_Mesh2::Voxel_Mesh2()
:m_sh(shader_file)
{
    Cube cb(1.0);
    VertexBuffer vb(cb.basic_verts, cb.num_verts()*sizeof(AMD::Vertex_Basic));
    m_VAO.Add_Basic_Vertex_Buffer(vb);
    m_IBO.Gen_Buffer(cb.indices,cb.num_idx());
    m_inst_vb_centers = m_VAO.Add_Dynamic_Instance_Buffer(sizeof(AMD::Vec3));
    m_inst_vb_counts = m_VAO.Add_Dynamic_Instance_Buffer(sizeof(float));
}



Voxel_Mesh2::~Voxel_Mesh2()
{
    if(init){
        free(m_counts);
        free(m_centers);
    }
}


void Voxel_Mesh2::Init(int num){
    m_counts = (float*)malloc(num*sizeof(float));
    m_centers = (AMD::Vec3*)malloc(num*sizeof(AMD::Vec3));
    init=true;
}

void Voxel_Mesh2::Set_Data(AMD::Voxel* vox, int num){
    if(!init){Init(num);}
    int count = 0;
    m_offset = 0.5*Sim->Sim_Box();
    m_box_dims = AMD::Voxel::lengths;
    for ( int i = 0 ; i < num; i++){
        m_counts[count] = vox[i].value;
        m_centers[count] = vox[i].Get_Center() - m_offset;
        count++;
    }
    m_num_bins = count;
}


void Voxel_Mesh2::Set_Data(TOPCon &hist) {
    m_cols = hist.Get_Cols();
    m_rows = hist.Get_Rows();
    m_depth = hist.Get_Depth();
    int tot = m_depth*m_rows*m_cols;
    if(!init){Init(tot);}
    int count = 0;
    float min = hist.Get_Min();
    float max = hist.Get_Max();
    m_box_dims = hist.Get_Bin_Widths();
    AMD::Voxel*** Data = hist.Get_Counts();
    //iface = Find_Interfaces(hist);
    m_offset = 0.5*Sim->Sim_Box();
    
    for ( int i = 0; i< m_depth; i++){
        for (int j = 0; j < m_rows; j++){
            for ( int k = 0 ; k < m_cols; k++){
                m_counts[count] = Data[i][j][k].value / 2.0;
                m_centers[count] = Data[i][j][k].Get_Center() - m_offset;
                count++;
            }
        }
    }
    m_num_bins = count;
}

void Voxel_Mesh2::Set_Data(PinHole& ph){
    int count = 0;
    AMD::Voxel* Data = ph.Get_Voxels();
    int num = ph.Get_Num_Vox();
    if(!init){Init(13500);}
    m_offset = 0.5*Sim->Sim_Box();
    m_box_dims = AMD::Voxel::lengths;
    iface = ph.Get_Interface();
    for ( int i = 0 ; i < num; i++){
        m_counts[count] = Data[i].value / 2.4;
        m_centers[count] = Data[i].Get_Center() - m_offset;
        count++;
    }
    m_num_bins = count;
}


void Voxel_Mesh2::Set_Uniforms(){
    this->m_sh.Set_Uniform_MVP();
    this->m_sh.Set_Uniform_Vec3("dim", m_box_dims);
}

void Voxel_Mesh2::Set_Shader(){
    this->m_sh.bind();
    
}



void Voxel_Mesh2::Draw() {
    glBindBuffer(GL_ARRAY_BUFFER,m_inst_vb_counts);
    glBufferData(GL_ARRAY_BUFFER, m_num_bins*sizeof(float), (void*)m_counts, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,m_inst_vb_centers);
    glBufferData(GL_ARRAY_BUFFER, m_num_bins*sizeof(AMD::Vec3), (void*)m_centers, GL_DYNAMIC_DRAW);
    this->m_VAO.bind();
    this->m_IBO.bind();
    glDrawElementsInstanced(GL_TRIANGLES,m_IBO.get_num(), GL_UNSIGNED_INT,0, m_num_bins);
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}

void Voxel_Mesh2::Clear() {
    for (int i = 0; i < m_num_bins; i++){
        m_counts[i] = 0.;
    }
}

unsigned int Voxel_Mesh2::Num_Points() {
    return m_num_bins;
}


void Voxel_Mesh2::Center() {
    op->m_Cam.Look_At(m_centers[0]);
}



//##########################################################################################################
//##########################################################################################################
//##########################################################################################################



Iso_Mesh::Iso_Mesh()
:m_sh(shader_file), grid_sh(grid_shader_file),m_cut(0.25), m_count(0), vox_len(1.0, 1.0, 1.0)
{
    
    m_pos_vbo = m_VAO.Add_Dynamic_Buffer(sizeof(AMD::Vec3));
    m_clr_vbo = m_VAO.Add_Dynamic_Buffer(sizeof(AMD::Vec4));
    m_max_el = 1000;
    m_pos = (AMD::Vec3*)malloc(m_max_el*sizeof(AMD::Vec3));
    m_norms = (AMD::Vec3*)malloc(m_max_el*sizeof(AMD::Vec3));
    m_clrs = (AMD::Vec4*)malloc(m_max_el*sizeof(AMD::Vec4));
    
}

Iso_Mesh::Iso_Mesh(float cut)
:m_sh(shader_file), grid_sh(grid_shader_file),m_cut(cut), m_count(0), vox_len(1.0, 1.0, 1.0)
{
    
    m_pos_vbo = m_VAO.Add_Dynamic_Buffer(sizeof(AMD::Vec3));
    m_clr_vbo = m_VAO.Add_Dynamic_Buffer(sizeof(AMD::Vec4));
    m_max_el = 1000;
    m_pos = (AMD::Vec3*)malloc(m_max_el*sizeof(AMD::Vec3));
    m_norms = (AMD::Vec3*)malloc(m_max_el*sizeof(AMD::Vec3));
    m_clrs = (AMD::Vec4*)malloc(m_max_el*sizeof(AMD::Vec4));
    
}


Iso_Mesh::~Iso_Mesh()
{
    free(m_pos);
    free(m_norms);
    free(m_clrs);
    glDeleteBuffers(1, &m_pos_vbo);
    glDeleteBuffers(1, &m_clr_vbo);
}

void Iso_Mesh::Set_Edges(){
    
    
    m_edge_index[0][0] = 0; m_edge_index[0][1] = 1;
    m_edge_index[1][0] = 1; m_edge_index[1][1] = 2;
    m_edge_index[2][0] = 3; m_edge_index[2][1] = 2;
    m_edge_index[3][0] = 0; m_edge_index[3][1] = 3;
    
    m_edge_index[4][0] = 4; m_edge_index[4][1] = 5;
    m_edge_index[5][0] = 5; m_edge_index[5][1] = 6;
    m_edge_index[6][0] = 7; m_edge_index[6][1] = 6;
    m_edge_index[7][0] = 4; m_edge_index[7][1] = 7;
    
    m_edge_index[8][0] = 0; m_edge_index[8][1] = 4;
    m_edge_index[9][0] = 1; m_edge_index[9][1] = 5;
    m_edge_index[10][0] = 3; m_edge_index[10][1] = 7;
    m_edge_index[11][0] = 2; m_edge_index[11][1] = 6;
    
    
    float dx = 0.5*vox_len.x; float dy = 0.5*vox_len.y; float dz = 0.5*vox_len.z;
    m_edges[0] = AMD::Vec3(dx, 0.0, 0.0);
    m_edges[1] = AMD::Vec3(2*dx, dy, 0.0);
    m_edges[2] = AMD::Vec3(dx, 2*dy, 0.0);
    m_edges[3] = AMD::Vec3(0.0, dy, 0.0);
    
    m_edges[4] = AMD::Vec3(dx, 0.0, 2*dz);
    m_edges[5] = AMD::Vec3(2*dx, dy,2*dz);
    m_edges[6] = AMD::Vec3(dx, 2*dy,2*dz);
    m_edges[7] = AMD::Vec3(0.0, dy, 2*dz);
    
    
    m_edges[8] = AMD::Vec3(0.0, 0.0, dz);
    m_edges[9] = AMD::Vec3(2*dx, 0.0, dz);
    m_edges[10] = AMD::Vec3(0.0, 2*dy, dz);
    m_edges[11] = AMD::Vec3(2*dx, 2*dy, dz);
}


void Iso_Mesh::Set_Data(float*** dat, int nd, int nr, int nc) {
    m_count = 0;
    vox_len = AMD::Vec3(10.0,10.0,10.0);
    Set_Edges();
    std::bitset<8> cube;
    for(int d = 0; d < nd; d++){
        for(int r = 0; r < nr; r++){
            for(int c = 0; c < nc; c++){
                int dp = d + 1; int rp = r+1; int cp = c+1;
                if(dp > nd-1){ dp = 0;}
                if(rp > nr-1){ rp = 0;}
                if(cp > nc-1){ cp = 0;}
                m_offset.z = d*vox_len.z;
                m_offset.y = r*vox_len.y;
                m_offset.x = c*vox_len.x;
                
                
                Curr_Cube[0] = dat[d][r][c];
                Curr_Cube[1] = dat[d][r][cp];
                Curr_Cube[2] = dat[d][rp][cp];
                Curr_Cube[3] = dat[d][rp][c];
                
                Curr_Cube[4] = dat[dp][r][c];
                Curr_Cube[5] = dat[dp][r][cp];
                Curr_Cube[6] = dat[dp][rp][cp];
                Curr_Cube[7] = dat[dp][rp][c];
                
                
                cube[0] = (dat[d][r][c] >= m_cut);
                cube[1] = (dat[d][r][cp] >= m_cut);
                cube[2] = (dat[d][rp][cp] >= m_cut);
                cube[3] = (dat[d][rp][c] >= m_cut);
                
                cube[4] = (dat[dp][r][c] >= m_cut);
                cube[5] = (dat[dp][r][cp] >= m_cut);
                cube[6] = (dat[dp][rp][cp] >= m_cut);
                cube[7] = (dat[dp][rp][c] >= m_cut);
                
                Evaluate_Cube(cube);
            }
        }
    }
    if(!init){
        initial_area = ph_area;
        init = true;
    }
}



void Iso_Mesh::Set_Data(TOPCon &hist) {
    AMD::Vec3 BOX = Sim->Sim_Box();
    m_count = 0;
    ph_area = 0.0;
    int nd = hist.Get_Depth();
    int nr = hist.Get_Rows();
    int nc = hist.Get_Cols();
    vox_len = hist.Get_Bin_Widths();
    Set_Edges();
    AMD::Voxel*** dat = hist.Get_Counts();
    std::bitset<8> cube;
    for(int d = 0; d < nd; d++){
        for(int r = 0; r < nr; r++){
            for(int c = 0; c < nc; c++){
                int dp = d + 1; int rp = r+1; int cp = c+1;
                if(dp > nd-1){ dp = 0;}
                if(rp > nr-1){ rp = 0;}
                if(cp > nc-1){ cp = 0;}
                m_offset.z = d*vox_len.z - 0.5*BOX.z;
                m_offset.y = r*vox_len.y - 0.5*BOX.y;
                m_offset.x = c*vox_len.x - 0.5*BOX.x;
                
                
                Curr_Cube[0] = dat[d][r][c].value;
                Curr_Cube[1] = dat[d][r][cp].value;
                Curr_Cube[2] = dat[d][rp][cp].value;
                Curr_Cube[3] = dat[d][rp][c].value;
                
                Curr_Cube[4] = dat[dp][r][c].value;
                Curr_Cube[5] = dat[dp][r][cp].value;
                Curr_Cube[6] = dat[dp][rp][cp].value;
                Curr_Cube[7] = dat[dp][rp][c].value;
                
                
                cube[0] = (dat[d][r][c].value >= m_cut);
                cube[1] = (dat[d][r][cp].value >= m_cut);
                cube[2] = (dat[d][rp][cp].value >= m_cut);
                cube[3] = (dat[d][rp][c].value >= m_cut);
                
                cube[4] = (dat[dp][r][c].value >= m_cut);
                cube[5] = (dat[dp][r][cp].value >= m_cut);
                cube[6] = (dat[dp][rp][cp].value >= m_cut);
                cube[7] = (dat[dp][rp][c].value >= m_cut);
                
                Evaluate_Cube(cube);
            }
        }
    }
    if(!init){
        initial_area = ph_area;
        init = true;
    }
}



void Iso_Mesh::Set_Data(RHO_3D& rho) {
    m_count = 0;
    ph_area = 0.0;
    int nd = rho.Get_Depth();
    int nr = rho.Get_Rows();
    int nc = rho.Get_Cols();
    vox_len = rho.Get_Bin_Widths();
    Set_Edges();
    AMD::Voxel*** dat = rho.Get_Counts();
    AMD::Vec3 BOX = AMD::Vec3(nc*vox_len[0], nr*vox_len[1], nd*vox_len[2]);

    std::bitset<8> cube;
    for(int d = 0; d < nd; d++){
        for(int r = 0; r < nr; r++){
            for(int c = 0; c < nc; c++){
                int dp = d + 1; int rp = r+1; int cp = c+1;
                if(dp > nd-1){ dp = 0;}
                if(rp > nr-1){ rp = 0;}
                if(cp > nc-1){ cp = 0;}
                m_offset.z = d*vox_len.z - 0.5*BOX.z;
                m_offset.y = r*vox_len.y - 0.5*BOX.y;
                m_offset.x = c*vox_len.x - 0.5*BOX.x;
                
                
                Curr_Cube[0] = dat[d][r][c].value;
                Curr_Cube[1] = dat[d][r][cp].value;
                Curr_Cube[2] = dat[d][rp][cp].value;
                Curr_Cube[3] = dat[d][rp][c].value;
                
                Curr_Cube[4] = dat[dp][r][c].value;
                Curr_Cube[5] = dat[dp][r][cp].value;
                Curr_Cube[6] = dat[dp][rp][cp].value;
                Curr_Cube[7] = dat[dp][rp][c].value;
                
                
                cube[0] = (dat[d][r][c].value >= m_cut);
                cube[1] = (dat[d][r][cp].value >= m_cut);
                cube[2] = (dat[d][rp][cp].value >= m_cut);
                cube[3] = (dat[d][rp][c].value >= m_cut);
                
                cube[4] = (dat[dp][r][c].value >= m_cut);
                cube[5] = (dat[dp][r][cp].value >= m_cut);
                cube[6] = (dat[dp][rp][cp].value >= m_cut);
                cube[7] = (dat[dp][rp][c].value >= m_cut);
                
                Evaluate_Cube(cube);
            }
        }
    }
    if(!init){
        initial_area = ph_area;
        init = true;
    }
}






void Iso_Mesh::Evaluate_Cube(std::bitset<8> number){
    int idx = (int)number.to_ulong();
    cube_table_el cb = Cube_Table::Get()->Table[idx];
    for(int i = 0; i< cb.num_tri; i++){
        Add_Triangle(cb.tri_idx[i]);
    }
}



void Iso_Mesh::Boundary_Wrapped_Index(int* d,int* r, int* c){
    int d_bw = (*d > hist_depth - 1) ? (*d - hist_depth) : (*d >= 0) ? *d : hist_depth + *d;
    int r_bw = (*r > hist_rows - 1) ? *r - hist_rows : (*r >= 0) ? *r : hist_rows + *r;
    int c_bw = (*c > hist_cols - 1) ? *c - hist_cols : (*c >= 0) ? *c : hist_cols + *c;
    *d = d_bw;
    *r = r_bw;
    *c = c_bw;
    
}


void Iso_Mesh::Set_Uniforms(){
    this->m_sh.Set_Uniform_MVP();
}

void Iso_Mesh::Set_Uniforms(Light_Src& light){
    this->m_sh.Set_Uniform_MVP();
    this->m_sh.Set_Uinform_LightSource(light);
    AMD::Mat4 mlp = op->Get_MLP(light);
    m_sh.Set_Uniform_Mat4("u_MLPV", &mlp[0][0]);
}


void Iso_Mesh::Set_ShadowMap(ShadowMap& sm){
    this->m_sh.Set_ShadowMap("ShadowMap", sm);
}


void Iso_Mesh::Set_Shader(){
    this->m_sh.bind();
    
}

void Iso_Mesh::Set_Grid_Shader(){
    float tmp_alpha = 3.75*m_cut;
    this->grid_sh.Set_Uniform_MVP();
    this->grid_sh.Set_Uniform_Float("u_a", tmp_alpha);
    this->grid_sh.bind();
}


void Iso_Mesh::Draw() {
    if(m_count > 40000){
        ;
    }
    glBindBuffer(GL_ARRAY_BUFFER,m_pos_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_count*sizeof(AMD::Vec3), (void*)m_pos, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,m_clr_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_count*sizeof(AMD::Vec4), (void*)m_clrs, GL_DYNAMIC_DRAW);
    this->m_VAO.bind();
    glDrawArrays(GL_TRIANGLES,0, m_count);
    this->m_VAO.unbind();
}



void Iso_Mesh::Look_Up_Table(std::bitset<8> number){
    int count = (int)number.count();
    int val = (int)number.to_ulong();
    if(count > 4){
        number.flip();
        count = 8-count;
    }
    cube_table_el cb = Cube_Table::Get()->Table[val];
    for(int i = 0; i< 3*cb.num_tri; i++){
        Add_Triangle(cb.tri_idx[i]);
    }
    
}


AMD::Vec3 Iso_Mesh::LERP(int idx){
    // if idx = 0, 2, 4, 6, aka( 0-1) then change only the x dir
    // if idx = 1, 3, 5, 7 then change only the y dir
    // if idx > 7 change only z dir
    int vert1 = m_edge_index[idx][0];
    int vert2 = m_edge_index[idx][1];
    float y1 = Curr_Cube[vert1]; // < Curr_Cube[vert2]) ? Curr_Cube[vert1] : Curr_Cube[vert2];
    float y2 = Curr_Cube[vert2]; // < Curr_Cube[vert2]) ? Curr_Cube[vert2] : Curr_Cube[vert1];
    float min = (Curr_Cube[vert1] < Curr_Cube[vert2]) ? Curr_Cube[vert1] : Curr_Cube[vert2];
    float max = (Curr_Cube[vert1] < Curr_Cube[vert2]) ? Curr_Cube[vert2] : Curr_Cube[vert1];
    
    if(min == m_cut){
        min-=0.01;
    }
    if(max == m_cut){
        max+=0.01;
    }
    
    if(idx > 7){
        float slope = (y2 - y1)/vox_len.z;
        float x = (m_cut - y1) / slope;
        if( x<0 || x>vox_len.z){
            ;
        }
        float delta = x - 0.5*vox_len.z;
        return AMD::Vec3(0.0,0.0,delta);
    }
    else if (idx%2){
        float slope = (y2 - y1)/vox_len.y;
        float x = (m_cut - y1) / slope;
        if( x<0 || x>vox_len.y){
            ;
        }
        float delta = x - 0.5*vox_len.y;
        return AMD::Vec3(0.0, delta, 0.0);
    }
    else{
        float slope = (y2 - y1)/vox_len.x;
        float x = (m_cut - y1) / slope;
        if( x<0 || x>vox_len.x){
            ;
        }
        float delta = x - 0.5*vox_len.x;
        return AMD::Vec3(delta, 0.0,0.0);
    }
}

void Iso_Mesh::Add_Triangle(AMD::Ivec3& tri){
    
    // I take in a triangle with three indices
    // 1) read in an index
    Triangle tr;
    for (int i = 0; i<3; i++){
        AMD::Vec3 mod = LERP(tri[i]);
        //mod = 0.0;
        AMD::Vec3 temp = m_edges[tri[i]] + mod;
        AMD::Vec3 p1 = m_offset + temp;
        tr[i] = p1;
        if(m_count < (m_max_el - 1)){
            m_pos[m_count] = p1;
            m_clrs[m_count] = m_clr;
            m_count ++;
        }
        else{
            m_max_el +=1000;
            m_pos = (AMD::Vec3*)realloc(m_pos, m_max_el*sizeof(AMD::Vec3));
            m_norms = (AMD::Vec3*)realloc(m_norms, m_max_el*sizeof(AMD::Vec3));
            m_clrs = (AMD::Vec4*)realloc(m_clrs, m_max_el*sizeof(AMD::Vec4));
            m_pos[m_count] = p1;
            m_count ++;
        }
    }
    
    ph_area += tr.Area();
    return;
}



void Iso_Mesh::Add_Triangle_Test(AMD::Ivec3& tri){
    
    // I take in a triangle with three indices
    // 1) read in an index
    for (int i = 0; i<3; i++){
        AMD::Vec3 temp = m_edges[tri[i]];
        AMD::Vec3 p1 = temp;
        m_pos[m_count] = p1;
        m_count ++;
    }
    
}

float Iso_Mesh::Get_Area(){
    return ph_area - initial_area;
}

void Iso_Mesh::Draw_Test(int idx){
    vox_len = AMD::Vec3(2.5,2.5,2.5);
    Set_Edges();
    cube_table_el cb = Cube_Table::Get()->Table[idx];
    for(int i = 0; i< cb.num_tri; i++){
        Add_Triangle_Test(cb.tri_idx[i]);
    }
    
}

void Iso_Mesh::Compute_Normals(){
    int count = 0;
    for(int i = 0; i < m_count; i+=3){
        AMD::Vec3 center = 0.5*m_pos[i+2] + 0.25*m_pos[i+1] + 0.25*m_pos[i];
        AMD::Vec3 V1 = m_pos[i+1] - m_pos[i];
        AMD::Vec3 V2 = m_pos[i+2] - m_pos[i+1];
        AMD::Vec3 u_norm = V1.cross(V2);
        u_norm.Normalize();
        m_norms[count] = center;
        count ++;
        m_norms[count] = center + 2.0*u_norm;
        count ++;
    }
    num_normals = count;
}

AMD::Vec3* Iso_Mesh::Get_Normals(){
    return m_norms;
}

void Iso_Mesh::Set_Color(AMD::Vec4& clr){
    m_clr = clr;
}






//########################################
//############# GRID ######################
//########################################

Grid_3D::Grid_3D(TOPCon& hist)
:m_sh(shader_file)
{
    AMD::Vec3 BOX = Sim->Sim_Box();
    AMD::Vec3 len = hist.Get_Bin_Widths();
    int depth = hist.Get_Depth();
    int rows =  hist.Get_Rows();
    int cols =  hist.Get_Cols();
    int num = depth*rows*cols;
    m_num_bins = num;
    float dx = len.x; float dy = len.y; float dz = len.z;
    AMD::Vec3 verts[8];
    verts[0] = AMD::Vec3(0.0 , 0.0, 0.0);
    verts[1] = AMD::Vec3(dx ,0.0, 0.0);
    verts[2] = AMD::Vec3(dx ,dy, 0.0);
    verts[3] = AMD::Vec3(0.0 ,dy, 0.0);
    
    verts[4] = AMD::Vec3(0.0 , 0.0,dz);
    verts[5] = AMD::Vec3(dx ,0.0, dz);
    verts[6] = AMD::Vec3(dx ,dy, dz);
    verts[7] = AMD::Vec3(0.0 ,dy, dz);
    
    AMD::Vec3 offset[num];
    float x, y, z;
    z = -0.5*BOX.z;
    int count = 0;
    for(int d = 0; d < depth; d++){
        y = -0.5*BOX.y;
        for (int r = 0; r < rows; r++) {
            x = -0.5*BOX.x;
            for (int c = 0; c < cols; c++){
                offset[count] = AMD::Vec3(x,y,z);
                count ++;
                x+=dx;
            }
            y+=dy;
        }
        z+=dz;
    }
    m_VAO.Add_Static_Buffer(verts, sizeof(AMD::Vec3), 8);
    m_VAO.Add_Static_Instance_Buffer(offset, sizeof(AMD::Vec3), num);
    int indices[24] = {0,1, 1,5, 5,4, 4,0, 0,3, 1,2, 5,6, 4,7, 3,2, 2,6, 6,7, 7,3};
    m_IBO.Gen_Buffer(indices, 24);
}



Grid_3D::Grid_3D(float w)
:m_sh(shader_file)
{
    AMD::Vec3 BOX = Sim->Sim_Box();
    
    int depth = ceil( BOX.z / w);
    int rows =  ceil( BOX.y / w);
    int cols =  ceil( BOX.x / w);
    float dx = w; float dy = w; float dz = w;
    int num = depth*rows*cols;
    m_num_bins = num;
    AMD::Vec3 verts[8];
    verts[0] = AMD::Vec3(0.0 , 0.0, 0.0);
    verts[1] = AMD::Vec3(dx ,0.0, 0.0);
    verts[2] = AMD::Vec3(dx ,dy, 0.0);
    verts[3] = AMD::Vec3(0.0 ,dy, 0.0);
    
    verts[4] = AMD::Vec3(0.0 , 0.0,dz);
    verts[5] = AMD::Vec3(dx ,0.0, dz);
    verts[6] = AMD::Vec3(dx ,dy, dz);
    verts[7] = AMD::Vec3(0.0 ,dy, dz);
    
    AMD::Vec3 offset[num];
    float x, y, z;
    z = -0.5*BOX.z;
    int count = 0;
    for(int d = 0; d < depth; d++){
        y = -0.5*BOX.y;
        for (int r = 0; r < rows; r++) {
            x = -0.5*BOX.x;
            for (int c = 0; c < cols; c++){
                offset[count] = AMD::Vec3(x,y,z);
                count ++;
                x+=dx;
            }
            y+=dy;
        }
        z+=dz;
    }
    m_VAO.Add_Static_Buffer(verts, sizeof(AMD::Vec3), 8);
    m_VAO.Add_Static_Instance_Buffer(offset, sizeof(AMD::Vec3), num);
    int indices[24] = {0,1, 1,5, 5,4, 4,0, 0,3, 1,2, 5,6, 4,7, 3,2, 2,6, 6,7, 7,3};
    m_IBO.Gen_Buffer(indices, 24);
}

Grid_3D::Grid_3D(int nd, int nr, int nc)
:m_sh(shader_file)
{
    float w = 10.0;
    float dx = w; float dy = w; float dz = w;
    int num = nd*nr*nc;
    m_num_bins = num;
    AMD::Vec3 verts[8];
    verts[0] = AMD::Vec3(0.0 , 0.0, 0.0);
    verts[1] = AMD::Vec3(dx ,0.0, 0.0);
    verts[2] = AMD::Vec3(dx ,dy, 0.0);
    verts[3] = AMD::Vec3(0.0 ,dy, 0.0);
    
    verts[4] = AMD::Vec3(0.0 , 0.0,dz);
    verts[5] = AMD::Vec3(dx ,0.0, dz);
    verts[6] = AMD::Vec3(dx ,dy, dz);
    verts[7] = AMD::Vec3(0.0 ,dy, dz);
    
    AMD::Vec3 offset[num];
    float x, y, z;
    z = 0.0;
    int count = 0;
    for(int d = 0; d < nd; d++){
        y = 0.0;
        for (int r = 0; r < nr; r++) {
            x = 0.0;
            for (int c = 0; c < nc; c++){
                offset[count] = AMD::Vec3(x,y,z);
                count ++;
                x+=dx;
            }
            y+=dy;
        }
        z+=dz;
    }
    m_VAO.Add_Static_Buffer(verts, sizeof(AMD::Vec3), 8);
    m_VAO.Add_Static_Instance_Buffer(offset, sizeof(AMD::Vec3), num);
    int indices[24] = {0,1, 1,5, 5,4, 4,0, 0,3, 1,2, 5,6, 4,7, 3,2, 2,6, 6,7, 7,3};
    m_IBO.Gen_Buffer(indices, 24);
}

Grid_3D::Grid_3D()
:m_sh(shader_file)
{
    AMD::Vec3 len(1.0,1.0,1.0);
    int num = 1;
    m_num_bins = num;
    AMD::Vec3 verts[8];
    float dx = 2.5;
    verts[0] = AMD::Vec3(0.0 , 0.0, 0.0);
    verts[1] = AMD::Vec3(dx ,0.0, 0.0);
    verts[2] = AMD::Vec3(dx , dx, 0.0);
    verts[3] = AMD::Vec3(0.0, dx, 0.0);
    
    verts[4] = AMD::Vec3(0.0 , 0.0,dx);
    verts[5] = AMD::Vec3(dx , 0.0, dx);
    verts[6] = AMD::Vec3(dx , dx,  dx);
    verts[7] = AMD::Vec3(0.0 ,dx,  dx);
    
    AMD::Vec3 offset[1];
    
    m_VAO.Add_Static_Buffer(verts, sizeof(AMD::Vec3), 8);
    m_VAO.Add_Static_Instance_Buffer(offset, sizeof(AMD::Vec3), num);
    int indices[24] = {0,1, 1,5, 5,4, 4,0, 0,3, 1,2, 5,6, 4,7, 3,2, 2,6, 6,7, 7,3};
    m_IBO.Gen_Buffer(indices, 24);
}



Grid_3D::~Grid_3D(){}


void Grid_3D::Set_Uniforms(){
    this->m_sh.Set_Uniform_MVP();
}

void Grid_3D::Set_Shader(){
    this->m_sh.bind();
    
}



void Grid_3D::Draw() {
    this->m_VAO.bind();
    this->m_IBO.bind();
    glDrawElementsInstanced(GL_LINES,m_IBO.get_num(), GL_UNSIGNED_INT,0, m_num_bins);
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}


//#####text Render Class#######################

Text_Mesh::Text_Mesh()
:m_sh(shader_file)
{
    Quad qd(2.0);
    m_num_verts = 4;
    for (int i = 0; i<4; i++){
        m_verts[i] = qd.verts[i].pos;
        m_colors[i] = AMD::Vec4(1.0,1.0,0.0,1.0);
    }
    
    m_tex_coords[0] = AMD::Vec2(0.0,1.0);
    m_tex_coords[1] = AMD::Vec2(1.0,1.0);
    m_tex_coords[2] = AMD::Vec2(1.0,0.0);
    m_tex_coords[3] = AMD::Vec2(0.0,0.0);
    
    m_pos_buffer =  m_VAO.Add_Dynamic_Buffer(sizeof(AMD::Vec3));
    m_color_buffer =  m_VAO.Add_Dynamic_Buffer(sizeof(AMD::Vec4));
    m_tex_buffer =  m_VAO.Add_Dynamic_Buffer(sizeof(AMD::Vec2));
    m_IBO.Gen_Buffer(qd.indices,qd.num_idx());
}


Text_Mesh::~Text_Mesh() {}

void Text_Mesh::Set_Uinforms(){
    m_sh.Set_Uniform_MVP();
}


void Text_Mesh::Set_Shader(){
    m_sh.bind();
}

void Text_Mesh::Draw(){
    glBindBuffer(GL_ARRAY_BUFFER,m_pos_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_num_verts*sizeof(AMD::Vec3), (void*)m_verts, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER,m_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_num_verts*sizeof(AMD::Vec4), (void*)m_colors, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,m_tex_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_num_verts*sizeof(AMD::Vec2), (void*)m_tex_coords, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    this->m_VAO.bind();
    this->m_IBO.bind();
    glDrawElements(GL_TRIANGLES,m_IBO.get_num(), GL_UNSIGNED_INT,0);
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}

void Text_Mesh::Render_Text(const char* word, AMD::Vec3 pos, float scale,const char* plane){
    
    int p_val = plane[0] + plane[1];
    AMD::Mat4 rot;
    switch (p_val) {
        case 241:
            break;
        
        case 242:
            rot.Rotate(AMD::Vec3(0.0, 0.0, -halfPI));
            break;
            
        case 243:
            rot.Rotate(AMD::Vec3(0.0, 0.0, -halfPI));
            break;
            
        default:
            break;
    }
    glActiveTexture(GL_TEXTURE0);
    
    m_sh.Set_Uniform_Mat4("plane_rot", &rot[0][0]);
    // iterate through all characters
    for (int i = 0; i < std::strlen(word); i++)
    {
        
        Character ch = CHAR_MAP[word[i]];

        float xs = scale/800;
        float ys = scale/450;
        float xpos = pos.x + (ch.Bearing.x * xs);
        float ypos = pos.y - ((ch.Size.y - ch.Bearing.y) * ys);

        float w = ch.Size.x * xs;
        float h = ch.Size.y * ys;
        // update VBO for each character
        m_verts[0] = AMD::Vec3(xpos,ypos, pos.z);
        m_verts[1] = AMD::Vec3(xpos + w,ypos, pos.z);
        m_verts[2] = AMD::Vec3(xpos + w,ypos + h, pos.z);
        m_verts[3] = AMD::Vec3(xpos,ypos + h, pos.z);
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.m_id);
        
        Draw();
        
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        pos.x += (ch.Advance >> 6) * xs; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindTexture(GL_TEXTURE_2D,0);
}


void Text_Mesh::Render_Tick_Labels(AMD::Vec3* pos, float* values, int num_vals, float size,const char* plane){
    for (int i = 0; i < num_vals; i++){
        std::string word = std::to_string(values[i]);
        Render_Text(word.c_str(), pos[i], size, plane);
    }
    
        
}

//##############################################################################################
Sphere_Mesh::Sphere_Mesh()
:m_sh(shader_file)
{
    Sphere sp(1.0);
    VertexBuffer vb(sp.basic_verts, sp.num_verts()*sizeof(AMD::Vertex_Basic));
    m_VAO.Add_Basic_Vertex_Buffer(vb);
    m_IBO.Gen_Buffer(sp.indices,sp.num_idx());
    m_offset_vbo = m_VAO.Add_Dynamic_Instance_Buffer(sizeof(AMD::Vec3));
    m_types_vbo = m_VAO.Add_Dynamic_Instance_Buffer(sizeof(float));
    
    m_offsets = (AMD::Vec3*)malloc(1000*sizeof(AMD::Vec3));
    m_types = (float*)malloc(1000*sizeof(float));
    
    
}



Sphere_Mesh::~Sphere_Mesh()
{
    free(m_types);
    free(m_offsets);
}



void Sphere_Mesh::Set_Data(float*** dat, int nd, int nr, int nc){
    int count = 0;
    float cut = 0.5;
    for(int d = 0; d< nd; d++){
        for(int r = 0; r < nr; r++){
            for(int c = 0; c < nc; c++){
                m_offsets[count] = AMD::Vec3(10.0*c,10.0*r,10.0*d);
                if(dat[d][r][c] < cut){
                    m_types[count] = 0.0;
                }
                else{
                    m_types[count] = 1.0;
                    
                }
                count ++;
            }
        }
    }
    m_num_sp = count;
    
    
}



void Sphere_Mesh::Set_Shader(){
    this->m_sh.bind();
}


void Sphere_Mesh::Set_Uniforms(){
    this->m_sh.Set_Uniform_MVP();
    this->m_sh.Set_Uniform_Normal();
    
}


void Sphere_Mesh::Draw(){
    glBindBuffer(GL_ARRAY_BUFFER,m_offset_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_num_sp*sizeof(AMD::Vec3), (void*)m_offsets, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,m_types_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_num_sp*sizeof(float), (void*)m_types, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    
    
    this->m_VAO.bind();
    this->m_IBO.bind();
    glDrawElementsInstanced(GL_TRIANGLES,m_IBO.get_num(), GL_UNSIGNED_INT,0, m_num_sp);
    
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}





//##############################################################################################
Vector_Mesh::Vector_Mesh()
:m_sh(shader_file)
{
    m_pos = (AMD::Vec3*)malloc(10000*sizeof(AMD::Vec3));
    m_pos_vbo = m_VAO.Add_Dynamic_Buffer(sizeof(AMD::Vec3));
    
}



Vector_Mesh::~Vector_Mesh()
{
    free(m_pos);
}



void Vector_Mesh::Set_Data(AMD::Vec3* dat, int num_vec){
    m_num = 0;
    for(int i = 0; i < num_vec; i++){
        m_pos[m_num] = dat[i];
        m_num++;
    }
    
}



void Vector_Mesh::Set_Shader(){
    this->m_sh.bind();
}


void Vector_Mesh::Set_Uniforms(){
    this->m_sh.Set_Uniform_MVP();
    
}


void Vector_Mesh::Draw(){
    glBindBuffer(GL_ARRAY_BUFFER,m_pos_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_num*sizeof(AMD::Vec3), (void*)m_pos, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    //glBindBuffer(GL_ARRAY_BUFFER,m_clr_vbo);
    //glBufferData(GL_ARRAY_BUFFER, m_num*sizeof(AMD::Vec4), (void*)m_clr, GL_DYNAMIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER,0);
    
    
    this->m_VAO.bind();
    glDrawArrays(GL_LINES,0,m_num);
    this->m_VAO.unbind();
}





//##############################################################################################
Sim_Box_Mesh::Sim_Box_Mesh()
:m_sh(shader_file)
{
    AMD::Vec3 h_box = 0.5*Sim->Sim_Box();
    float x = h_box.x;
    float y = h_box.y;
    float z = h_box.z;
    m_pos[0] = AMD::Vec3(-x,-y,-z);
    m_pos[1] = AMD::Vec3(x,-y,-z);
    m_pos[2] = AMD::Vec3(x,y,-z);
    m_pos[3] = AMD::Vec3(-x,y,-z);
    
    m_pos[4] = AMD::Vec3(-x,-y,z);
    m_pos[5] = AMD::Vec3(x,-y,z);
    m_pos[6] = AMD::Vec3(x,y,z);
    m_pos[7] = AMD::Vec3(-x,y,z);
    
    m_VAO.Add_Static_Buffer(m_pos, sizeof(AMD::Vec3), 8);
    m_IBO.Gen_Buffer(indices, 24);
}



Sim_Box_Mesh::~Sim_Box_Mesh(){}





void Sim_Box_Mesh::Set_Shader(){
    this->m_sh.bind();
}


void Sim_Box_Mesh::Set_Uniforms(){
    this->m_sh.Set_Uniform_MVP();
    
}


void Sim_Box_Mesh::Draw(){
    this->m_VAO.bind();
    this->m_IBO.bind();
    glDrawElements(GL_LINES,m_IBO.get_num(), GL_UNSIGNED_INT,0);
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}


//#####################################################################################################
//#####################################################################################################
//#####################################################################################################
//#####################################################################################################

Wire_Frame::Wire_Frame()
:m_sh(shader_file)
{}



Wire_Frame::~Wire_Frame() {
    if(init){
        free(m_verts);
    }
    
}



void Wire_Frame::Set_Data(const char* file){
    int count = 0;
    Data_2D dat(file);
    m_w = 60;
    m_h = 90;
    float min = 100000000000.0;
    float max = -10000000000.0;
    float x, y, val;
    float* vals = (float*)malloc(100000*sizeof(float));
    unsigned int tri_indices[500000];
    unsigned int line_indices[500000];
    if(!init){
        m_verts = (AMD::Vec3*)malloc(dat.m_num_rows*sizeof(AMD::Vec3));
        init = true;
    }
    
    for(int i = 0; i < dat.m_num_rows; i++){
        val = dat.m_dat[i][0];
        min = (val < min) ? val : min;
        max = (val > max) ? val : max;
        vals[i] = val;
        
    }
    printf("min = %.6f max = %.6f\n",min,max);
    unsigned int i_count = 0;
    unsigned int l_count = 0;
    x = -0.5*m_w;
    y = -0.5*m_h;
    for(int i = 0; i < m_h; i++){
        x = -0.5*m_w;
        for(int j = 0; j<m_w; j++){
            val = (vals[count] - min) / (max - min);
            m_verts[count] = AMD::Vec3(x,y,30.0*val);
            count ++;
            x+=1.0;
            if(i == (m_h - 1)){
                continue;
            }
            
            if(j % (m_w - 1) || j == 0){
                //line indices
                //   x
                //   |
                //   x
                line_indices[l_count] = (i*m_w) + j;
                line_indices[l_count + 1] = (i*m_w) + j + 1;
                l_count +=2;
                line_indices[l_count] = (i*m_w) + j;
                line_indices[l_count + 1] = ((i+1)*m_w) + j;
                l_count +=2;
                // triangle indices
                
                tri_indices[i_count] = (i*m_w) + j;
                tri_indices[i_count + 1] = (i*m_w) + j + 1;
                tri_indices[i_count + 2] = ((i+1)*m_w) + j + 1;
                // x--x
                i_count +=3;
                tri_indices[i_count] = ((i+1)*m_w) + j;
                tri_indices[i_count + 1] = (i*m_w) + j;
                tri_indices[i_count + 2] = ((i+1)*m_w) + j + 1;
                // x--x
                i_count +=3;
            }
            
            else{
                line_indices[l_count] = (i*m_w) + j;
                line_indices[l_count + 1] = ((i+1)*m_w) + j;
                l_count +=2;
            }
        
        }
        y+=1.0;
    }
  
    m_num = count;
    m_VAO.Add_Static_Buffer(m_verts, sizeof(AMD::Vec3), m_num);
    m_tri_IBO.Gen_Buffer(tri_indices, i_count);
    m_line_IBO.Gen_Buffer(line_indices, l_count);
    free(vals);
}



void Wire_Frame::Set_Uniforms(){
    this->m_sh.Set_Uniform_MVP();
}

void Wire_Frame::Set_Shader(){
    this->m_sh.bind();
    
}



void Wire_Frame::Draw(){
    float s = 1.0;
    this->m_sh.Set_Uniform_Float("clr_scale",s);
    glBindBuffer(GL_ARRAY_BUFFER,m_inst_vb);
    glBufferData(GL_ARRAY_BUFFER, m_num*sizeof(AMD::Vec3), (void*)m_verts, GL_DYNAMIC_DRAW);
    this->m_VAO.bind();
    this->m_tri_IBO.bind();
    glDrawElements(GL_TRIANGLES, m_tri_IBO.get_num(), GL_UNSIGNED_INT,0);
    this->m_tri_IBO.unbind();
    
    s = 0.2;
    this->m_sh.Set_Uniform_Float("clr_scale",s);
    this->m_line_IBO.bind();
    glDrawElements(GL_LINES, m_line_IBO.get_num(), GL_UNSIGNED_INT,0);
    this->m_line_IBO.unbind();
    this->m_VAO.unbind();
}



