//
//  Computes.cpp
//  VolumeData
//
//  Created by Andrew Diggs on 3/22/23.
//

#include "Computes.hpp"
#include "Atomic.hpp"
#include "Comp_Kernel.hpp"
#include "OGL/Meshes.hpp"
#include "FileIO.hpp"

extern Simulation* Sim;
static Vox_Sphere sp;
static AMD::Ivec3 dims;


static void Print_Result(float* res, int num){
    for (int i = 0; i < num; i++){
        if(res[i] > 0){
            printf("result[%d] = %0.3f\n",i, res[i]);
        }
    }
}



static float Boundary_Wrapped_Dist(AMD::Vec3 A, AMD::Vec3 B){
    AMD::Vec3 box = Sim->Sim_Box();
    float dist_sq = 0.0;
    for (int i = 0; i< 3; i++){
        float delta = abs(A[i]-B[i]);
        if(delta > 0.5*box[i]){
            float x1 = (A[i] < B[i]) ? A[i] : B[i];
            float x2 = (A[i] < B[i]) ? B[i] : A[i];
            dist_sq += ((x1 + box[i]) - x2) * ((x1 + box[i]) - x2);
        }
        else{dist_sq += delta * delta;}
    }
    
    return sqrt(dist_sq);
}




static float LERP(float x1, float y1, float x2, float y2, float y){
    float m = (y1 - y2) / (x1 - x2);
    float b = y1 - x1*m;
    return (y - b) / m;
}





AMD::Vec2 Find_Interfaces(TOPCon& dat){
    AMD::Voxel*** voxels = dat.Get_Counts();
    AMD::Ivec3 dims = dat.Get_Dims();
    AMD::Vec3 tmp_vox_lens = dat.Get_Bin_Widths();
    float ave_vals[dims.d];
    float z_vals[dims.d];
    float div = (float)(dims.r*dims.c);
    float ave = 0.0;
    for(int d = 0; d < dims.d; d++){
        z_vals[d] = voxels[d][0][0].Get_Center().z;
        ave = 0.0;
        for (int r = 0; r < dims.r; r++) {
            for (int c = 0; c < dims.c; c++){
                ave += voxels[d][r][c].value;
                
            }
        }
        ave/=div;
        ave_vals[d] = ave;
        //printf("Z = %.2f x = %.2f\n", z_vals[d], ave_vals[d]);
    }
    
    float prev_z, z;
    float prev, val;
    float cut = 0.5;
    float low = 0.0;
    float hi = 0.0;
    int p_idx;
    
    for(int i = 0; i<dims.d;i++){
        
        if(i == 0){
            p_idx = dims.d - 1;
            prev_z = z_vals[p_idx];
        }
        else{
            p_idx = i - 1;
            prev_z = z_vals[p_idx];
        }
        z = z_vals[i];
        if(z< 2.7){
            prev_z = z_vals[p_idx] - Sim->Sim_Box().z;
        }
        prev = ave_vals[p_idx];
        val = ave_vals[i];
        
       //printf("Z = %.2f val = %.2f\n", z, val);
        if(prev > cut && val < cut){
            //hi = prev_z - del;
            hi =  LERP(prev_z, prev, z, val, cut);
            //printf("ave = %.2f z = %.2f\n", ave, z);
        }
        
        if(prev < cut && val > cut){
            //low = prev_z + del;
            low = LERP(prev_z, prev, z, val, cut);
            
        }
    }
    if(low < 0.1){
        ;
    }
    printf("low = %.2f High = %.2f\n", low, hi);
    printf("###########################\n");
    return AMD::Vec2(low,hi);
}







AMD::Vec3 Compute_Weights(float d1, float d2, float rad){
    float dl = rad - d1;
    float dr = rad - d2;
    float weight_left = AMD::Max(dl, 0.0);
    float weight_right = AMD::Max(dr, 0.0);
    float weight_middle = 1.0 - weight_left - weight_right;
    float N = weight_left + weight_middle + weight_right;
    return AMD::Vec3(weight_left/N, weight_middle/N, weight_right/N);
}


Vox_Sphere::Vox_Sphere()
{
    int l = 5;
    float dist = 0.;
    int count = 0;
    
    for (int i = -l; i<l; i++) {
        for (int j = -l; j<l; j++){
            for (int k = -l; k < l; k++) {
                dist = sqrt(i*i + j*j + k*k);
                if(dist <= r){
                    m_el = (AMD::Vec3*)realloc(m_el, (count + 1) * sizeof(AMD::Vec3));
                    m_el[count] = AMD::Vec3(k/r, j/r, i/r);
                    count ++;
                }
            }
        }
    }
    m_num_el = count;
    
}

Vox_Sphere::~Vox_Sphere(){
    free(m_el);
}


//###################################################
//################# SET Class #######################
//###################################################

Set::Set():m_ID(0), num_el(0), min_z(1000), max_z(-1000){}
Set::Set(int num):m_ID(num), num_el(0), min_z(1000), max_z(-1000){}

void Set::Push_El(AMD::Voxel& other){
    for(int i = 0; i < num_el; i++){
        if(m_vox[i] == other){
            return;
            
        }
    }
    min_z = (other.d < min_z) ? other.d : min_z;
    max_z = (other.d > max_z) ? other.d : max_z;
    if((num_el + 1) > 4500){
        printf("Set Overflow!!!\n");
        exit(2);
    }
    
    other.in_set = true;
    m_vox[num_el] = other;
    num_el ++;
    
}


bool Set::Check_Neighbors(AMD::Voxel& other){
    bool nebs = false;
    int lower;
    int upper;
    if(num_el == 0){return false;}
    for(int i = num_el - 1; i >= 0; i--){
        int dd = abs(m_vox[i].d - other.d);
        if (dd > (dims.d/2)){
            lower = (m_vox[i].d < other.d) ? m_vox[i].d : other.d;
            upper = (m_vox[i].d < other.d) ? other.d : m_vox[i].d;
            dd = abs( upper - (lower + dims.d));
        }
        
        int dr = abs(m_vox[i].r - other.r);
        if (dr > (dims.r/2)){
            lower = (m_vox[i].r < other.r) ? m_vox[i].r : other.r;
            upper = (m_vox[i].r < other.r) ? other.r : m_vox[i].r;
            dr = abs( upper - (lower + dims.r));
        }
        
        int dc = abs(m_vox[i].c - other.c);
        if (dc > (dims.c/2)){
            lower = (m_vox[i].c < other.c) ? m_vox[i].c : other.c;
            upper = (m_vox[i].c < other.c) ? other.c : m_vox[i].c;
            dc = abs( upper - (lower + dims.c));
        }
        //if(dd == 0 && dr == 0 && dc == 0){return false;}
        if(dd < 2 && dr < 2 && dc < 2){return true;}
        else{continue;}
    }
    return nebs;
}


void Set::Merge(Set& other){
    for(int i = 0; i<other.Num_El(); i++){
        Push_El(other.m_vox[i]);
    }
    
    //other.num_el = 0;
}


