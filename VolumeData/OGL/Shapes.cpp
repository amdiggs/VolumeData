//
//  Shapes.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 7/14/22.
//

#include "Shapes.hpp"
#include "Texture.hpp"
//#include "Logging.h"
using namespace AMD;


const Mat4 CT = ROTATION_MATRIX(Vec3(0.0, 0.0, halfPI));




void SHAPE::Gen_cyl_index(int off_set) {
    for (int i = 0; i < m_num_theta ; i++) {
        if((i+1) % m_num_theta){
            indices[m_num_idx] = i + off_set;
            indices[m_num_idx + 1] = i + 1 + off_set;
            indices[m_num_idx + 2] = i + 1 + m_num_theta + off_set;
            
            
            indices[m_num_idx + 3] = i + off_set;
            indices[m_num_idx + 4] = i + m_num_theta + 1 + off_set;
            indices[m_num_idx + 5] = i + m_num_theta + off_set;
            m_num_idx +=6;
            
            
            
        }
        
        
        else{
            indices[m_num_idx] = i+ off_set;
            indices[m_num_idx + 1] = i + 1 - m_num_theta + off_set;
            indices[m_num_idx + 2] = i + 1 + off_set;
            
            
            indices[m_num_idx + 3] = i + off_set;
            indices[m_num_idx + 4] = i + 1 + off_set;
            indices[m_num_idx + 5] = i + m_num_theta + off_set;
            m_num_idx +=6;

        }
    }
    
}

void SHAPE::Gen_fan_index(int count, int tip) {
    int start = m_num_verts - m_num_theta - 1;
    int end = m_num_verts - 1;
    for (int i = start; i < end; i++){
        if((i+1) % m_num_theta){
            indices[count] = i;
            indices[count + 1] = i + 1;
            indices[count + 2] = tip;
            count +=3;
            
            }
        
        
        else{
            indices[count] = i;
            indices[count + 1] = start;
            indices[count + 2] = tip;
            count +=3;

        }
   
}
    return;
}


SHAPE::SHAPE():m_num_idx(0), m_num_verts(0){}
SHAPE::~SHAPE(){}

int SHAPE::num_idx() const {
    return m_num_idx;
}

int SHAPE::num_verts() const {
    return m_num_verts;
}


float Triangle::Area(){
    AMD::Vec3 v1 = c - a;
    AMD::Vec3 v2 = c - b;
    
    AMD::Vec3 CR = v1.cross(v2);
    float mag_cr = CR.len();
    
    return mag_cr / 2.0;
    
}

AMD::Vec3& Triangle::operator[](const int idx){
    switch (idx) {
        case 0:
            return a;
            break;
            
        case 1:
            return b;
            break;
            
        case 2:
            return c;
            break;
            
        default:
            return a;
            break;
    }
}


Circle::Circle(float ex_rad)
:rad(ex_rad)
{}

Circle::~Circle(){
}

void Circle::Gen_points(){
    float theta = 0.0;
    float _x, _y;
    float _z = 0.0;
    verts[0].pos = AMD::Vec3(0.0,0.0,0.0);
    verts[0].clr = AMD::Vec4(0.3,0.1,0.7,1.0);
    verts[0].texture = AMD::Vec2(0.5, 0.5);
    verts[0].tex_indx = 2.0;
    m_num_verts++;
    for (int i =1; i< m_num_theta; i++){
        _x = rad*cos(theta);
        _y = rad*sin(theta);
        verts[i].pos = AMD::Vec3(_x, _y, _z);
        verts[i].clr = AMD::Vec4(0.3,0.1,0.7,1.0);
        verts[i].texture = AMD::Vec2(0.5, 0.5);
        verts[i].tex_indx = 2.0;
        theta+=m_dth;
        m_num_verts++;
    }
    return;
}

void Circle::Gen_indices(){
    int count = 0;
    for (int i = 1; i < m_num_verts; i++){
        if((i+1) % m_num_theta){
            indices[count] = 0;
            indices[count + 1] = i;
            indices[count + 2] = i+1;
            count +=3;
            
            }
        
        
        else{
            indices[count] = 0;
            indices[count + 1] = i;
            indices[count + 2] = i - m_num_verts;
            count +=3;

        }
}
    return;
}






Cube::Cube()
:m_lengths(1.0,1.0,1.0)
{
    Gen_Points();
    AMD::Compute_norms(verts, indices, m_num_idx);
    for(int i = 0; i < 8; i++){
        basic_verts[i] = verts[i];
    }
}

Cube::Cube(float len)
:m_lengths(len)
{
    Gen_Points();
    AMD::Compute_norms(verts, indices, m_num_idx);
    for(int i = 0; i < 8; i++){
        basic_verts[i] = verts[i];
    }
}
    

Cube::Cube(AMD::Vec3 BB)
:m_lengths(BB)
{
    Gen_Points();
    AMD::Compute_norms(verts, indices, m_num_idx);
    for(int i = 0; i < 8; i++){
        basic_verts[i] = verts[i];
    }
}

Cube::~Cube(){}


