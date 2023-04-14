//
//  Computes.cpp
//  VolumeData
//
//  Created by Andrew Diggs on 3/22/23.
//

#include "Computes.hpp"
#include "Atomic.hpp"
extern Simulation* Sim;



AMD::Vec3 Compute_Weights(float d1, float d2, float rad){
    float dl = rad - d1;
    float dr = rad - d2;
    float weight_left = AMD::Max(dl, 0.0);
    float weight_right = AMD::Max(dr, 0.0);
    float weight_middle = rad + weight_left + weight_right;
    float N = weight_left + weight_middle + weight_right;
    return AMD::Vec3(weight_left/N, weight_middle/N, weight_right/N);
}



Hist_2D::Hist_2D()
: m_cols(0), m_rows(0), m_min(1000.0), m_max(0.0)
{}

Hist_2D::Hist_2D(int num_bins_x, int num_bins_y)
:m_cols(num_bins_x), m_rows(num_bins_y),  m_min(1000.0), m_max(0.0)
{
    Init(num_bins_x, num_bins_y);
}

Hist_2D::~Hist_2D() { 
    Free();
}

void Hist_2D::Init(int seg_x, int seg_y){
    m_rows = seg_y; m_cols = seg_x;
    m_bin_vol = m_bin_width_x*m_bin_width_y*m_bin_width_z;
    
    float dx = m_bin_width_x;
    float dy = m_bin_width_x;
    float x = 0.5*dx;
    float y = 0.5*dy;
    
    int count = 0;
    m_bin_centers = (AMD::Vec3*)malloc(m_cols*m_rows*sizeof(AMD::Vec3));
    m_counts = (float**)malloc(m_rows*sizeof(float*));
    for (int i = 0; i < m_rows; i++){
        m_counts[i] = (float*)malloc(m_cols*sizeof(float));
        x = 0.5*dx;
        for (int j = 0; j< m_cols; j++){
            m_counts[i][j] = 0;
            m_bin_centers[count] = AMD::Vec3(x, y, 0.0);
            count++;
            x+=dx;
        }
        y+=dy;
    }
    m_bin_centers[8].print();
    init = true;
}

void Hist_2D::Free(){
    for (int i = 0; i < m_rows; i++){
        free(m_counts[i]);
    
    }
    free(m_counts);
    free(m_bin_centers);
}


void Hist_2D::Clear(){
    for(int i = 0; i < m_rows; i++){
        for (int j = 0; j < m_cols; j++) {
            m_counts[i][j] = 0;
        }
    }
}

void Hist_2D::Compute(float **data) { 
    for(int i = 0; i < m_rows; i++){
        for (int j = 0; j < m_cols; j++) {
            m_counts[i][j] = 0;
        }
    }
}

void Hist_2D::Compute(float data[][1000]) { 
    for(int i = 0; i < m_rows; i++){
        for (int j = 0; j < m_cols; j++) {
            m_counts[i][j] = 0;
        }
    }
}

void Hist_2D::Compute_XY_Number_Density(float bin_size) {
    AMD::Vec3 Box = Sim->Sim_Box();
    if(!(this -> init)){
        int seg_x = (ceil(Box.x)/bin_size);
        int seg_y = (ceil(Box.y)/bin_size);
        Init(seg_x, seg_y);
        m_bin_width_x = bin_size;
    }
   
    else{Clear();}
    AMD::Vec3 tmp;
    for(int i = 0; i < Sim->Num_Atoms(); i++){
        tmp = Sim->Atoms()[i].Get_Coords();
        if (tmp.x > 1000.0){continue;}
        int idx = ((int)tmp.x)/bin_size;
        int idy = ((int)tmp.y)/bin_size;
        m_counts[idy][idx]++;
    }
    Set_MinMax();
}

void Hist_2D::Compute_XY_Number_Density(float bin_size, int type) {
    AMD::Vec3 Box = Sim->Sim_Box();
    if(!(this -> init)){
        int seg_x = (ceil(Box.x)/bin_size);
        int seg_y = (ceil(Box.y)/bin_size);
        Init(seg_x, seg_y);
        m_bin_width_x = bin_size;
    }
   
    else{Clear();}
    AMD::Vec3 tmp;
    for(int i = 0; i < Sim->Num_Atoms(); i++){
        tmp = Sim->Atoms()[i].Get_Coords();
        if (tmp.x > 1000.0){continue;}
        int idx = ((int)tmp.x)/bin_size;
        int idy = ((int)tmp.y)/bin_size;
        m_counts[idy][idx]++;
    }
    Set_MinMax();
}