int Set::Num_El(){return num_el;}

float Set::Delta_Z(){return (float)(max_z - min_z);}

void Set::Print(){
    printf("### Vol: %d ###\n", m_ID);
    for (int i = 0; i<num_el;i++){
        printf("(%d,%d,%d) ",m_vox[i].d, m_vox[i].r, m_vox[i].c);
    }
    printf("\n");
}



bool Merge_Check(Set& A, Set& B){
    bool nebs = false;
    if(A.Num_El() == 0 || B.Num_El() == 0){
        return false;
        
    }
    for(int i = 0; i< B.Num_El(); i++){
        nebs = A.Check_Neighbors(B.m_vox[i]);
        if(nebs){return true;}
    }
    return nebs;
}




/*
 ##############################################################################
 #                                                                            #
 #                                                                            #
 #           _     _     _       _____   ___________   _____    _______       #
 #          | |   | |   |_|     /  ___| |____   ____| |___  \  |  ___  \      #
 #          | |   | |    _     /  /          |  |         \  | | |   \  \     #
 #          | |___| |   | |    |  |          |  |       __/  | | |    \  \    #
 #          |  ___  |   | |    \  \          |  |      |__   | | |     |  |   #
 #          | |   | |   | |     \  \         |  |         \  | | |    /  /    #
 #          | |   | |   | |    __\  |        |  |       __/  | | |___/  /     #
 #          |_|   |_|   |_|   |_____/        |__|      |____/  |_______/      #
 #                                                                            #
 ##############################################################################
 */






TOPCon::TOPCon()
:init(false),m_rows(0), m_cols(0), m_depth(0), m_min(1000.0), m_max(-10000.0), m_num_vox(0)
{
    
}

TOPCon::~TOPCon() {
    if(init){
        for (int i = 0; i< m_depth; i++){
            for (int j = 0; j< m_rows; j++){
                free(m_voxels[i][j]);
                free(m_temp_values[i][j]);
                free(counts_Si[i][j]);
                free(counts_Ox[i][j]);
            }
            free(m_voxels[i]);
            free(m_temp_values[i]);
            free(counts_Si[i]);
            free(counts_Ox[i]);
        }
        free(m_voxels);
        free(m_temp_values);
        free(counts_Si);
        free(counts_Ox);
    }
}


void TOPCon::Init(float bw){
    AMD::Vec3 Box = Sim->Sim_Box();
    int cols = (ceil(Box.x)/bw);
    int rows = (ceil(Box.y)/bw);
    int depth = (ceil(Box.z)/bw);
    
    dims = AMD::Ivec3(depth, rows, cols);
    
    m_bin_width.x = Box.x/(cols + 0.001);
    m_bin_width.y = Box.y/(rows + 0.001);
    m_bin_width.z = Box.z/(depth + 0.001);
    //12,19,19
    AMD::Voxel::lengths = m_bin_width;
    
    m_rows = rows; m_cols = cols; m_depth = depth;
    m_num_vox = m_depth*m_rows*m_cols;
    
    m_bin_vol = m_bin_width.x*m_bin_width.y*m_bin_width.z;
    
    m_voxels = (AMD::Voxel***)malloc(m_depth*sizeof(AMD::Voxel**));
    m_temp_values = (float***)malloc(m_depth*sizeof(float**));
    counts_Si = (float***)malloc(m_depth*sizeof(float**));
    counts_Ox = (float***)malloc(m_depth*sizeof(float**));
    
    for (int i = 0; i< m_depth; i++){
        m_voxels[i] = (AMD::Voxel**)malloc(m_rows*sizeof(AMD::Voxel*));
        m_temp_values[i] = (float**)malloc(m_rows*sizeof(float*));
        counts_Si[i] = (float**)malloc(m_rows*sizeof(float*));
        counts_Ox[i] = (float**)malloc(m_rows*sizeof(float*));
        for (int j = 0; j< m_rows; j++){
            m_voxels[i][j] = (AMD::Voxel*)malloc(m_cols* sizeof(AMD::Voxel));
            m_temp_values[i][j] = (float*)malloc(m_cols*sizeof(float));
            counts_Si[i][j] = (float*)malloc(m_cols*sizeof(float));
            counts_Ox[i][j] = (float*)malloc(m_cols*sizeof(float));
            for (int k = 0; k < m_cols; k++) {
                m_voxels[i][j][k] = AMD::Voxel(i, j, k);
                m_voxels[i][j][k].Set_Center();
                m_temp_values[i][j][k] = 0.;
                counts_Si[i][j][k] = 0.;
                counts_Ox[i][j][k] = 0.;
            }
        }
    }
    init = true;
}



void TOPCon::Clear(){
    m_min = 500000.;
    m_max = 0.;
    for (int i = 0; i< m_depth; i++){
        for (int j = 0; j< m_rows; j++){
            for (int k = 0; k < m_cols; k++) {
                m_voxels[i][j][k].Clear();
                m_temp_values[i][j][k] = 0.;
                counts_Si[i][j][k] = 0.;
                counts_Ox[i][j][k] = 0.;
            }
        }
    }
    
}




void TOPCon::Boundary_Wrapped_Index(int* d,int* r, int* c){
    int d_bw = (*d > m_depth - 1) ? (*d - m_depth) : (*d >= 0) ? *d : m_depth + *d;
    int r_bw = (*r > m_rows - 1) ? *r - m_rows : (*r >= 0) ? *r : m_rows + *r;
    int c_bw = (*c > m_cols - 1) ? *c - m_cols : (*c >= 0) ? *c : m_cols + *c;
    *d = d_bw;
    *r = r_bw;
    *c = c_bw;
    
}


void TOPCon::Compute_Density(float bin_width) {
    if(!(this -> init)){Init(bin_width);}
   
    else{Clear();}
    
    // make an array of ints that can contain the possible
    AMD::Vec3 tmp;
    int type_indx;
    for(int i = 0; i < Sim->Num_Atoms(); i++){
        tmp = Sim->Atoms()[i].Get_Coords();
        type_indx = Sim->Atoms()[i].Get_Type() - 1;
        Compute_Dist_Value(tmp, type_indx);
    }
    //Set_MinMax();
}



void TOPCon::Compute_Dist_Value(AMD::Vec3& coords, int type){
    
    // 1) computed distances to bin_edges
    float radius = Sim->radii[type];
    float mass = Sim->masses[type];
    
    float r_scale = radius;
    float mass_frac = mass/sp.m_num_el;
    int d, r, c;
    AMD::Vec3 delta;
    AMD::Vec3 shifted;
    for (int i = 0; i < sp.m_num_el; i++){
        delta = sp.m_el[i]*r_scale;
        shifted = coords + delta;
        c = round(shifted.x/m_bin_width.x);
        r = round(shifted.y/m_bin_width.y);
        d = round(shifted.z/m_bin_width.z);
        Boundary_Wrapped_Index(&d, &r, &c);
        m_voxels[d][r][c].value += mass_frac;
      
    }
    
}