void Cube::Gen_Points(){
    float dx = 0.5*m_lengths.x; float dy = 0.5*m_lengths.y; float dz = 0.5*m_lengths.z;
    
    
    verts[0].pos = Vec3(-1.0 * dx ,-1.0 * dy, 1.0 * dz);
    verts[1].pos = Vec3(1.0 * dx ,-1.0 * dy, 1.0 * dz);
    verts[2].pos = Vec3(1.0 * dx ,1.0 * dy, 1.0 * dz);
    verts[3].pos = Vec3(-1.0 * dx , 1.0 * dy, 1.0 * dz);
    
    verts[0].texture[0] = 0.0; verts[0].texture[1] = 0.0;
    verts[1].texture[0] = 1.0; verts[1].texture[1] = 0.0;
    verts[2].texture[0] = 1.0; verts[2].texture[1] =1.0;
    verts[3].texture[0] = 0.0; verts[3].texture[1] = 1.0;
    //
    verts[0].tex_indx = 0.0;
    verts[1].tex_indx = 0.0;
    verts[2].tex_indx = 0.0;
    verts[3].tex_indx = 0.0;
    //
    verts[4].pos = Vec3(-1.0 * dx ,-1.0 * dy, -1.0 * dz);
    verts[5].pos = Vec3(1.0 * dx ,-1.0 * dy, -1.0 * dz);
    verts[6].pos = Vec3(1.0 * dx ,1.0 * dy, -1.0 * dz);
    verts[7].pos = Vec3(-1.0 * dx , 1.0 * dy, -1.0 * dz);
    
    
    verts[4].texture[0] = 0.0; verts[4].texture[1] = 0.0;
    verts[5].texture[0] = 1.0; verts[5].texture[1] = 0.0;
    verts[6].texture[0] = 1.0; verts[6].texture[1] = 1.0;
    verts[7].texture[0] = 0.0; verts[7].texture[1] = 1.0;
    
    verts[4].tex_indx = 1.0;
    verts[5].tex_indx = 1.0;
    verts[6].tex_indx = 1.0;
    verts[7].tex_indx = 1.0;
    
    for (int i = 0; i<m_num_verts; i++){
        verts[i].clr = Vec4(0.6, 0.2, 0.4, 1.0);
        
    }
    
}




/*
      7-------6
     /|      |/
    / |      /
  3,9-------2|
   |  4------5
   | /       /
  0,8-------1
 
 
    {0,1,2}, {2,3,0}, {1,5,6}, {6,2,1},
    {5,4,7}, {7,6,5}, {4,8,9}, {9,7,4},
    {0,3,4}, {3,4,7}, {4,5,6}, {4,6,7}
 */
int Cube::num_idx(){
    return m_num_idx;
}

int Cube::num_verts(){
    return m_num_verts;
}

//################################################################################################

Environment_Cube::Environment_Cube()
{
    Gen_Points();
    AMD::Compute_norms(verts, indices, m_num_idx);
}

Environment_Cube::~Environment_Cube(){}


void Environment_Cube::Gen_Points(){
    float c = 0.99; float zb = -2.99; float zf = 0.99;
    
    verts[0].pos = Vec3(-c,-c,zb);
    verts[1].pos = Vec3(c, -c, zb);
    verts[2].pos = Vec3(c, c, zb);
    verts[3].pos = Vec3(-c, c, zb);
    
    verts[0].texture[0] = 0.0; verts[0].texture[1] = 0.0;
    verts[1].texture[0] = 1.0; verts[1].texture[1] = 0.0;
    verts[2].texture[0] = 1.0; verts[2].texture[1] =1.0;
    verts[3].texture[0] = 0.0; verts[3].texture[1] = 1.0;
    //
    verts[0].tex_indx = 0.0;
    verts[1].tex_indx = 0.0;
    verts[2].tex_indx = 0.0;
    verts[3].tex_indx = 0.0;
    //
    verts[4].pos = Vec3(c,-c, zb);
    verts[5].pos = Vec3(c, -c, c);
    verts[6].pos = Vec3(c, c,  c);
    verts[7].pos = Vec3(c, c, zb);
    
    
    verts[4].texture[0] = 0.0; verts[4].texture[1] = 0.0;
    verts[5].texture[0] = 1.0; verts[5].texture[1] = 0.0;
    verts[6].texture[0] = 1.0; verts[6].texture[1] = 1.0;
    verts[7].texture[0] = 0.0; verts[7].texture[1] = 1.0;
    
    verts[4].tex_indx = 1.0;
    verts[5].tex_indx = 1.0;
    verts[6].tex_indx = 1.0;
    verts[7].tex_indx = 1.0;
    //###############################################################
    verts[8 ].pos = Vec3(c,-c, c);
    verts[9 ].pos = Vec3(-c, -c, c);
    verts[10].pos = Vec3(-c, c,  c);
    verts[11].pos = Vec3(c, c, c);
    
    
    verts[8 ].texture[0] = 0.0; verts[4].texture[1] = 0.0;
    verts[9 ].texture[0] = 1.0; verts[5].texture[1] = 0.0;
    verts[10].texture[0] = 1.0; verts[6].texture[1] = 1.0;
    verts[11].texture[0] = 0.0; verts[7].texture[1] = 1.0;
    
    verts[8 ].tex_indx = 2.0;
    verts[9 ].tex_indx = 2.0;
    verts[10].tex_indx = 2.0;
    verts[11].tex_indx = 2.0;
    
    
    //###############################################################
    verts[12].pos = Vec3(-c,-c, c);
    verts[13].pos = Vec3(-c, -c, zb);
    verts[14].pos = Vec3(-c, c,  zb);
    verts[15].pos = Vec3(-c, c, c);
    
    
    verts[12].texture[0] = 0.0; verts[4].texture[1] = 0.0;
    verts[13].texture[0] = 1.0; verts[5].texture[1] = 0.0;
    verts[14].texture[0] = 1.0; verts[6].texture[1] = 1.0;
    verts[15].texture[0] = 0.0; verts[7].texture[1] = 1.0;
    
    verts[12].tex_indx = 3.0;
    verts[13].tex_indx = 3.0;
    verts[14].tex_indx = 3.0;
    verts[15].tex_indx = 3.0;
    
    
    
    //###############################################################
    verts[16].pos = Vec3(-c,-c, c);
    verts[17].pos = Vec3(c, -c, c);
    verts[18].pos = Vec3(c, -c,  zb);
    verts[19].pos = Vec3(-c, -c, zb);
    
    
    verts[16].texture[0] = 0.0; verts[4].texture[1] = 0.0;
    verts[17].texture[0] = 1.0; verts[5].texture[1] = 0.0;
    verts[18].texture[0] = 1.0; verts[6].texture[1] = 1.0;
    verts[19].texture[0] = 0.0; verts[7].texture[1] = 1.0;
    
    verts[16].tex_indx = 4.0;
    verts[17].tex_indx = 4.0;
    verts[18].tex_indx = 4.0;
    verts[19].tex_indx = 4.0;
    
    //###############################################################
    verts[20].pos = Vec3(-c,c, zb);
    verts[21].pos = Vec3(c, c, zb);
    verts[22].pos = Vec3(c, c,  c);
    verts[23].pos = Vec3(-c, c, c);
    
    
    verts[20].texture[0] = 0.0; verts[4].texture[1] = 0.0;
    verts[21].texture[0] = 1.0; verts[5].texture[1] = 0.0;
    verts[22].texture[0] = 1.0; verts[6].texture[1] = 1.0;
    verts[23].texture[0] = 0.0; verts[7].texture[1] = 1.0;
    
    verts[20].tex_indx = 5.0;
    verts[21].tex_indx = 5.0;
    verts[22].tex_indx = 5.0;
    verts[23].tex_indx = 5.0;
    
    int count = 0;
    for (int i = 0; i<m_num_verts; i+=4){
        verts[i + 0].clr = Vec4(0.6, 0.2, 0.4, 1.0);
        verts[i + 1].clr = Vec4(0.6, 0.2, 0.4, 1.0);
        verts[i + 2].clr = Vec4(0.6, 0.2, 0.4, 1.0);
        verts[i + 3].clr = Vec4(0.6, 0.2, 0.4, 1.0);
        indices[count + 0] = i;
        indices[count + 1] = i + 1;
        indices[count + 2] = i + 2;
        
        indices[count + 3] = i + 3;
        indices[count + 4] = i + 2;
        indices[count + 5] = i;
        
        count+=6;
        
    }

}

