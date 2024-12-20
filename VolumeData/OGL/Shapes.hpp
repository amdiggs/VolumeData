//
//  Shapes.hpp
//  OpenGL
//
//  Created by Andrew Diggs on 7/14/22.
//

#ifndef Shapes_hpp
#define Shapes_hpp

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "AMDmath.hpp"



#define MAX_VERTS 10000
#define MAX_IDX 50000






class SHAPE{
protected:
    int m_num_idx;
    int m_num_verts;
    const int m_num_theta = 60;
    const float m_dth = twoPI/m_num_theta;
    void Gen_cyl_index(int off_set);
    void Gen_fan_index(int count, int tip);
public:
    SHAPE();
    virtual ~SHAPE() = 0;
    AMD::Vertex_TX verts[MAX_VERTS];
    unsigned int indices[MAX_IDX];
    int num_idx() const ;
    int num_verts() const;
    virtual void Gen_points() = 0;
    virtual void Gen_indices() = 0;
    
    
};



class Circle: public SHAPE{
private:
    float rad;
    void Gen_points() override;
    void Gen_indices() override;
public:
    Circle(float ex_rad);
    ~Circle();
    

    
};

class Triangle{
private:
    AMD::Vec3 a;
    AMD::Vec3 b;
    AMD::Vec3 c;
    
public:
    Triangle(){};
    Triangle(AMD::Vec3 ea, AMD::Vec3 eb, AMD::Vec3 ec):a(ea), b(eb), c(ec){};
    ~Triangle(){};
    
    AMD::Vec3& operator[](const int idx);
    float Area();
};


class Cube{
private:
    int m_num_idx = 36;
    int m_num_verts = 8;
    AMD::Vec3 m_lengths;

    void Gen_Points();
    
    
    
public:
    Cube();
    Cube(float len);
    Cube(AMD::Vec3 BB);
    ~Cube();
    AMD::Vec3 pos[8];
    AMD::Vec4 clrs[8];
    AMD::Vertex_TX verts[8];
    AMD::Vertex_Basic basic_verts[8];
    unsigned int indices[36] = {
        // front
        0, 3, 2,
        2, 1, 0,
        // right
        1, 2, 6,
        6, 5, 1,
        // back
        4, 5, 6,
        6, 7, 4,
        // left
        4, 7, 3,
        3, 0, 4,
        // bottom
        0, 4, 5,
        5, 1, 0,
        // top
        3, 7, 6,
        6, 3, 2
    };
    
    
    /*
     
          7-------6
         /|       /
        / |      /
       3--|-----2
       |  4------5
       |       |
       0-------1
     
     
        {0,1,2}, {0,2,3}, {0,1,4}, {1,4,5},
        {2,3,7}, {2,6,7}, {1,2,5}, {1,5,6},
        {0,3,4}, {3,4,7}, {4,5,6}, {4,6,7}
     */
    
    
    int num_idx();
    int num_verts();
};


class Environment_Cube{
public:
    int m_num_verts = 24;
    int m_num_idx = 36;
    void Gen_Points();
    
    Environment_Cube();
    ~Environment_Cube();
    AMD::Vertex_TX verts[24];
    unsigned int indices[36]; 
    
    
    
};

class Volume_XY{
private:
    void Gen_Verts(int num_slices);
    int m_num_verts = 0;
    int m_num_idx = 0;
    AMD::Vec3 m_dimensions;
    
public:
    Volume_XY(AMD::Vec3 dim, int num_slices);
    Volume_XY(float w, float h, float d, int num_slices);
    ~Volume_XY();
    
    AMD::Vertex_TX verts[MAX_VERTS];
    unsigned int indices[MAX_IDX];
    int num_idx();
    int num_verts();
};

//====================================================

class Volume_XZ{
private:
    void Gen_Verts(int num_slices);
    int m_num_verts = 0;
    int m_num_idx = 0;
    AMD::Vec3 m_dimensions;
    
public:
    Volume_XZ(AMD::Vec3 dim, int num_slices);
    Volume_XZ(float w, float h, float d, int num_slices);
    ~Volume_XZ();
    