float TOPCon::Average(int depth, int row, int col){
    // 1) computed distances
    float test = 0.0;
    int size = 1;
    for (int i = -size; i < size +1; i++){
        for (int j = -size; j < size +1; j++){
            for (int k = -size; k < size +1; k++){
                int a = depth + i; int b = row + j; int c = col + k;
                Boundary_Wrapped_Index(&a, &b, &c);
                test += m_temp_values[a][b][c];
        
            }

        }
    }
    float div = (2*size + 1)*(2*size + 1)*(2*size + 1);
    return test / div;
}

void TOPCon::Set_MinMax() {
    float Density_Mod = 1.66 / (m_bin_vol);
    for(int d = 0; d < m_depth; d++){
        for (int r = 0; r < m_rows; r++) {
            for (int c = 0; c < m_cols; c++){
                m_voxels[d][r][c].value*=Density_Mod;
                m_min = AMD::Min(m_voxels[d][r][c].value, m_min);
                m_max = AMD::Max(m_voxels[d][r][c].value, m_max);
                
            }
        }
    }
}


void Compute_Density(){
    
}




void TOPCon::Set_Fraction_X(AMD::Vec3& coords, int type){
    AMD::Vec3 tmp;
    int d, r, c;
    float frac = 1.0/(float)sp.m_num_el;
    for (int i = 0; i < sp.m_num_el; i++){
        tmp = coords + sp.m_el[i];
        c = round(tmp.x/m_bin_width.x);
        r = round(tmp.y/m_bin_width.y);
        d = round(tmp.z/m_bin_width.z);
        Boundary_Wrapped_Index(&d, &r, &c);
        switch (type) {
            case 1:
                counts_Si[d][r][c] += frac;
                break;
                
            case 2:
                counts_Ox[d][r][c] +=frac;
                break;
                
            default:
                break;
        }
    }
}


void TOPCon::Set_Fraction_X(Atom& at){
    AMD::Vec3 coords = at.Get_Coords();
    int type = at.Get_Type();
    AMD::Vec3 tmp;
    int d, r, c;
    float frac = 1.0/(float)sp.m_num_el;
    for (int i = 0; i < sp.m_num_el; i++){
        tmp = coords + sp.m_el[i];
        c = round(tmp.x/m_bin_width.x);
        r = round(tmp.y/m_bin_width.y);
        d = round(tmp.z/m_bin_width.z);
        Boundary_Wrapped_Index(&d, &r, &c);
        switch (type) {
            case 1:
                counts_Si[d][r][c] += frac;
                break;
                
            case 2:
                counts_Ox[d][r][c] +=frac;
                break;
                
            default:
                break;
        }
    }
  
}


void TOPCon::Compute_X(float bin_width){
    if(!(this -> init)){Init(bin_width);}
   
    else{Clear();}
    
    // make an array of ints that can contain the possible
    AMD::Vec3 tmp;
    int type;
    for(int i = 0; i < Sim->Num_Atoms(); i++){
        tmp = Sim->Atoms()[i].Get_Coords();
        type = Sim->Atoms()[i].Get_Type();
        Set_Fraction_X(tmp, type);
        
    }
    
    
    //Set the voxel values.
    for(int d = 0; d < m_depth; d++){
        for (int r = 0; r < m_rows; r++) {
            for (int c = 0; c < m_cols; c++){
                //m_voxels[d][r][c].value = Average_X(d, r, c);
                if(counts_Si[d][r][c] > 0.1){
                    m_voxels[d][r][c].value = counts_Ox[d][r][c] / counts_Si[d][r][c];
                }
                else{m_voxels[d][r][c].value = 2.0;}
                m_min = AMD::Min(m_voxels[d][r][c].value, m_min);
                m_max = AMD::Max(m_voxels[d][r][c].value, m_max);
            }
        }
    }
}



void TOPCon::GPU_Compute_X(float bin_width){
    if(!(this -> init)){Init(bin_width);}
   
    else{Clear();}
    //AMD::Timer tr("Build Hist");
    //AMD::Vec2 test = Find_Interface();
    int num_atoms = Sim->Num_Atoms();
    CL_Atom ats[num_atoms];
    Atom* atoms = Sim->Atoms();
    int w = m_cols;
    int h = m_rows;
    int d = m_depth;
    cl_int3 k_dims;
    k_dims.s[0] = w;
    k_dims.s[1] = h;
    k_dims.s[2] = d;
    int tot = w*h*d;
    
    cl_float3 box;
    box.s[0] = Sim->Sim_Box().x;
    box.s[1] = Sim->Sim_Box().y;
    box.s[2] = Sim->Sim_Box().z;
    float result[tot];
    size_t dims[3] = {(unsigned long)w, (unsigned long)h,(unsigned long)d};
    for(int i =0; i < num_atoms; i++){
        ats[i] = atoms[i];
        if(i < tot){
            result[i] = 0.;
        }
    }
    
    CL_Program prgm("AMD_Hist.cl");
    
    CL_Kernel kr = prgm.Create_Kernel("cl_Hist");
    
    kr.Set_Arg(Input, sizeof(ats), ats);
    kr.Set_Arg(Output, sizeof(result), result);
    kr.Set_Arg(Constant, sizeof(int), &num_atoms);
    kr.Set_Arg(Constant, sizeof(cl_int3), &k_dims);
    kr.Set_Arg(Constant, sizeof(cl_float3), &box);
    
    kr.Set_Global_Dimensions(dims);
    
    
    kr.Compute();
    kr.Read_Output(sizeof(result), result);
    
    
    int r_id = 0;
    for(int d = 0; d < m_depth; d++){
        for (int r = 0; r < m_rows; r++) {
            for (int c = 0; c < m_cols; c++){
                r_id = d*(m_rows*m_cols) + r*(m_cols) + c;
                m_voxels[d][r][c].value = result[r_id];
                m_min = AMD::Min(m_voxels[d][r][c].value, m_min);
                m_max = AMD::Max(m_voxels[d][r][c].value, m_max);
            }
        }
    }
    
}



float TOPCon::Average_X(int depth, int row, int col){
    // 1) computed distances
    float num_Si = 0;
    float num_Ox = 0;
    int size = 1;
    float div = (float)((2*size + 1)*(2*size + 1)*(2*size + 1));
    for (int i = -size; i < size +1; i++){
        for (int j = -size; j < size +1; j++){
            for (int k = -size; k < size +1; k++){
                int a = depth + i; int b = row + j; int c = col + k;
                Boundary_Wrapped_Index(&a, &b, &c);
                num_Si+=counts_Si[a][b][c];
                num_Ox+=counts_Ox[a][b][c];
        
            }

        }
    }
    m_voxels[depth][row][col].num_Si = num_Si / div;
    m_voxels[depth][row][col].num_Ox = num_Ox / div;
    if(num_Si < 0.01){
        if(num_Ox > 0.02){
            return 2.0;
        }
        else{
            return 0.0;
        }
    }
    else{
        float ret = (float)num_Ox / (float)num_Si;
        if(ret > 2.0){
            //printf(" x = %.2f\n",ret);
        }
        return ret;
    }
    
}