//========Volume class for drawing volume data===================================
Volume_XY::Volume_XY(AMD::Vec3 dim, int num_slices)
:m_dimensions(dim), m_num_verts(4*num_slices), m_num_idx(6*num_slices)
{
    Gen_Verts(num_slices);
    AMD::Compute_norms(verts, indices, m_num_idx);
}

Volume_XY::Volume_XY(float w, float h, float d, int num_slices)
:m_dimensions(w,h,d), m_num_verts(4*num_slices), m_num_idx(6*num_slices)
{
    Gen_Verts(num_slices);
    AMD::Compute_norms(verts, indices, m_num_idx);
}



Volume_XY::~Volume_XY(){}

void Volume_XY::Gen_Verts(int num_slices){
    float dz = m_dimensions.z / (float)num_slices;
    float x = m_dimensions.x/2.0;
    float y = m_dimensions.y/2.0;
    float z = m_dimensions.z / 2.0;
    unsigned int count = 0;
    float tz = 0.0;
    float dtz = dz/m_dimensions.z;
    for (int i = 0; i< num_slices; i++){
        verts[count + 0].pos = Vec3(-x,-y, z);
        verts[count + 1].pos = Vec3(x,-y, z);
        verts[count + 2].pos = Vec3(x, y, z);
        verts[count + 3].pos = Vec3(-x,y, z);
        
        
        verts[count + 0].clr = Vec4(0.3,0.0,0.8,1.0);
        verts[count + 1].clr = Vec4(0.3,0.0,0.8,1.0);
        verts[count + 2].clr = Vec4(0.3,0.0,0.8,1.0);
        verts[count + 3].clr = Vec4(0.3,0.0,0.8,1.0);
        
        verts[count + 0].texture = Vec2(0.0,0.0);
        verts[count + 1].texture = Vec2(1.0,0.0);
        verts[count + 2].texture = Vec2(1.0,1.0);
        verts[count + 3].texture = Vec2(0.0,1.0);
        
        
        verts[count + 0].tex_indx = tz;
        verts[count + 1].tex_indx = tz;
        verts[count + 2].tex_indx = tz;
        verts[count + 3].tex_indx = tz;
        
        count += 4;
        z -= dz;
        tz += dtz;
    }
    
    count = 0;
    int v_num = 0;
    for (int i = 0; i<num_slices; i++){
        
        indices[count + 0] = v_num + 0;
        indices[count + 1] = v_num + 1;
        indices[count + 2] = v_num + 2;
        indices[count + 3] = v_num + 2;
        indices[count + 4] = v_num + 3;
        indices[count + 5] = v_num + 0;
        count += 6;
        v_num+=4;
        
    }
    
}

int Volume_XY::num_idx(){
    return m_num_idx;
}

int Volume_XY::num_verts(){
    return m_num_verts;
}



//========Volume class for drawing volume data===================================
Volume_XZ::Volume_XZ(AMD::Vec3 dim, int num_slices)
:m_dimensions(dim), m_num_verts(4*num_slices), m_num_idx(6*num_slices)
{
    Gen_Verts(num_slices);
    AMD::Compute_norms(verts, indices, m_num_idx);
}

Volume_XZ::Volume_XZ(float w, float h, float d, int num_slices)
:m_dimensions(w,h,d), m_num_verts(4*num_slices), m_num_idx(6*num_slices)
{
    Gen_Verts(num_slices);
    AMD::Compute_norms(verts, indices, m_num_idx);
}



Volume_XZ::~Volume_XZ(){}

void Volume_XZ::Gen_Verts(int num_slices){
    float dy = m_dimensions.y / (float)num_slices;
    float x = m_dimensions.x/2.0;
    float y = -m_dimensions.y/2.0;
    float z = m_dimensions.z / 2.0;
    unsigned int count = 0;
    float ty = 0.0;
    float dty = dy/m_dimensions.y;
    for (int i = 0; i< num_slices; i++){
        verts[count + 0].pos = Vec3(-x, y, -z);
        verts[count + 1].pos = Vec3(x, y, -z);
        verts[count + 2].pos = Vec3(x, y, z);
        verts[count + 3].pos = Vec3(-x, y, z);
        
        
        verts[count + 0].clr = Vec4(0.3,0.0,0.8,1.0);
        verts[count + 1].clr = Vec4(0.3,0.0,0.8,1.0);
        verts[count + 2].clr = Vec4(0.3,0.0,0.8,1.0);
        verts[count + 3].clr = Vec4(0.3,0.0,0.8,1.0);
        
        verts[count + 0].texture = Vec2(0.0,0.0);
        verts[count + 1].texture = Vec2(1.0,0.0);
        verts[count + 2].texture = Vec2(1.0,1.0);
        verts[count + 3].texture = Vec2(0.0,1.0);
        
        
        verts[count + 0].tex_indx = ty;
        verts[count + 1].tex_indx = ty;
        verts[count + 2].tex_indx = ty;
        verts[count + 3].tex_indx = ty;
        
        count += 4;
        y += dy;
        ty += dty;
    }
    
    count = 0;
    int v_num = 0;
    for (int i = 0; i<num_slices; i++){
        
        indices[count + 0] = v_num + 0;
        indices[count + 1] = v_num + 1;
        indices[count + 2] = v_num + 2;
        indices[count + 3] = v_num + 2;
        indices[count + 4] = v_num + 3;
        indices[count + 5] = v_num + 0;
        count += 6;
        v_num+=4;
        
    }
    
}