void Hist_2D::Compute_XY_Mass_Density(float bin_size) {
    AMD::Vec3 Box = Sim->Sim_Box();
    if(!(this -> init)){
        int seg_x = (ceil(Box.x)/bin_size);
        int seg_y = (ceil(Box.y)/bin_size);
        
        m_bin_width_x = Box.x/(seg_x + 0.001);
        m_bin_width_y = Box.y/(seg_y + 0.001);
        m_bin_width_z = Box.z;
        
        
        Init(seg_x, seg_y);
    
    }
   
    else{Clear();}
    
    // make an array of ints that can contain the possible
    AMD::Vec3 tmp;
    int type_indx;
    for(int i = 0; i < Sim->Num_Atoms(); i++){
        tmp = Sim->Atoms()[i].Get_Coords();
        type_indx = Sim->Atoms()[i].Get_Type() - 1;
        if (tmp.x > 1000.0){continue;}
        int tmp_col = (int)(tmp.x/m_bin_width_x);
        int tmp_row = (int)(tmp.y/m_bin_width_y);
        if(tmp_col == m_cols || tmp_row == m_rows){
            ;
        }
        m_counts[tmp_row][tmp_col] += Sim->masses[type_indx];
    }
    Set_MinMax();
}

void Hist_2D::Compute_XY_Mass_Density(float bin_size, int type) {
    AMD::Vec3 Box = Sim->Sim_Box();
    if(!(this -> init)){
        int seg_x = (ceil(Box.x)/bin_size);
        int seg_y = (ceil(Box.y)/bin_size);
        
        m_bin_width_x = Box.x/(seg_x + 0.001);
        m_bin_width_y = Box.y/(seg_y + 0.001);
        m_bin_width_z = Box.z;
        
        
        Init(seg_x, seg_y);
    
    }
   
    else{Clear();}
    
    // make an array of ints that can contain the possible
    AMD::Vec3 tmp;
    int type_indx;
    for(int i = 0; i < Sim->Num_Atoms(); i++){
        tmp = Sim->Atoms()[i].Get_Coords();
        type_indx = Sim->Atoms()[i].Get_Type() - 1;
        if (tmp.x > 1000.0){continue;}
        int tmp_col = (int)(tmp.x/m_bin_width_x);
        int tmp_row = (int)(tmp.y/m_bin_width_y);
        if(tmp_col == m_cols || tmp_row == m_rows){
            ;
        }
        m_counts[tmp_row][tmp_col] += Sim->masses[type_indx];
    }
    Set_MinMax();
}





void Hist_2D::Compute_XZ_Mass_Density( float bin_size) {
    AMD::Vec3 Box = Sim->Sim_Box();
    if(!(this -> init)){
        int seg_x = (ceil(Box.x)/bin_size);
        int seg_z = (ceil(Box.z)/bin_size);
        
        m_bin_width_x = Box.x/(seg_x + 0.001);
        m_bin_width_y = Box.y;
        m_bin_width_z = Box.z/(seg_z + 0.001);
        
        
        Init(seg_x, seg_z);
    
    }
   
    else{Clear();}
    
    // make an array of ints that can contain the possible
    AMD::Vec3 tmp;
    int type_indx;
    for(int i = 0; i < Sim->Num_Atoms(); i++){
        tmp = Sim->Atoms()[i].Get_Coords();
        type_indx = Sim->Atoms()[i].Get_Type() - 1;
        if (tmp.x > 1000.0){continue;}
        int tmp_col = (int)(tmp.x/m_bin_width_x);
        int tmp_row = (int)(tmp.z/m_bin_width_z);
        if(tmp_col == m_cols || tmp_row == m_rows){
            ;
        }
        Get_Portions(tmp, type_indx , tmp_row, m_bin_width_z, tmp_col, m_bin_width_x);
    }
    Set_MinMax();
}