void TOPCon::Compute_Hcon(float bin_width) {
    if(!(this -> init)){Init(bin_width);}
   
    else{Clear();}
    
    // make an array of ints that can contain the possible
    AMD::Vec3 tmp;
    int type_indx;
    int d;
    float hcons[m_depth];
    for(int i = 0; i<m_depth;i++){
        hcons[i] = 0.;
    }
    for(int i = 0; i < Sim->Num_Atoms(); i++){
        tmp = Sim->Atoms()[i].Get_Coords();
        type_indx = Sim->Atoms()[i].Get_Type() - 1;
        if(type_indx == 2){
            d = round(tmp.z/m_bin_width.z);
            //Boundary_Wrapped_Index(&d, &r, &c);
            hcons[d] += 1.0;
        }
    }
    
    
    //Set the voxel values.
    float ave = 0.0;
    float zcon = Sim->Num_Atoms() / (float)m_depth;
    float div = (float)(zcon * m_cols * m_rows);
    printf("###############################################\n");
    printf("TS = %d\n",Sim->Timestep());
    
    for(int d = 0; d < m_depth; d++){
        ave = hcons[d]/ div;
        printf("[H] = %.2e\n",ave);
        for (int r = 0; r < m_rows; r++) {
            for (int c = 0; c < m_cols; c++){
                m_voxels[d][r][c].value = ave;
                m_min = AMD::Min(m_voxels[d][r][c].value, m_min);
                m_max = AMD::Max(m_voxels[d][r][c].value, m_max);
            }
        }
    }
    //printf("min = %.2f max = %.2f Ave = %.2f\n", m_min, m_max, ave/m_num_vox);
    
    //Set_MinMax();
}






AMD::Voxel ***TOPCon::Get_Counts() {
    return m_voxels;
}


unsigned int TOPCon::Get_Rows() const { 
    return m_rows;
}

unsigned int TOPCon::Get_Cols() const { 
    return m_cols;
}

unsigned int TOPCon::Get_Depth() const { 
    return m_depth;
}


AMD::Ivec3 TOPCon::Get_Dims() const{
    return AMD::Ivec3(m_depth, m_rows, m_cols);
}

AMD::Vec3 TOPCon::Get_Bin_Widths() const { 
    return m_bin_width;
}




float TOPCon::Get_Bin_Vol() const { 
    return m_bin_width.x*m_bin_width.y*m_bin_width.z;
}

float TOPCon::Get_Min(){
    return m_min;
}

float TOPCon::Get_Max(){
    return m_max;
}

void TOPCon::Print_Slice(int d){
    for (int i = 0; i < m_rows; i++){
        printf("\n");
        for (int j = 0; j<m_cols; j++){
            printf("%.2f ", m_voxels[d][i][j].value);
        }
        printf("\n");
    }
    
    printf("####################################################################\n");
}


//#######################################################################################################
//#######################################################################################################
//#######################################################################################################


RHO_3D::RHO_3D(){}

RHO_3D::~RHO_3D(){
    if(init){
        for (int i = 0; i< m_depth; i++){
            for (int j = 0; j< m_rows; j++){
                free(m_voxels[i][j]);
            }
            free(m_voxels[i]);
        }
        free(m_voxels);
        free(m_at_types);
        free(m_at_coords);
    }
}



void RHO_3D::Init(const char* file){
    
    Rho_Data_3D dat = Read_Charge_Density(file);
    
    m_bin_width = dat.m_bin_widths;
    
    AMD::Voxel::lengths = m_bin_width;
    
    m_rows = dat.m_ny; m_cols = dat.m_nx; m_depth = dat.m_nz;
    m_num_vox = m_depth*m_rows*m_cols;
    
    m_num_ats = dat.num_ats;
    m_at_coords = (AMD::Vec3*)malloc(m_num_ats*sizeof(AMD::Vec3));
    m_at_types = (float*)malloc(m_num_ats*sizeof(float));
    for(int i = 0; i < m_num_ats; i++){
        m_at_types[i] = dat.at_types[i];
        m_at_coords[i] = dat.at_coords[i];
    }
    
    m_voxels = (AMD::Voxel***)malloc(m_depth*sizeof(AMD::Voxel**));
    float min = 1000000000.; float max = -10000000000.;
    float val = 0.0;
    for (int i = 0; i< m_depth; i++){
        m_voxels[i] = (AMD::Voxel**)malloc(m_rows*sizeof(AMD::Voxel*));
        for (int j = 0; j< m_rows; j++){
            m_voxels[i][j] = (AMD::Voxel*)malloc(m_cols* sizeof(AMD::Voxel));
            for (int k = 0; k < m_cols; k++) {
                m_voxels[i][j][k] = AMD::Voxel(i, j, k);
                m_voxels[i][j][k].Set_Center();
                val = dat.m_dat[i][j][k];
                m_voxels[i][j][k].value = dat.m_dat[k][j][i];
                max = AMD::Max(val, max);
                min = AMD::Min(val, min);
            }
        }
    }
    m_max = max; m_min = min;
    Normalize();
    init = true;
}


AMD::Voxel ***RHO_3D::Get_Counts() {
    return m_voxels;
}


unsigned int RHO_3D::Get_Rows() const {
    return m_rows;
}

unsigned int RHO_3D::Get_Cols() const {
    return m_cols;
}

unsigned int RHO_3D::Get_Depth() const {
    return m_depth;
}


AMD::Ivec3 RHO_3D::Get_Dims() const{
    return AMD::Ivec3(m_depth, m_rows, m_cols);
}

AMD::Vec3 RHO_3D::Get_Bin_Widths() const {
    return m_bin_width;
}


float RHO_3D::Get_Bin_Vol() const {
    return m_bin_width.x*m_bin_width.y*m_bin_width.z;
}

float RHO_3D::Get_Min(){
    return m_min;
}

float RHO_3D::Get_Max(){
    return m_max;
}

float* RHO_3D::Get_At_Types(){
    return m_at_types;
}

AMD::Vec3* RHO_3D::Get_At_Coords(){
    return m_at_coords;
}

int RHO_3D::Get_Num_Ats(){
    return m_num_ats;
}
void RHO_3D::Normalize(){
    for(int d = 0; d < m_depth; d++){
        for(int r = 0; r<m_rows; r++){
            for(int c = 0; c<m_cols; c++){
                float val = (m_voxels[d][r][c].value - m_min) / (m_max - m_min);
                m_voxels[d][r][c].value = val;
            }
        }
    }
}