int Volume_XZ::num_idx(){
    return m_num_idx;
}

int Volume_XZ::num_verts(){
    return m_num_verts;
}





//========class for drawing Iso surfaces===================================

Surface_Cube::Surface_Cube(){}


Surface_Cube::Surface_Cube(AMD::Vec3 origin, AMD::Vec3 vox_len){
    float x0 = origin.x; float y0 = origin.y; float z0 = origin.z;
    float dx = 0.5*vox_len.x; float dy = 0.5*vox_len.y; float dz = 0.5*vox_len.z;
    float _2dx = vox_len.x; float _2dy = vox_len.y; float _2dz = vox_len.z;
    verts[0] = AMD::Vec3(x0 + dx, y0, z0);
    verts[1] = AMD::Vec3(x0 + _2dx, y0 + dy, z0);
    verts[2] = AMD::Vec3(x0 + dx, y0 + _2dy, z0);
    verts[3] = AMD::Vec3(x0, y0 + dy, z0);
    
    verts[4] = AMD::Vec3(x0 + dx, y0, z0 + _2dz);
    verts[5] = AMD::Vec3(x0 + _2dx, y0 + dy, z0 + _2dz);
    verts[6] = AMD::Vec3(x0 + dx, y0 + _2dy, z0 + _2dz);
    verts[7] = AMD::Vec3(x0, y0 + dy, z0 + _2dz);
    
    
    verts[8] = AMD::Vec3(x0, y0, z0 + dz);
    verts[9] = AMD::Vec3(x0 + _2dx, y0, z0 + dz);
    verts[10] = AMD::Vec3(x0, y0 + _2dy, z0 + dz);
    verts[11] = AMD::Vec3(x0 + _2dx, y0 + _2dy, z0 + dz);
}



void Surface_Cube::Set(AMD::Vec3 origin, AMD::Vec3 vox_len){
    float x0 = origin.x; float y0 = origin.y; float z0 = origin.z;
    float dx = 0.5*vox_len.x; float dy = 0.5*vox_len.y; float dz = 0.5*vox_len.z;
    float _2dx = vox_len.x; float _2dy = vox_len.y; float _2dz = vox_len.z;
    verts[0] = AMD::Vec3(x0 + dx, y0, z0);
    verts[1] = AMD::Vec3(x0 + _2dx, y0 + dy, z0);
    verts[2] = AMD::Vec3(x0 + dx, y0 + _2dy, z0);
    verts[3] = AMD::Vec3(x0, y0 + dy, z0);
    
    verts[4] = AMD::Vec3(x0 + dx, y0, z0 + _2dz);
    verts[5] = AMD::Vec3(x0 + _2dx, y0 + dy, z0 + _2dz);
    verts[6] = AMD::Vec3(x0 + dx, y0 + _2dy, z0 + _2dz);
    verts[7] = AMD::Vec3(x0, y0 + dy, z0 + _2dz);
    
    
    verts[8] = AMD::Vec3(x0, y0, z0 + dz);
    verts[9] = AMD::Vec3(x0 + _2dx, y0, z0 + dz);
    verts[10] = AMD::Vec3(x0, y0 + _2dy, z0 + dz);
    verts[11] = AMD::Vec3(x0 + _2dx, y0 + _2dy, z0 + dz);
}



Surface_Cube::~Surface_Cube(){}




//===========================================================
Quad::Quad()
:m_size(2.0)
{
    gen_verts();
}


Quad::Quad(float s)
:m_size(s)
{
    gen_verts();
}

Quad::Quad(AMD::Vec3 A, AMD::Vec3 B, AMD::Vec3 C, AMD::Vec3 D, const char* cw)
{
    gen_verts(A,B,C,D);
    if(strcmp(cw, "ccw") == 0){
        indices[0] = 0;
        indices[1] = 2;
        indices[2] = 1;
        indices[3] = 0;
        indices[4] = 3;
        indices[5] = 2;
    }
    AMD::Compute_norms(verts, indices, m_num_idx);
}


void Quad::gen_verts(){
    float _x = m_size/2.0;
    verts[0].pos = Vec3(-_x,-_x, 0.0);
    verts[1].pos = Vec3(_x,-_x, 0.0);
    verts[2].pos = Vec3(_x,_x, 0.0);
    verts[3].pos = Vec3(-_x,_x, 0.0);
    
    
    verts[0].clr = Vec4(0.3,0.0,0.8,1.0);
    verts[1].clr = Vec4(0.3,0.0,0.8,1.0);
    verts[2].clr = Vec4(0.3,0.0,0.8,1.0);
    verts[3].clr = Vec4(0.3,0.0,0.8,1.0);
    
    verts[0].norm = Vec3(-_x,-_x,0.0);
    verts[1].norm = Vec3(_x,-_x,0.0);
    verts[2].norm = Vec3(_x,_x,0.0);
    verts[3].norm = Vec3(-_x,_x,0.0);
    
    
    verts[0].texture = Vec2(0.0,0.0);
    verts[1].texture = Vec2(1.0,0.0);
    verts[2].texture = Vec2(1.0,1.0);
    verts[3].texture = Vec2(0.0,1.0);
 

}