void Hist_2D::Compute_XZ_Mass_Density(float bin_size, int type) {
    AMD::Vec3 Box = Sim->Sim_Box();
    if(!(this -> init)){
        int seg_x = (ceil(Box.x)/bin_size);
        int seg_z = (ceil(Box.z)/bin_size);
        
        m_bin_width_x = Box.x/(seg_x + 0.001);
        m_bin_width_y = Box.y;
        m_bin_width_z = Box.z/(seg_z + 0.001);
        
        
        Init(seg_x, seg_z);
    
    }
   
    else{Clear();}
    
    // make an array of ints that can contain the possible
    AMD::Vec3 tmp;
    int type_indx;
    for(int i = 0; i < Sim->Num_Atoms(); i++){
        tmp = Sim->Atoms()[i].Get_Coords();
        type_indx = Sim->Atoms()[i].Get_Type() - 1;
        if (tmp.x > 1000.0){continue;}
        int tmp_col = (int)(tmp.x/m_bin_width_x);
        int tmp_row = (int)(tmp.z/m_bin_width_z);
        if(tmp_col == m_cols || tmp_row == m_rows){
            ;
        }
        Get_Portions(tmp, type_indx , tmp_row, m_bin_width_z, tmp_col, m_bin_width_x);
    }
    Set_MinMax();
}








void Hist_2D::Get_Portions(AMD::Vec3& coords, int type, int row, float row_mod, int col, float col_mod){
    // 1) computed distances to bin_edges
    float radius = Sim->radii[type];
    float mass = Sim->masses[type];
    
    float left = col*col_mod; float right = (col + 1)*col_mod;
    float bottom = row*row_mod; float top = (row + 1)*row_mod;
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
    
    float test = 0.0;
    for (int i = -1; i < 2; i++){
        for (int j = -1; j< 2; j++){
            int a = row + i; int b = col + j;
            Boundary_Wrapped_Index(&a, &b);
            m_counts[a][b] += (mass*row_weights[i + 1]*col_weights[j + 1])/(4*radius*radius);
            test += (mass*row_weights[i + 1]*col_weights[j + 1])/(4*radius*radius);
            if((test/mass) > 1.01){
                ;
            }

        }
    }
   
}



void Hist_2D::Boundary_Wrapped_Index(int* r, int* c){
    if(*c > m_cols || *r > m_rows){
        ;
    }
    int r_bw = (*r > m_rows - 1) ? *r - m_rows : (*r >= 0) ? *r : m_rows + *r;
    int c_bw = (*c > m_cols - 1) ? *c - m_cols : (*c >= 0) ? *c : m_cols + *c;
    *r = r_bw;
    *c = c_bw;
    
}




void Hist_2D::Set_MinMax() {
    float mod = 1.66 / (m_bin_vol);
    for(int i = 0; i < m_rows; i++){
        for (int j = 0; j < m_cols; j++) {
            m_counts[i][j]*=mod;
            m_min = AMD::Min(m_counts[i][j], m_min);
            m_max = AMD::Max(m_counts[i][j], m_max);
        }
    }
}



int Hist_2D::Get_Rows(){
    return m_rows;
}


int Hist_2D::Get_Cols(){
    return m_cols;
}


float** Hist_2D::Get_Counts(){
    return m_counts;
}

AMD::Vec3* Hist_2D::Get_Centers(){
    return m_bin_centers;
}

float Hist_2D::Get_Delta(){
    return m_bin_width_x;
}


float Hist_2D::Get_Bin_Vol(){
    return m_bin_width_x * m_bin_width_y * m_bin_width_z;
}


float Hist_2D::Compute_Total_Density(){
    float tot = 0.0;
    int count = 0;
    for(int i = 0; i < m_rows; i++){
        for (int j = 0; j < m_cols; j++) {
            tot += m_counts[i][j];
            count++;
        }
    }
    return tot/count;
}

void Hist_2D::Print(){
    
    for (int i = 0; i<m_rows; i++){
        for (int j = 0; j< m_cols; j++){
            std::cout << "---";
        }
        std::cout << "\n|";
        for (int j = 0; j< m_cols; j++){
            int count = m_counts[i][j];
            if (count>9){std::cout << m_counts[i][j] << "|";}
            else{std::cout << m_counts[i][j] << " |";}
        }
        std::cout << "\n";
    }
    for (int j = 0; j< m_cols; j++){
        std::cout << "---";
    }
    std::cout << std::endl;
}








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
 */









Hist_3D::Hist_3D()
:init(false),m_rows(0), m_cols(0), m_depth(0), m_min(1000.0), m_max(-10000.0)
{
    
}

Hist_3D::~Hist_3D() {
    if(init){
        for (int i = 0; i< m_depth; i++){
            for (int j = 0; j< m_rows; j++){
                free(m_values[i][j]);
            }
            free(m_values[i]);
        }
        free(m_values);
    }
}