    AMD::Vertex_TX verts[MAX_VERTS];
    unsigned int indices[MAX_IDX];
    int num_idx();
    int num_verts();
};



class Surface_Cube{
private:
    
public:
    AMD::Vec3 verts[12];
    Surface_Cube();
    Surface_Cube(AMD::Vec3 origin, AMD::Vec3 vox_len);
    ~Surface_Cube();
    
    void Set(AMD::Vec3 origin, AMD::Vec3 vox_len);
    
};

class Quad{
private:
    void gen_verts();
    void gen_verts(AMD::Vec3 A, AMD::Vec3 B, AMD::Vec3 C, AMD::Vec3 D);
    int m_num_verts = 4;
    int m_num_idx = 6;
    float m_size;
public:
    Quad();
    Quad(float s);
    Quad(float s, float z);
    Quad(AMD::Vec3 A, AMD::Vec3 B, AMD::Vec3 C, AMD::Vec3 D, const char* cw);
    
    AMD::Vertex_TX verts[4];
    unsigned int indices[6] = {0,1,2,2,3,0};
    int num_idx();
    int num_verts();
};


class Sphere : public SHAPE{
private:
    float rad;
    void Gen_points() override;
    void Gen_indices() override;
    
public:
    AMD::Vertex_Basic basic_verts[MAX_VERTS];
    Sphere(float e_rad);
    ~Sphere();
    
    AMD::Vec4 m_clr;
    
    
    
};




class Cylinder: public SHAPE{
private:
    float m_dz;
    float m_rad;
    float m_len;
    void Gen_points() override;
    void Gen_indices() override;
    
    
public:
    Cylinder(float e_len);
    Cylinder();
    ~Cylinder();
    
};


class Cylinder2: public SHAPE{

private:
    float m_rad;
    float m_len;
    void Gen_points() override;
    void Gen_indices() override;
    
    
public:
    Cylinder2(float e_len);
    Cylinder2();
    ~Cylinder2();
};


class Cone: public SHAPE{
private:
    void Gen_points() override;
    void Gen_indices() override;
    
    
public:
    Cone();
    ~Cone();
    
    
    
};



class Arrow: public SHAPE {
    
private:
    float m_length;
    int m_num_z;
    float text_id;
    AMD::Vec4 m_color;
    AMD::Mat4 r_mat;
    AMD::Mat4 label_rot_mat;
    AMD::Vec3 delta;
    void Gen_points() override;
    void Gen_indices() override;
    void Gen_Quad();
    void Coordinate_Transform();
    void Rotation();
    
    
public:
    Arrow(char color);
    Arrow(char color, char dir);
    Arrow(char color, char dir, float len);
    ~Arrow();
    
};


class Grid{
    
private:
    static const unsigned int max_verts = 50000;
    static const unsigned int max_indices = 250000;
    float m_w;
    float m_h;
    float m_row_width, m_col_width;
    int m_num_cols, m_num_rows;
    int m_num_points;
    int m_num_line_idx;
    int m_num_tri_idx;
    int m_num_verts;
    void gen_points();
    void gen_line_indices();
    void gen_tri_indices();
    void add_norm();
    
public:
    Grid(int rows, float row_width, int cols, float col_width);
    Grid(std::string file_name);
    ~Grid();
    AMD::Vertex_TX verts[max_verts];
    unsigned int L_indices[max_indices];
    unsigned int T_indices[max_indices];
    int num_line_idx();
    int num_tri_idx();
    void Set_Height(float** arr, int nx, int nz);
    int num_verts();
    
};


void ReadXYZ(std::string in_file, AMD::Vertex_TX* verts, int& num_verts);
AMD::Vec3 Normed_average(AMD::Vec3 Va, AMD::Vec3 Vb);


#endif /* Shapes_hpp */