void Quad::gen_verts(AMD::Vec3 A, AMD::Vec3 B, AMD::Vec3 C, AMD::Vec3 D){
    verts[0].pos = A;
    verts[1].pos = B;
    verts[2].pos = C;
    verts[3].pos = D;
    
    
    
    verts[0].clr = Vec4(0.3,0.0,0.8,1.0);
    verts[1].clr = Vec4(0.3,0.0,0.8,1.0);
    verts[2].clr = Vec4(0.3,0.0,0.8,1.0);
    verts[3].clr = Vec4(0.3,0.0,0.8,1.0);
    
    
    verts[0].texture = Vec2(0.0,0.0);
    verts[1].texture = Vec2(0.0,1.0);
    verts[2].texture = Vec2(1.0,1.0);
    verts[3].texture = Vec2(1.0,0.0);
    
    verts[0].tex_indx = 1.0;
    verts[1].tex_indx = 1.0;
    verts[2].tex_indx = 1.0;
    verts[3].tex_indx = 1.0;
 

}


int Quad::num_idx(){
    return m_num_idx;
}


int Quad::num_verts(){
    return m_num_verts;
}

Sphere::Sphere(float e_rad)
:rad(e_rad), m_clr(0.9,0.3,0.4,1.0){
    Gen_points();
    Gen_indices();
    
    
    for (int i = 0 ; i< m_num_verts; i++){
        basic_verts[i].pos =  verts[i].pos;
        basic_verts[i].clr =  verts[i].clr;
        basic_verts[i].norm = verts[i].norm;
    }
    
}


Sphere::~Sphere(){
    
}

void Sphere::Gen_points(){
    float d_phi = m_dth;
    float theta = PI - m_dth;
    float phi = 0.0;
    float x, y, z;
    int end = ((m_num_theta)/2) -1;
    verts[0].pos = Vec3(0.0,0.0,-rad);
    verts[0].clr = m_clr;
    verts[0].norm = Vec3(0.0,0.0,-rad);
    verts[0].texture[0] = 0.0; verts[0].texture[1] = phi;
    verts[0].tex_indx = 0.0;
    m_num_verts ++;
    for ( int i = 0; i<end; i++){
        for (int j = 0; j<m_num_theta; j++){
            phi = j*d_phi;
            x = rad*cos(phi)*sin(theta);
            y = rad*sin(phi)*sin(theta);
            z = rad*cos(theta);
            verts[m_num_verts].pos = Vec3(x,y,z);
            verts[m_num_verts].clr = m_clr;
            verts[m_num_verts].norm = Vec3(x,y,z);
            verts[m_num_verts].texture[0] = theta/3.14; verts[m_num_verts].texture[1] = phi/6.28;
            verts[m_num_verts].tex_indx = cos(theta)*cos(theta);
            m_num_verts ++;
            }
        theta-=m_dth;
    }
   
    verts[m_num_verts].pos = Vec3(0.0,0.0,rad);
    verts[m_num_verts].clr = m_clr;
    verts[m_num_verts].norm = Vec3(0.0,0.0,rad);
    verts[m_num_verts].texture[0] = 1.0; verts[0].texture[1] = 1.0;
    verts[m_num_verts].tex_indx = 0.0;
    m_num_verts ++;
    return;
}


void Sphere::Gen_indices(){
    int start = 0;
    int off_set = 1;
    
    for (int i = 1; i<m_num_theta + 1; i++){
        if ((i) % m_num_theta){
            indices[m_num_idx] = 0;
            indices[m_num_idx + 1] = i+ 1;
            indices[m_num_idx +2] = i;
            m_num_idx+=3;
        }
        else{
            indices[m_num_idx] = 0;
            indices[m_num_idx +1] = 1;
            indices[m_num_idx + 2] = i;
            m_num_idx+=3;
        }
     start++;
    } // end for loop
    
    
    for (int i = 1; i < m_num_verts - 1 - m_num_theta ; i++) {
        if((i) % m_num_theta){
            indices[m_num_idx] = i;
            indices[m_num_idx + 1] = i + 1;
            indices[m_num_idx + 2] = i + 1 + m_num_theta;
            
            
            indices[m_num_idx + 3] = i;
            indices[m_num_idx + 4] = i + m_num_theta + 1;
            indices[m_num_idx + 5] = i + m_num_theta;
            m_num_idx +=6;
            
            
            
        }
        
        
        else{
            indices[m_num_idx] = i;
            indices[m_num_idx + 1] = i + 1 - m_num_theta;
            indices[m_num_idx + 2] = i + 1;
            
            
            indices[m_num_idx + 3] = i;
            indices[m_num_idx + 4] = i + 1;
            indices[m_num_idx + 5] = i + m_num_theta;
            m_num_idx +=6;

        }
    }// end for loop
    off_set = m_num_verts - 1 - m_num_theta ;
    
    for (int i = off_set; i<m_num_verts -1; i++){
        if ((i) % m_num_theta){
            indices[m_num_idx] = i;
            indices[m_num_idx + 1] = i+1;
            indices[m_num_idx +2 ] = m_num_verts-1;
            m_num_idx+=3;
        }
        else{
            indices[m_num_idx] = i;
            indices[m_num_idx + 1] = off_set;
            indices[m_num_idx + 2] = m_num_verts -1;
            m_num_idx+=3;
        }
    }// end for loop
     
    return;
}
    










//======================================================
Cylinder::Cylinder()
:m_len(3.0), m_rad(0.5), m_dz(0.1)
{
    Gen_points();
    Gen_indices();
    AMD::Compute_norms(verts, indices, m_num_idx);
    
}

Cylinder::Cylinder(float e_len)
:m_len(e_len), m_dz(10.0/e_len)
{
    Gen_points();
    Gen_indices();
    
}

Cylinder::~Cylinder() {}