void Hist_3D::Init(int rows, int cols, int depth){
    m_rows = rows; m_cols = cols; m_depth = depth;
    m_num_bins = m_depth*m_rows*m_cols;
    
    m_bin_vol = m_bin_width.x*m_bin_width.y*m_bin_width.z;
    
    m_values = (float***)malloc(m_depth*sizeof(float**));
    for (int i = 0; i< m_depth; i++){
        m_values[i] = (float**)malloc(m_rows*sizeof(float*));
        for (int j = 0; j< m_rows; j++){
            m_values[i][j] = (float*)malloc(m_cols* sizeof(float));
            for (int k = 0; k < m_cols; k++) {
                m_values[i][j][k] = 0.0;
            }
        }
    }
    init = true;
}

void Hist_3D::Clear(){
    for (int i = 0; i< m_depth; i++){
        for (int j = 0; j< m_rows; j++){
            for (int k = 0; k < m_cols; k++) {
                m_values[i][j][k] = 0.0;
            }
        }
    }
}

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
        m_values[tmp_depth][tmp_row][tmp_col] += Sim->masses[type_indx];
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




void Hist_3D::Boundary_Wrapped_Index(int* d,int* r, int* c){
    int d_bw = (*d > m_depth - 1) ? (*d - m_depth) : (*d >= 0) ? *d : m_depth + *d;
    int r_bw = (*r > m_rows - 1) ? *r - m_rows : (*r >= 0) ? *r : m_rows + *r;
    int c_bw = (*c > m_cols - 1) ? *c - m_cols : (*c >= 0) ? *c : m_cols + *c;
    *d = d_bw;
    *r = r_bw;
    *c = c_bw;
    
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
    
    
}




void Hist_3D::Set_MinMax() {
    float mod = 1.66 / (m_bin_vol);
    for(int i = 0; i < m_depth; i++){
        for (int j = 0; j < m_rows; j++) {
            for (int k = 0; k < m_cols; k++){
                m_values[i][j][k] *= mod;
                m_min = AMD::Min(m_values[i][j][k], m_min);
                m_max = AMD::Max(m_values[i][j][k], m_max);
            }
        }
    }
    
    
}





float ***Hist_3D::Get_Counts() { 
    return m_values;
}

unsigned int Hist_3D::Get_Rows() const { 
    return m_rows;
}

unsigned int Hist_3D::Get_Cols() const { 
    return m_cols;
}

unsigned int Hist_3D::Get_Depth() const { 
    return m_depth;
}

AMD::Vec3 Hist_3D::Get_Bin_Widths() const { 
    return m_bin_width;
}

float Hist_3D::Get_Bin_Vol() const { 
    return m_bin_width.x*m_bin_width.y*m_bin_width.z;
}

float Hist_3D::Get_Min(){
    return m_min;
}

float Hist_3D::Get_Max(){
    return m_max;
}

void Hist_3D::Print_Slice(int d){
    for (int i = 0; i < m_rows; i++){
        printf("\n");
        for (int j = 0; j<m_cols; j++){
            printf("%.2f ", m_values[d][i][j]);
        }
        printf("\n");
    }
}



void Hist_3D::Compute_Bin_Density_3D(float bin_width) {
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
        
        int tmp_col = (int)(tmp.x/m_bin_width.x);
        int tmp_row = (int)(tmp.y/m_bin_width.y);
        int tmp_depth = (int)(tmp.z/m_bin_width.z);
        
        if(tmp_depth > m_depth - 1){tmp_depth = m_depth -1;}
        if(tmp_row > m_rows - 1){tmp_row = m_rows -1;}
        if(tmp_col > m_cols - 1){tmp_col = m_cols -1;}
        Compute_Fraction(tmp, type_indx, tmp_depth, tmp_row, tmp_col);
    }
    Set_MinMax();
}



void Hist_3D::Compute_Dist_Value(AMD::Vec3& coords, int type,int depth, int row, int col){
    
    // 1) computed distances to bin_edges
    float radius = Sim->radii[type];
    float mass = Sim->masses[type];
    float val = 0.;
    for (int i = -1; i < 2; i++){
        for (int j = -1; j< 2; j++){
            for (int k = -1; k< 2; k++){
                int a = depth + i; int b = row + j; int c = col + k;
                AMD::Vec3 bin_pos(c*m_bin_width.x, b*m_bin_width.y, a*m_bin_width.z);
                float dist_coord = AMD::Distance(coords, bin_pos);
                float dist_rad = dist_coord - radius;
                if( dist_rad <= 0.1){val = 10.0;}
                else{val = 1.0/dist_rad;}
                Boundary_Wrapped_Index(&a, &b, &c);
                m_values[a][b][c] += mass*val;
        
            }

        }
    }
    
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