AMD::Vec3 RHO_3D::Get_Center(){
    float x = m_cols*m_bin_width.x;
    float y = m_rows*m_bin_width.y;
    float z = m_depth*m_bin_width.z;
    return AMD::Vec3(x,y,z);
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################

PinHole::PinHole()
{
    m_vox = (AMD::Voxel*)malloc(5000*sizeof(AMD::Voxel));
}
PinHole::~PinHole()
{
    free(m_vox);
    if(file_is_open){
        m_file.close();
    }
    
}


bool PinHole::In_Pinhole(AMD::Voxel vox){
    float val = vox.value;
    float del = 0.5*vox_lens.z;
    float z = vox.Get_Center().z ;
    bool in_reg = false;
    float top = z + del;
    float bot = z - del;
    if(Iface.y < Iface.x){
        in_reg = ( top < Iface.y || bot  > Iface.x);
    }
    else{
        in_reg = (top < Iface.y && bot > Iface.x);
    }
    
    return (in_reg && val < cut);
}




void PinHole::Compute_Volume(TOPCon& dat){
    m_num_vox = 0;
    m_num_Si = 0;
    m_num_Ox = 0;
    Iface = Find_Interfaces(dat);
    AMD::Ivec3 dims = dat.Get_Dims();
    AMD::Voxel*** voxs = dat.Get_Counts();
    vox_lens = dat.Get_Bin_Widths();
    float vox_vol = dat.Get_Bin_Vol();
    vox_vol *= 1.0e-24; // convert to cm^3
    mass_tot = vox_vol*den;
    for(int d = 0; d < dims.d; d++){
        for (int r = 0; r < dims.r; r++) {
            for (int c = 0; c < dims.c; c++){
                if(In_Pinhole(voxs[d][r][c])){
                    float val = voxs[d][r][c].value;
                    float mass_part = mass_si + val*mass_ox;
                    float num_si = mass_tot / mass_part;
                    float num_ox = val*num_si;
                    m_vox[m_num_vox] = voxs[d][r][c];
                    m_num_Si+=num_si;
                    m_num_Ox+=num_ox;
                    m_num_vox++;
                    if(m_num_vox >= 5000){
                        ;
                    }
                }
                
            }
        }
    }
    //Build_Set();
    float vol = AMD::Voxel::Get_Vol();
    m_vol = m_num_vox *vol;
    
}


void PinHole::Build_Set(){

    Set* temp_sets = new Set[1000];
    int num_sets = 0;
    bool new_set;
    for(int i = 0; i < m_num_vox; i ++){
        new_set = true;
        for(int j = 0; j < num_sets; j++){
            if(temp_sets[j].Check_Neighbors(m_vox[i])){
                temp_sets[j].Push_El(m_vox[i]);
                new_set = false;
                break;
            }
            
        }
        if(new_set){
            temp_sets[num_sets].Push_El(m_vox[i]);
            num_sets ++;
        }
    }
    
    
    // check first set[i] against all others,
    // if set[i] has nebighors in set[j]
    // merge set[i] and set[j] and store in new sets.
    // else store set[i]
    // prev_sets = new_sets
    // go back to step one;
    


    bool all_sets_merged = false;
    int num_merged = 0;
    while (!all_sets_merged) {
        num_merged = 0;
        for(int i = 0; i < num_sets;i++){
            for(int j = i + 1; j < num_sets; j++){
                
                if(Merge_Check(temp_sets[i], temp_sets[j])){
                    
                    temp_sets[i].Merge(temp_sets[j]);
                    num_merged ++;
                    temp_sets[j].num_el = 0;
                }//if
                 
            }//for 2
        }//for 1
        if(num_merged == 0){
            all_sets_merged = true;
            //break;
        }
    }//while
    
    m_num_Ox = 0.0;
    m_num_Si = 0.0;
    int count = 0;
    
    int largest = 0;
    for(int i = 0; i < num_sets; i++){
        int num = temp_sets[i].Num_El();
        largest = (num > largest) ? num : largest;
    }
    
    
    for(int i = 0; i < num_sets; i++){
        float del = temp_sets[i].Delta_Z();
        int num = temp_sets[i].Num_El();
        //printf("Num el = %d delta z = %.2f\n", num, del);
        //temp_sets[i].Print();
        if(num == largest){
            for(int j = 0; j < num; j++){
                float val = temp_sets[i].m_vox[j].value;
                float mass_part = mass_si + val*mass_ox;
                float num_si = mass_tot / mass_part;
                float num_ox = val*num_si;
                m_num_Si+=num_si;
                m_num_Ox+=num_ox;
                m_vox[count] = temp_sets[i].m_vox[j];
                count ++;
            }
        }
    }
    
    
    //printf("old = %d new = %d\n", m_num_vox, count);
    m_num_vox = count;
    delete[] temp_sets;
}



void PinHole::Compute_Num_Atoms(){
    return;
}

AMD::Voxel* PinHole::Get_Voxels(){
    return m_vox;
}


int PinHole::Get_Num_Vox(){
    return m_num_vox;
}

float PinHole::Get_Num_Si(){
    return m_num_Si;
}

float PinHole::Get_Num_Ox(){
    return m_num_Ox;
}

float PinHole::Get_Volume(){
    return m_vol;
}

AMD::Vec2 PinHole::Get_Interface(){
    return Iface;
}


void PinHole::Write_Data(){
    
    if(!file_is_open){
        m_file.open(m_file_name, std::ios::out);
        m_file << "#Timestep Volume Num_Si Num_Ox\n";
        file_is_open = true;
    }
    if(write){
        m_file << Sim->Timestep() << " " << m_vol << " " << m_num_Si << " " << m_num_Ox << std::endl;
    }
}

void PinHole::Write_Atoms(){
    
    std::ofstream at_file;
    const char* at_file_name = "/Users/diggs/Desktop/Pinhole-Atoms.txt";
    
    at_file.open(at_file_name, std::ios::out);
    at_file << "#ID Type  x y z\n";
    
    AMD::Vec3 c;
    AMD::Vec3 at_coords;
    Atom* tmp_ats = Sim->Atoms();
    float dist;
    for(int i = 0; i< m_num_vox; i++){
        c = m_vox[i].Get_Center();
        
        for(int j = 0; j < Sim->Num_Atoms(); j++){
            at_coords = tmp_ats[j].Get_Coords();
            dist = Boundary_Wrapped_Dist(c, at_coords);
            if(dist < 2.715){
                
                at_file << tmp_ats[j].Get_ID() << " " << tmp_ats[j].Get_Type() << " " << at_coords.x << " " <<
                at_coords.y << " " << at_coords.z << "\n";
            }
            
        }
    }
    
    
    
    at_file.close();
    
}






Voxelize::Voxelize()
{
    int num_ats = 1; //Sim->Num_Atoms();
    int num_sp = sp.m_num_el;
    m_num_vox = num_ats*num_sp;
    m_vox = (AMD::Voxel*)malloc(m_num_vox*sizeof(AMD::Voxel));
    AMD::Voxel::lengths = AMD::Vec3(0.25,0.25,0.25);
    int count = 0;
    for ( int i = 0; i< num_ats; i++){
        AMD::Vec3 pos; // = Sim->Atoms()[i].Get_Coords();
        int typ = 1; //Sim->Atoms()[i].Get_Type();
        for(int j = 0; j<num_sp; j++){
            AMD::Vec3 sp_el = sp.m_el[j];
            AMD::Vec3 c = pos + sp_el;
            m_vox[count] = AMD::Voxel(c);
            m_vox[count].value = (float)(typ) / 2.0;
            count ++;
        }
        
    }
}


Voxelize::~Voxelize(){free(m_vox);}

AMD::Voxel* Voxelize::Get_Vox(){return m_vox;}
int Voxelize::Get_Num_Vox(){return m_num_vox;}


bool Is_DB(AMD::Vec3 TC, AMD::Vec3 TPN, Atom& at){
    AMD::Vec3 offset = 0.5*Sim->Sim_Box();
    TC += offset;
    TPN += offset;
    AMD::Vec3 coords = at.Get_Coords();
    float dist = AMD::Distance(TC, coords);
    AMD::Vec3 n = TPN - TC;
    n.Normalize();
    AMD::Vec3 PQ = coords - TC;
    bool inside = (PQ.dot(n) > -0.001);
    if((dist < 10.0) && inside){
        at.Set_Type(4);
        return true;
        }
    return false;
}

int Compute_Defects(Iso_Mesh& iso)
{
    int Num_Defects = 0;
    
    // I need to define the interface: that will be done with the Iso-Surf.
    // I want to loop through the elements of the mesh and all the atoms and find atoms near the interface.
    Sim->Compute_Neighbors();
    int num_atoms = Sim->Num_Atoms();
    Atom* ats = Sim->Atoms();
    int num_norms = iso.num_normals;
    AMD::Vec3* norms = iso.Get_Normals();
    AMD::Vec3 offset = 0.5*Sim->Sim_Box();
    
    AMD::Vec3 TC;
    AMD::Vec3 TPN;
    for(int i = 0; i < num_atoms; i++){
        float closest = 100000.0;
        int typ = ats[i].Get_Type();
        if(typ != 1){continue;}
        int NN = ats[i].Get_Num_Neighbors();
        if(NN != 3){continue;}
        bool checked = false;
        for(int j = 0; j < num_norms; j+=2){
            AMD::Vec3 P = norms[j];
            AMD::Vec3 PN = norms[j + 1];
            AMD::Vec3 coords = ats[i].Get_Coords() - offset;
            float dist = AMD::Distance(P, coords);
            if(dist < closest){
                TC = P;
                TPN = PN;
                closest = dist;
                checked = true;
                
            }
           
        }
        
        if(!checked){
            ;
        }
        
        if(Is_DB(TC, TPN, ats[i])){
            Num_Defects ++;
        }
        
    }// outer for loop
        
        
    return Num_Defects;
    
    
}


void Compute_MSD(int T){
    
    int n = Sim->Num_Atoms();
    int num_steps = 0;
    AMD::Vec3 box = Sim->Sim_Box();
    AMD::Vec3* initial = (AMD::Vec3*)malloc(n*sizeof(AMD::Vec3));
    float* at_msd = (float*)malloc(n*sizeof(float));
    Atom* ats = Sim->Atoms();
    int num_part = 4;
    float msd[num_part];
    int num_ox[num_part];
    int num_si[num_part];
    float z_part = box.z / (float)num_part;
    int tot_ox = 0;
    for(int i = 0; i < num_part; i++){
        msd[i] = 0.0;
        num_ox[i] = 0;
        num_si[i] = 0;
    }
    int off_set = 0;
    for(int i = 0; i < off_set; i++){
        Sim->Step_Forward();
    }
    
    for(int i = 0; i < n; i++){
        int idx = ats[i].Get_ID() - 1;
        int typ = ats[i].Get_Type();
        AMD::Vec3 coords = ats[i].Get_Coords();
        initial[idx] = coords;
        at_msd[i] = 0.0;
        int z_idx = floor(coords.z / (z_part + 0.1));
        if(z_idx > (num_part - 1)){
            z_idx = 0;
            
        }
        if(typ == 2){
            num_ox[z_idx] ++;
            tot_ox ++;
          
        }
        
        if(typ == 1){
            num_si[z_idx] ++;
        }
        
    }
    
    for(int i = 0; i < num_part; i++){
        float lo = i*z_part;
        float hi = (i + 1)*z_part;
        float x = (float)num_ox[i] / (float)num_si[i];
        printf(" Z = [%.2f, %.2f] x = %.2f Nox = %d\n",lo,hi, x,num_ox[i]);
    }
    
    std::ofstream p_out;
    char file_name[256];
    snprintf(file_name,256,"/Users/diggs/Desktop/noH-%d-msd.txt",T);
    
    p_out.open(file_name, std::ios::out);
    p_out << "#MSD \n";
    
    int num_blks = Sim->Num_Blocks();
    float val;
    float z = 0.0;
    float ave = 0.0;
    float ave_msd;
    float cut = 0.0;
    for(int i = 0; i < num_blks - off_set; i++){
        int ts = Sim->Timestep();
        int tmp_count = 0;
        ave = 0.0;
        ave_msd = 0.0;
        for(int j = 0; j < n; j++){
            int at_idx = ats[j].Get_ID() - 1;
            AMD::Vec3 init = initial[at_idx];
            int z_idx = floor(init.z / (z_part));
            
            if(z_idx > (num_part - 1)){
                z_idx = 0;
                }
            
            AMD::Vec3 curr = ats[j].Get_Coords();
            int typ = ats[j].Get_Type();
            if(typ == 2){
                if(z_idx == 0){
                    //z_idx = num_part - 1;
                }
                val = Boundary_Wrapped_Dist(init, curr);
                ave += val;
            }
            else{val = 0.0;}
            if(val > 2.0*cut || val < -0.001){
                //val = cut;
                tmp_count ++;
            }
            msd[z_idx] += val * val;
            at_msd[at_idx] += val;
            ave_msd += val * val;
            
        }
        ave /= (float)(tot_ox);
        cut = ave;
        if(cut < 1.0){
            cut = 2.0;
        }
        ave *= ave;
        ave_msd /= (float)(tot_ox);
        printf("Num removed = %d Ave = %.3f\n",tmp_count, ave);
        printf("#########ts = %d############\n",ts);
        printf("Z = ");
        for(int k = 0; k < num_part; k++){
            z = (k+1)*z_part;
            printf(" %.2f",z);
            
        }
        printf("\n");
        printf("MSD = ");
        for(int k = 0; k < num_part; k++){
            if(num_ox[k] > 0){
                msd[k] /= (float)num_ox[k];
            }
            printf(" %.2f", msd[k]);
            p_out << msd[k] << " ";
            msd[k] = 0.0;
        }
        printf("\n");
        p_out << ave_msd << " ";
        p_out << ave << " ";
        p_out << "\n";
        num_steps ++;
        Sim->Step_Forward();
    }
    
    int ts = Sim->Timestep();
    printf("#########ts = %d############\n",ts);
    Sim->Compute_Neighbors();
    for(int i = 0; i < n; i++){
        int at_idx = ats[i].Get_ID() - 1;
        float val = at_msd[at_idx] / num_steps;
        if(val > 5.0){
            //printf("#####################\n");
            printf("ID = %d dist = %.3f\n", at_idx + 1 ,val);
            printf(" (%.2f %.2f %.2f)\n", initial[at_idx].x, initial[at_idx].y, initial[at_idx].z);
            ats[i].Print_Neighbors();
        }
    }
    
    p_out.close();
    free(initial);
    free(at_msd);
    
}



/*
bool PinHole::In_Pinhole(AMD::Voxel vox){
    float val = vox.value;
    float del = 0.5*vox_lens.z;
    float z = vox.Get_Center().z ;
    bool in_reg = z > 15.0;
    if(in_reg){
        if((z - Iface.x) < del){
            return (val > cut);
        }
        else{
            return (val > cut);
        }
    }
    return (in_reg && val > cut);
}

*/


//########JUNK FUNCS################################################################
/*
 
 void Data_3D::Compute_Fraction_X(AMD::Vec3& coords, int type){
     
     // 1) computed distances to bin_edges
     float radius = Sim->radii[type];
     float mass = Sim->masses[type];
     
     float r_scale = radius;
     float mass_frac = mass/m_num_sp_el;
     int d, r, c;
     AMD::Vec3 delta;
     AMD::Vec3 shifted;
     for (int i = 0; i < m_num_sp_el; i++){
         delta = m_sphere[i]*r_scale;
         shifted = coords + delta;
         c = round(shifted.x/m_bin_width.x);
         r = round(shifted.y/m_bin_width.y);
         d = round(shifted.z/m_bin_width.z);
         Boundary_Wrapped_Index(&d, &r, &c);
         m_voxels[d][r][c].value += mass_frac;
       
     }
     
 }

 
 
 bool Data_3D::Meets_Conditions(){
     return true;
 }


 void Data_3D::Add_Voxel_to_Set(AMD::Voxel& vox){
     for (int i =0; i<m_num_sets; i++){
         if (m_sets[i].Check_Neighbors(vox)){
             m_sets[i].Push_El(&vox);
             float num = (float)m_sets[i].s_num_vox;
             set_max = AMD::Max(num, set_max);
             return;
         }
         
     }
     
     m_sets[m_num_sets].s_num = m_num_sets;
     m_sets[m_num_sets].Push_El(&vox);
     m_num_sets ++;
 }


 
 void Data_3D::Sort_Data(){
     for(int i = 0; i < m_depth; i++){
         for (int j = 0; j < m_rows; j++) {
             for (int k = 0; k < m_cols; k++){
                 if(m_voxels[i][j][k].value < m_density_cutoff){
                     Add_Voxel_to_Set(m_voxels[i][j][k]);
                     
                 }
             }
         }
     }
 }


 
 void Data_3D::Set_MinMax() {
     float Density_Mod = 1.66 / (m_bin_vol);
     for(int d = 0; d < m_depth; d++){
         for (int r = 0; r < m_rows; r++) {
             for (int c = 0; c < m_cols; c++){
                 m_voxels[d][r][c].value*=Density_Mod;
                 if(m_voxels[d][r][c].value < m_density_cutoff){
                     Add_Voxel_to_Set(m_voxels[d][r][c]);
                     
                 }
                 m_min = AMD::Min(m_voxels[d][r][c].value, m_min);
                 m_max = AMD::Max(m_voxels[d][r][c].value, m_max);
                 
             }
         }
     }
 }


 
 
 
void Hist_3D::Get_Vox_Surface(AMD::Voxel* vox){
    int _d = vox->d;
    int _r = vox->r;
    int _c = vox->c;
    float val;
    float x = ((float)_c - 0.5) * m_bin_width.x;
    float y = ((float)_r - 0.5) * m_bin_width.y;
    float z = ((float)_d - 0.5) * m_bin_width.z;
    AMD::Vec3 Center(x,y,z);
    AMD::Vec3 norm;
    for (int i = 0; i < 3; i++){
        for (int j = 0; j< 3; j++){
            for (int k = 0; k< 3; k++){
                int a = _d + i - 1; int b = _r + j - 1; int c = _c + k - 1;
                norm.x = (float)k; norm.y = (float)j; norm.z = (float)i;
                norm.Normalize();
                Boundary_Wrapped_Index(&a, &b, &c);
                val = m_voxels[a][b][c].value;
                if (val > m_density_cutoff){
                    Add_Triangle(Center, norm, val);
                    
                }
                
            }
            
        }
        
    }
}
*/

/*
void Hist_3D::Compute_Ave_Density_3D(float bin_width) {
    AMD::Vec3 Box = Sim->Sim_Box();
    if(!(this -> init)){
        int seg_x = (ceil(Box.x)/bin_width);
        int seg_y = (ceil(Box.y)/bin_width);
        int seg_z = (ceil(Box.z)/bin_width);
        
        m_bin_width.x = Box.x/(seg_x + 0.001);
        m_bin_width.y = Box.y/(seg_y + 0.001);
        m_bin_width.z = Box.z/(seg_z + 0.001);
        
        
        Init(seg_x, seg_y, seg_z);
    
    }
   
    else{Clear();}
    
    // make an array of ints that can contain the possible
    AMD::Vec3 tmp;
    int type_indx;
    for(int i = 0; i < Sim->Num_Atoms(); i++){
        tmp = Sim->Atoms()[i].Get_Coords();
        type_indx = Sim->Atoms()[i].Get_Type() - 1;
        if (tmp.x > 1000.0){continue;}
        int tmp_col = (int)(tmp.x/m_bin_width.x);
        int tmp_row = (int)(tmp.y/m_bin_width.y);
        int tmp_depth = (int)(tmp.z/m_bin_width.z);
        if(tmp_depth > m_depth - 1){tmp_depth -= m_depth -1;}
        if(tmp_row > m_rows - 1){tmp_row -= m_rows -1;}
        if(tmp_col > m_cols - 1){tmp_col -= m_cols -1;}
        m_voxels[tmp_depth][tmp_row][tmp_col].value += Sim->masses[type_indx];
    }
    Set_MinMax_W_Ave();
}





float Hist_3D::Average(int depth, int row, int col){
    // 1) computed distances
    float test = 0.0;
    for (int i = -1; i < 2; i++){
        for (int j = -1; j< 2; j++){
            for (int k = -1; k< 2; k++){
                int a = depth + i; int b = row + j; int c = col + k;
                Boundary_Wrapped_Index(&a, &b, &c);
                test += m_values[a][b][c];
        
            }

        }
    }
    return test / 27.0;
}





void Hist_3D::Set_MinMax_W_Ave() {
    float mod = 1.66 / (m_bin_vol);
    float temp[m_depth][m_rows][m_cols];
    float ave;
    for(int i = 0; i < m_depth; i++){
        for (int j = 0; j < m_rows; j++) {
            for (int k = 0; k < m_cols; k++){
                ave = mod*Average(i, j, k);
                if (ave > 6.0){
                    ;
                }
                temp[i][j][k] = ave;
            }
        }
    }
    for(int i = 0; i < m_depth; i++){
        for (int j = 0; j < m_rows; j++) {
            for (int k = 0; k < m_cols; k++){
                m_values[i][j][k] = temp[i][j][k];
                m_min = AMD::Min(m_values[i][j][k], m_min);
                m_max = AMD::Max(m_values[i][j][k], m_max);
            }
        }
    }
    ;
    
}


void Hist_3D::Compute_Fraction(AMD::Vec3& coords, int type,int depth, int row, int col){
    // 1) computed distances to bin_edges
    float radius = Sim->radii[type];
    float mass = Sim->masses[type];
    
    float dist_left = abs(coords.x - col*m_bin_width.x); float dist_right = abs(coords.x - (col + 1)*m_bin_width.x);
    float dist_bottom = abs(coords.y - row*m_bin_width.y); float dist_top = abs(coords.y - (row + 1)*m_bin_width.y);
    float dist_out = abs(coords.z - depth*m_bin_width.z); float dist_in = abs(coords.z - (depth + 1)*m_bin_width.z);
    
    AMD::Vec3 col_weights = Compute_Weights(dist_left, dist_right, radius);
    AMD::Vec3 row_weights = Compute_Weights(dist_bottom, dist_top, radius);
    AMD::Vec3 depth_weights = Compute_Weights(dist_out, dist_in, radius);
     
    
    float test = 0.0;
    float test_mat[3][3][3];
    for (int i = -1; i < 2; i++){
        for (int j = -1; j< 2; j++){
            for (int k = -1; k< 2; k++){
                int a = depth + i; int b = row + j; int c = col + k;
                Boundary_Wrapped_Index(&a, &b, &c);
                m_values[a][b][c] += (mass*depth_weights[i + 1]*row_weights[j + 1]*col_weights[k + 1]);
                test += (mass*depth_weights[i + 1]*row_weights[j + 1]*col_weights[k + 1]);
                test_mat[i+1][j+1][k+1] = (mass*depth_weights[i + 1]*row_weights[j + 1]*col_weights[k + 1]);
        
            }

        }
    }
    if((test/mass) > 1.25 || (test/mass) < 0.75){
        ;
    }
   
}



 int tmp_col = (int)(tmp.x/m_bin_width.x);
 int tmp_row = (int)(tmp.y/m_bin_width.y);
 int tmp_depth = (int)(tmp.z/m_bin_width.z);
 
 if(tmp_depth > m_depth - 1){tmp_depth = m_depth -1;}
 if(tmp_row > m_rows - 1){tmp_row = m_rows -1;}
 if(tmp_col > m_cols - 1){tmp_col = m_cols -1;}
 Compute_Fraction(tmp, type_indx, tmp_depth, tmp_row, tmp_col);
 
 
 */






/*
 
 void Hist_2D::Get_Portions(AMD::Vec3& coords, float radius, int row, int col, float bin_width, int indx[][2], float vals[]){
     // 1) computed distances to bin_edges
     float left = col*bin_width; float right = (col + 1)*bin_width;
     float bottom = row*bin_width; float top = (row + 1)*bin_width;
     float delta_left = abs(coords.x - left); float delta_right = abs(coords.x - right);
     float delta_bottom = abs(coords.y - bottom); float delta_top = abs(coords.y - top);
     
     float weight_left = AMD::Max(radius - delta_left, 0.0);
     float weight_right = AMD::Max(radius - delta_right, 0.0);
     
     float weight_top = AMD::Max(radius - delta_top, 0.0);
     float weight_bottom = AMD::Max(radius - delta_bottom, 0.0);
     
     float weight_mid_left = AMD::Min(radius + delta_left, 2.0 * radius);
     float weight_mid_right = AMD::Min(radius + delta_right, 2.0 * radius);
     
     float weight_mid_top = AMD::Min(radius + delta_top, 2.0 * radius);
     float weight_mid_bottom = AMD::Min(radius + delta_bottom, 2.0 * radius);
     
     float col_weights[3] = {weight_left, AMD::Min(weight_mid_left, weight_mid_right), weight_right};
     float row_weights[3] = {weight_bottom, AMD::Min(weight_mid_top, weight_mid_bottom), weight_top};
     int count = 0;
     for (int i = -1; i < 2; i++){
         for (int j = -1; j< 2; j++){
             int a = row + i; int b = col + j;
             Boundary_Wrapped_Index(&a, &b);
             indx[count][0] = a; indx[count][1] = b;
             vals[count] = (row_weights[i + 1]*col_weights[j + 1])/(4*radius*radius);
             
         }
     }
     
     for (int i = 6; i >= 0; i-=3){
         std::cout << vals[i] << " " << vals[i + 1] << " " << vals[i + 2] << std::endl;
     }
     
 }
 
 void Hist_3D::Evaluate_Neighbors(AMD::Voxel* vox_ptr, float mat[3][3][3], int* num_nebs){
     int _d = vox_ptr->d;
     int _r = vox_ptr->r;
     int _c = vox_ptr->c;
     int count =0;
     float val;
     float x = ((float)_c - 0.5) * m_bin_width.x;
     float y = ((float)_r - 0.5) * m_bin_width.y;
     float z = ((float)_d - 0.5) * m_bin_width.z;
     AMD::Vec3 Center(x,y,z);
     for (int i = 0; i < 3; i++){
         for (int j = 0; j< 3; j++){
             for (int k = 0; k< 3; k++){
                 int a = _d + i - 1; int b = _r + j - 1; int c = _c + k - 1;
                 Boundary_Wrapped_Index(&a, &b, &c);
                 val = m_voxels[a][b][c].value;
                 mat[i][j][k] = val;
                 if (val > m_density_cutoff){count++;}
                 
             }
             
         }
         
     }
     *num_nebs = count;
 }

 
 float Data_3D::No_Ave_X(int d, int r, int c){
     float num_Si = 0;
     float num_Ox = 0;
     num_Si = counts_Si[d][r][c];
     num_Ox = counts_Ox[d][r][c];
     m_voxels[d][r][c].num_Si = num_Si;
     m_voxels[d][r][c].num_Ox = num_Ox;
     if(num_Si < 0.01){
         if(num_Ox > 1.){
             return 2.0;
         }
         else{
             return 0.0;
         }
     }
     else{
         float ret = (float)num_Ox / (float)num_Si;
         if(ret > 2.0){
             //printf(" x = %.2f\n",ret);
         }
         return ret;
     }
 }


 
 
 */