void Cylinder::Gen_points() {
    int num_z = (int)(m_len/m_dz);
    float x, y, theta;
    float z = -0.5*m_len;
    int count = 0;
    for (int i = 0; i<= num_z; i++){
        theta = 0.0;
        for (int j = 0; j< m_num_theta; j++) {
            x = m_rad*cos(theta);
            y = m_rad*sin(theta);
            verts[count].pos = Vec3(x,y,z);
            verts[count].clr = Vec4(abs(sin(theta)),0.0,abs(cos(theta)),1.0);
            verts[count].texture[0] = theta; verts[count].texture[1] = abs(z);
            theta +=m_dth;
            count++;
        }
        z+=m_dz;
    }
    m_num_verts = count;
}

void Cylinder::Gen_indices() {
    int count = 0;
    for (int i = 0; i < m_num_verts - m_num_theta ; i++) {
        if((i+1) % m_num_theta){
            indices[count] = i;
            indices[count + 1] = i + 1;
            indices[count + 2] = i + 1 + m_num_theta;
            
            
            indices[count + 3] = i;
            indices[count + 4] = i + m_num_theta + 1;
            indices[count + 5] = i + m_num_theta ;
            count +=6;
            
            
            
        }
        
        
        else{
            indices[count] = i;
            indices[count + 1] = i + 1 - m_num_theta;
            indices[count + 2] = i + 1;
            
            
            indices[count + 3] = i;
            indices[count + 4] = i + 1;
            indices[count + 5] = i + m_num_theta;
            count +=6;

        }
    }
    
    m_num_idx = count;
}




void Cylinder2::Gen_points() {
    float x, y, theta;
    double z[2] = {-0.5*m_len ,0.5*m_len};
    int count = 0;
    for (int i = 0; i< 2; i++){
        theta = 0.0;
        for (int j = 0; j< m_num_theta; j++) {
            x = m_rad*cos(theta);
            y = m_rad*sin(theta);
            verts[count].pos = Vec3(x,y,z[i]);
            verts[count].clr = Vec4(abs(sin(theta)),0.0,0.5,1.0);
            verts[count].texture[0] = theta; verts[count].texture[1] = float(i);
            verts[count].tex_indx = 0.0;
            theta +=m_dth;
            count++;
        }
    }
    m_num_verts = count;
}

void Cylinder2::Gen_indices() {
    for (int i = 0; i< m_num_theta; i++){
        if((i+1) % m_num_theta){
            indices[m_num_idx] = i;
            indices[m_num_idx + 1] = i + 1;
            indices[m_num_idx + 2] = i + 1 + m_num_theta;
            
            
            indices[m_num_idx + 3] = i;
            indices[m_num_idx + 4] = i + m_num_theta +1;
            indices[m_num_idx + 5] = i + m_num_theta ;
            m_num_idx +=6;
            
            
            
        }
        
        
        else{
            indices[m_num_idx] = i;
            indices[m_num_idx + 1] = i + 1 - m_num_theta;
            indices[m_num_idx + 2] = i + 1;
            
            
            indices[m_num_idx + 3] = i;
            indices[m_num_idx + 4] = i + 1;
            indices[m_num_idx + 5] = i + m_num_theta;
            m_num_idx +=6;

        }
    }
}

Cylinder2::Cylinder2(float e_len)
:m_len(e_len), m_rad(0.5){
    Gen_points();
    Gen_indices();
    AMD::Compute_norms(verts, indices, m_num_idx);
}

Cylinder2::Cylinder2()
:m_len(3.0), m_rad(0.5)
{
    Gen_points();
    Gen_indices();
    AMD::Compute_norms(verts, indices, m_num_idx);
}

Cylinder2::~Cylinder2() {}














void Cone::Gen_points() {
    float r; float _z = 0.0; float dz = 0.1;
    float dth = 0.314159;
    float theta;
    int count = 0;
    float _x, _y;
    for (int i =0; i < 10; i++){
        theta = 0.0;
        r = 1.0 - _z;
        for (int j = 0; j < m_num_theta; j++) {
            _x = r*cos(theta);
            _y = r* sin(theta);
            verts[count].pos = Vec3(_x,_y,_z);
            verts[count].clr = Vec4(0.0,0.0,1.0,1.0);
            theta+=dth;
            count ++;
        }
        _z+=dz;
    }
    verts[count].pos = Vec3(0.0, 0.0, 1.0);
    verts[count].clr = Vec4(0.0,0.0,1.0,1.0);
    m_num_verts = count + 1;
}

void Cone::Gen_indices() {
    int count = 0;
    for (int i = 0; i < m_num_verts - m_num_theta - 1; i++) {
        if((i+1) % m_num_theta){
            indices[count] = i;
            indices[count + 1] = i + 1;
            indices[count + 2] = i + 1 + m_num_theta;
            
            
            indices[count + 3] = i;
            indices[count + 4] = i + m_num_theta + 1;
            indices[count + 5] = i + m_num_theta ;
            count +=6;
            
            
            
        }
        
        
        else{
            indices[count] = i;
            indices[count + 1] = i + 1 - m_num_theta;
            indices[count + 2] = i + 1;
            
            
            indices[count + 3] = i;
            indices[count + 4] = i + 1;
            indices[count + 5] = i + m_num_theta;
            count +=6;

        }
    }
    
    
    int start = m_num_verts - m_num_theta - 1;
    int end = m_num_verts - 1;
    for (int i = start; i < end; i++){
        if((i+1) % m_num_theta){
            indices[count] = i;
            indices[count + 1] = i + 1;
            indices[count + 2] = m_num_verts -1;
            count +=3;
            
            }
        
        
        else{
            indices[count] = i;
            indices[count + 1] = start;
            indices[count + 2] = m_num_verts - 1;
            count +=3;

        }
        
        
    }
    
    m_num_idx = count;
}

Cone::Cone() { 
    Gen_points();
    Gen_indices();
    AMD::Compute_norms(this ->verts, this->indices, m_num_idx);
}

Cone::~Cone() {}








Arrow::Arrow(char color)
:m_num_z(20){
    switch (color) {
        case 'r':
            m_color = Vec4(1.0,0.0,0.0,1.0);
            break;
        case 'g':
            m_color = Vec4(0.0,1.0,0.0,1.0);
            break;
            
        case 'b':
            m_color = Vec4(0.0,0.0,1.0,1.0);
            break;
            
        default:
            break;
    }
    
    Gen_points();
    Gen_indices();
    AMD::Compute_norms(this->verts, this->indices, m_num_idx);
    //Coordinate_Transform();
}

