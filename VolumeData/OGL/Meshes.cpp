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
#include <map>


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

char* zero_to_string(int percsion){
    int s = percsion + 3;
    char* ret = (char*)malloc(s*sizeof(char));
    ret[0] = '0';
    ret[1] = '.';
    for (int i = 2; i < s - 1; i++){
        ret[i] = '0';
    }
    ret[s-1] = 0;
    return ret;
}


char* ftos(float val, int percsion){
    if(abs(val) < 0.001){ return zero_to_string(percsion);}
    
    bool neg = val < 0.0;
    bool less_1 = abs(val) < 1.0;
    if(neg){val *= -1.0;}
    int start = pow(10, percsion) * val;
    int size = ceil(log10(start)) + 2;
    //---------adjustments------------
    if(neg){size++;}
    if(less_1){size++;}
    bool mod10 = start % 10;
    if (!mod10){size++;}
    //--------------------------
    
    
    int count = size - 1;
    char* ret = (char*)malloc(size*sizeof(char));
    ret[count] = 0;
    count --;
    for (int i = start; i >= 1; i/=10){
        char c = (i % 10) + 48;
        //std::cout << i << " % 10 = " << i % 10 << c << "\n";
        ret[count] = c;
        count --;
        if(count == size -(percsion + 2)){ret[count] = '.'; count --;}
    }
    if(neg){
        
        ret[0] = '-';
        if(less_1){ ret[1] = '0';}
        
    }
    else if(less_1){ ret[0] = '0';}
    return ret;
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
Atoms_Mesh::Atoms_Mesh(std::string atomfile)
:m_sh(shader_file), file_name(atomfile)
{
    Sphere sp(1.0);
    VertexBuffer vb(sp.verts, sp.num_verts()*sizeof(AMD::Vertex));
    m_VAO.Add_Vertex_Buffer(vb);
    m_IBO.Gen_Buffer(sp.indices,sp.num_idx());
    Add_Instance_Layout();
    //Bind();
    
    
}



Atoms_Mesh::~Atoms_Mesh() {}


void Atoms_Mesh::Add_Instance_Layout(){

    m_num_atoms = Sim->Num_Atoms();
    AMD::Vec3 off_set[m_num_atoms];
    float types[m_num_atoms];
    for (int i = 0; i<m_num_atoms; i++){
        off_set[i] = 1.0;
        types[i] =   1.0;
    }
    
    m_VAO.Add_Static_Instance_Buffer(off_set,sizeof(AMD::Vec3), m_num_atoms);
    m_VAO.Add_Static_Instance_Buffer(types,sizeof(float), m_num_atoms);
}

void Atoms_Mesh::Bind(){
    this->m_sh.bind();
    this->m_VAO.bind();
    this->m_IBO.bind();
    
    
}

void Atoms_Mesh::UnBind(){
    this->m_sh.unbind();
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}

void Atoms_Mesh::Draw(int num_draws){
    this->m_VAO.bind();
    this->m_IBO.bind();
    if (num_draws == 1){
        glDrawElements(GL_TRIANGLES,m_IBO.get_num(), GL_UNSIGNED_INT,0);
    }
    else{
        glDrawElementsInstanced(GL_TRIANGLES,m_IBO.get_num(), GL_UNSIGNED_INT,0, m_num_atoms);
    }
    
}


unsigned int Atoms_Mesh::num_idx(){
    return this->m_IBO.get_num();
}


void Atoms_Mesh::Set_Op(Light_Src& light){
    this->m_sh.bind();
    this->m_sh.Set_Uniform_MVP();
    this->m_sh.Set_Uniform_Normal();
    this->m_sh.Set_Uinform_LightSource(light);
    
}



//##############################################################3
Quad_Mesh::Quad_Mesh()
{
    Quad qd(2.0);
    VertexBuffer vb(qd.verts, qd.num_verts()*sizeof(AMD::Vertex));
    m_VAO.Add_Vertex_Buffer(vb);
    m_IBO.Gen_Buffer(qd.indices,qd.num_idx());
}

Quad_Mesh::Quad_Mesh(AMD::Vec3 LL, AMD::Vec3 LR, AMD::Vec3 UR, AMD::Vec3 UL)
{
    Quad qd(LL,LR,UR,UL, "cw");
    VertexBuffer vb(qd.verts, qd.num_verts()*sizeof(AMD::Vertex));
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
    VertexBuffer vb(cb.verts, cb.m_num_verts*sizeof(AMD::Vertex));
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
    VertexBuffer vb(cb.verts, cb.num_verts()*sizeof(AMD::Vertex));
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
{}

Voxel_Mesh::Voxel_Mesh(Hist_3D &hist)
:m_sh(shader_file), m_cols(hist.Get_Cols()), m_rows(hist.Get_Rows()), m_depth(hist.Get_Depth())
{
    m_num_bins = m_rows*m_cols*m_depth;
    m_counts = (float*)malloc(m_num_bins*sizeof(float));
    AMD::Vec3 temp_centers[m_num_bins];
    AMD::Vec3 bw = hist.Get_Bin_Widths();
    
    float x;
    float y;
    float z = -0.5*m_depth*bw.z;
    int count = 0;
    for (int i = 0; i< m_depth; i++){
        y = -0.5* m_rows * bw.y;
        for (int j = 0; j< m_rows; j++){
            x = -0.5*m_cols*bw.x;
            for (int k = 0; k < m_cols; k++) {
                temp_centers[count] = AMD::Vec3(x,y,z);
                x+= bw.x;
                count++;
            }
            y+= bw.y;
        }
        z+= bw.z;
    }
    
    Set_Data(hist);
    Cube cb(0.9*bw.x);
    VertexBuffer vb(cb.verts, cb.num_verts()*sizeof(AMD::Vertex));
    m_VAO.Add_Vertex_Buffer(vb);
    m_IBO.Gen_Buffer(cb.indices,cb.num_idx());
    m_VAO.Add_Static_Instance_Buffer(temp_centers,sizeof(AMD::Vec3), m_num_bins);
    m_inst_vb_counts = m_VAO.Add_Dynamic_Instance_Buffer(sizeof(float));
    
}

Voxel_Mesh::~Voxel_Mesh()
{
    free(m_counts);
    //free(m_centers);
}

void Voxel_Mesh::Set_Data(float*** data, int rows, int cols, int depth) {
    return;
}

void Voxel_Mesh::Set_Data(Hist_3D &hist) {
    int count = 0;
    float min = 0.0;//hist.Get_Min();
    float max = 2.4;//hist.Get_Max();
    
    float*** tmp_counts = hist.Get_Counts();
    for ( int i = 0; i< m_depth; i++){
        for (int j = 0; j < m_rows; j++){
            for ( int k = 0 ; k < m_cols; k++){
                m_counts[count] = (tmp_counts[i][j][k] - min)/(max-min);
                count++;
            }
        }
    }
}

void Voxel_Mesh::Set_Uniforms(Light_Src& light){
    this->m_sh.Set_Uniform_MVP();
}

void Voxel_Mesh::Set_Shader(){
    this->m_sh.bind();
    
}



void Voxel_Mesh::Draw() {
    glBindBuffer(GL_ARRAY_BUFFER,m_inst_vb_counts);
    glBufferData(GL_ARRAY_BUFFER, m_num_bins*sizeof(float), (void*)m_counts, GL_DYNAMIC_DRAW);
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