Arrow::Arrow(char color, char dir)
: m_length(3.0), m_num_z(30){
    switch (color) {
        case 'r':
            m_color = Vec4(1.0,0.0,0.0,1.0);
            break;
        case 'g':
            m_color = Vec4(0.0,1.0,0.0,1.0);
            break;
            
        case 'b':
            m_color = Vec4(0.0,0.0,1.0,1.0);
            break;
            
        default:
            break;
    }
    
    switch (dir) {
        case 'x':
            r_mat = ROTATION_MATRIX(AMD::Vec3(0.0, halfPI, 0.0));
            break;
        
        case 'y':
            r_mat = ROTATION_MATRIX(AMD::Vec3(0.0, 0.0, -halfPI));
            break;
            
        case 'z':
            r_mat = ROTATION_MATRIX(AMD::Vec3(0.0, 0.0, 0.0));
            break;
        default:
            break;
    }
    
    
    Gen_points();
    Gen_indices();
    Gen_Quad();
    AMD::Compute_norms(this->verts, this->indices, m_num_idx);
    Rotation();
    Coordinate_Transform();
}


Arrow::Arrow(char color, char dir, float len)
:m_length(len), m_num_z((int)(len/0.1))
{
    switch (color) {
        case 'r':
            m_color = Vec4(0.6,0.1,0.9,1.0);
            break;
        case 'g':
            m_color = Vec4(0.0,1.0,0.0,1.0);
            break;
            
        case 'b':
            m_color = Vec4(0.0,0.0,1.0,1.0);
            break;
            
        default:
            break;
    }
    
    switch (dir) {
        case 'x':
            r_mat = ROTATION_MATRIX(AMD::Vec3(0.0, halfPI, 0.0));
            label_rot_mat = ROTATION_MATRIX(Vec3(0.0, halfPI, 0.0));
            text_id =2.0;
            break;
        
        case 'y':
            r_mat = ROTATION_MATRIX(AMD::Vec3(0.0, 0.0, -halfPI));
            label_rot_mat = ROTATION_MATRIX(Vec3(0.0, halfPI,halfPI));
            text_id =3.0;
            break;
            
        case 'z':
            r_mat = ROTATION_MATRIX(AMD::Vec3(0.0, 0.0, 0.0));
            label_rot_mat = ROTATION_MATRIX(Vec3(0.0, halfPI, 0.0));
            text_id =4.0;
            break;
        default:
            break;
    }
    
    
    Gen_points();
    Gen_indices();
    Gen_Quad();
    AMD::Compute_norms(this->verts, this->indices, m_num_idx);
    Rotation();
    Coordinate_Transform();
}

Arrow::~Arrow() {}




void Arrow::Gen_points() {
    float r = 0.25; float _z = 0.0; float dz = 0.1;
    float theta;
    int count = 0;
    float _x, _y, _z_start;
    

    for (int i =0; i < m_num_z; i++){
        theta = 0.0;
        for (int j = 0; j < m_num_theta; j++) {
            _x = r*cos(theta);
            _y = r* sin(theta);
            verts[count].pos = Vec3(_x,_y,_z);
            verts[count].clr = m_color;
            verts[count].texture[0] = theta; verts[count].texture[1] = _z;
            verts[count].tex_indx = 1.0;
            theta+=m_dth;
            count ++;
        }
        if (i < m_num_z - 1){
            _z+=dz;
        }
    }
    _z_start = _z;
    for (int i =0; i < 10; i++){
        theta = 0.0;
        r = 0.5*(1.0 - (_z - _z_start));
        for (int j = 0; j < m_num_theta; j++) {
            _x = r*cos(theta);
            _y = r* sin(theta);
            verts[count].pos = Vec3(_x,_y,_z);
            verts[count].clr = m_color;
            verts[count].texture[0] = 0.5; verts[count].texture[1] = 0.5;
            verts[count].tex_indx = 0.0;
            theta+=m_dth;
            count ++;
        }
        _z+=dz;
    }
    verts[count].pos = Vec3(0.0, 0.0, _z);
    verts[count].clr = m_color;
    verts[count].texture[0] = 0.5; verts[count].texture[1] = 0.5;
    verts[count].tex_indx = 0.0;
    m_num_verts = count + 1;
}

void Arrow::Gen_indices() {
    int count = 0;
    for (int i = 0; i < m_num_verts - m_num_theta - 1; i++) {
        if((i+1) % m_num_theta){
            indices[count] = i;
            indices[count + 1] = i + 1;
            indices[count + 2] = i + 1 + m_num_theta;
            
            
            indices[count + 3] = i;
            indices[count + 4] = i + m_num_theta + 1;
            indices[count + 5] = i + m_num_theta ;
            count +=6;
            
            
            
        }
        
        
        else{
            indices[count] = i;
            indices[count + 1] = i + 1 - m_num_theta;
            indices[count + 2] = i + 1;
            
            
            indices[count + 3] = i;
            indices[count + 4] = i + 1;
            indices[count + 5] = i + m_num_theta;
            count +=6;

        }
    }
    
    
    int start = m_num_verts - m_num_theta - 1;
    int end = m_num_verts - 1;
    for (int i = start; i < end; i++){
        if((i+1) % m_num_theta){
            indices[count] = i;
            indices[count + 1] = i + 1;
            indices[count + 2] = m_num_verts -1;
            count +=3;
            
            }
        
        
        else{
            indices[count] = i;
            indices[count + 1] = start;
            indices[count + 2] = m_num_verts - 1;
            count +=3;

        }
        
        
    }
    
    m_num_idx = count;
}




void Arrow::Coordinate_Transform(){
    for (int i = 0 ; i< m_num_verts; i++){
        verts[i].pos = CT * verts[i].pos;
        verts[i].norm = CT * verts[i].norm;
    }
}


void Arrow::Rotation(){
    for (int i = 0 ; i< m_num_verts; i++){
        verts[i].pos = r_mat* verts[i].pos;
        verts[i].norm = r_mat * verts[i].norm;
    }
}


void Arrow::Gen_Quad(){
    AMD::Vec4 delta(0.0, -0.6, 1.9,1.0);
    AMD::Vec3 temp;
    label_rot_mat.assign_row(3, delta);
    Quad qd(1.0);
    int off_set_verts = m_num_verts;
    for (int i = 0; i<qd.num_verts(); i++){
        qd.verts[i].pos = label_rot_mat*qd.verts[i].pos;
        qd.verts[i].pos = qd.verts[i].pos*0.75;
        verts[m_num_verts] = qd.verts[i];
        verts[m_num_verts].clr = m_color;
        verts[m_num_verts].tex_indx = text_id;
        m_num_verts++;
    }
    
    for (int j = 0; j < qd.num_idx(); j++){
        indices[m_num_idx] = qd.indices[j] + off_set_verts;
        m_num_idx++;
    }
    
}


//###########END of SHAPES#############################################



//========================================================
Grid::Grid(int rows, float row_width, int cols, float col_width)
:m_row_width(0.0), m_w(cols*col_width), m_h(rows*row_width), m_num_cols(cols), m_num_rows(rows)
{
    gen_points();
    gen_line_indices();
    gen_tri_indices();
}


Grid::Grid(std::string file_name)
:m_row_width(0.0), m_num_points(163)
{
    ReadXYZ(file_name, verts, m_num_verts);
    gen_line_indices();
    gen_tri_indices();
}

Grid::~Grid(){}

int Grid::num_line_idx() {
    return m_num_line_idx;
}

int Grid::num_tri_idx() {
    return m_num_tri_idx;
}

int Grid::num_verts() {
    return  m_num_verts;;
}

void Grid::gen_points() {
    float dx = m_w/m_num_cols;
    float dz = m_h/m_num_rows;
    int count = 0;
    float x =-0.5*m_w; float y = 0.0; float z = -0.5*m_h;
    for (int i = 0; i< m_num_rows; i++){
        x = -0.5*m_w;
        for (int j = 0; j<m_num_cols; j++){
            
            verts[count].pos = Vec3(x,y,z);
            verts[count].clr = Vec4(0.9,0.4,0.1,1.0);
            verts[count].norm = Vec3(0.0,0.0,1.0);
            verts[count].texture[0] = x; verts[count].texture[1] = z;
            verts[count].tex_indx = 0.0;
            count++;
            x+=dx;

        }
        z+=dz;
    }
    m_num_verts = count;
}

void Grid::gen_line_indices() {
    int count =0;
    int a, b;
    for (int row = 0; row< m_num_rows; row++){
        for(int col = 0; col < m_num_cols - 1; col++){
            a = row*m_num_cols + col;
            b = a + 1;
            L_indices[count + 0] = a;
            L_indices[count + 1] = b;
            count +=2;
        }
    }
    
    for(int col = 0; col < m_num_cols; col++){
        for (int row = 0; row< m_num_rows - 1; row++){
            a = row*m_num_cols + col;
            b = a + m_num_cols;
            L_indices[count + 0] = a;
            L_indices[count + 1] = b;
            count +=2;
        }
    }
    
    m_num_line_idx = count;
}



void Grid::gen_tri_indices(){
    int count =0;
    int a, b, c, d;
    for (int row = 0; row< m_num_rows - 1; row++){
        for(int col = 0; col < m_num_cols - 1; col++){
            a = row*m_num_cols + col;
            b = a + 1;
            c = a + m_num_cols;
            d = c + 1;
            T_indices[count + 0] = a;
            T_indices[count + 1] = b;
            T_indices[count + 2] = d;
            //------------------------
            T_indices[count + 3] = d;
            T_indices[count + 4] = c;
            T_indices[count + 5] = a;
            count +=6;
        }
    }
    
    m_num_tri_idx = count;
    return;
}

void Grid::Set_Height(float** arr, int nx, int nz){
    int count = 0;
    float min = 10000.0;
    float max = 0.0;
    //if (nx > m_num_x || nz > m_num_z){ERROR_LOG(OUT_OF_RANGE);}
    float val = 0;
    for (int i = 0; i < nx; i++){
        for ( int j = 0 ; j < nz; j++){
            val = arr[i][j];
            if (val< min){min = val;}
            if (val> max){max = val;}
        }
    }
    float mod = (0.5*m_w)/(max - min);
    for (int i = 0; i < nx; i++){
        for ( int j = 0 ; j < nz; j++){
            val =(-0.5*m_w) +  mod*arr[i][j];
            verts[count].pos.y = val;
            float normed_h = (arr[i][j] - min)/(max - min);
            verts[count].clr = AMD::Vec4(1.0 - normed_h, 0.0, normed_h, 1.0);
            count ++;
        }
    }
    
}






void ReadXYZ(std::string in_file, Vertex_TX* verts, int& num_verts){
    
    std::ifstream f_dat;
    f_dat.open(in_file, std::ios::in);
    float x, y, z;
    float cmap;
    int i = 0;
    if (!f_dat.is_open()) {
        std::cout << "FILE DID NOT OPEN!!!" << std::endl;
        exit(-1);
    }
    while (f_dat >> x >> y >> z) {
        cmap = (0.8 + z) / 1.6;
        if (/* DISABLES CODE */ (true)) {
        verts[i].pos = Vec3(x,z,y);
        verts[i].clr = Vec4(cmap, 0.5 + cmap, 1.0 - cmap ,1.0);
        verts[i].norm = Vec3(0.0,0.0,0.0);
        i++;
        }
        else{
            verts[i].pos = Vec3(x,z,y);
            verts[i].clr = Vec4(cmap, 0.2, 1.0 - cmap ,1.0);
            verts[i].norm = Vec3(0.0,0.0,0.0);
            i++;
        }
    }
    
    num_verts = i;
    f_dat.close();
    return;
    
}


