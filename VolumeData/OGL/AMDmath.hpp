//
//  AMDmath.hpp
//  SimViz
//
//  Created by Andrew Diggs on 1/1/23.
//

#ifndef AMDmath_hpp
#define AMDmath_hpp


#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <thread>
#include <chrono>

#define twoPI 6.283185307
#define PI 3.141592654
#define halfPI 1.5707963

#define U_2_G 1.66e+24


namespace AMD {
enum Projection_Type {Orthographic, Perspective};
struct Quat;

struct Vec2{
    float x,y;
    Vec2(float x, float y);
    Vec2();
    ~Vec2();
    
    
    Vec2 add(const Vec2& other) const;
    float& operator[](const int index);
    float* get();
    
    float dot(const Vec2& other) const;
    float len() const;
    
    
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2& operator=(const Vec2& other);
    Vec2 operator*(float scale);
    Vec2 operator/(float div);
    Vec2 operator+=(const Vec2& other);
    Vec2 operator*=(float scale);
    void print();
    
    
    
    
};


struct Vec3{
    float x,y,z;
    Vec3();
    Vec3(float s);
    Vec3(float e_x, float e_y, float e_z);
    
    Vec3 add(const Vec3& other) const;
    float& operator[](const int index);
    float* get();
    
    float dot(const Vec3& other) const;
    Vec3 cross(const Vec3& other) const;
    float len() const;
    
    
    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3& operator=(const Vec3& other);
    Vec3& operator=(const Quat& other);
    Vec3 operator*(float scale) const;
    Vec3 operator/(float div) const;
    Vec3 operator+=(const Vec3& other);
    Vec3 operator*=(float scale);
    void Rotate_Quaternion(const Vec3& axis, float ang);
    void Reset();
    void print();
    void Normalize();
    void Vround(int decimals);
    
    
};


struct Vec4{
    float r,g,b,a;
    Vec4();
    Vec4(float s);
    Vec4(float e_r, float e_g, float e_b, float e_a);
    Vec4(float* e_vec);
    Vec4(Vec3 vec);
    
    
    float* get();
    float& operator[](const int& index);
    Vec4 operator+(const Vec4& other) const;
    //Vec4 operator*(float scale);
    void Reset();
    
};


struct Quat{
    float w,x,y,z;
    
    Quat();
    Quat(float ew, float ex, float ey, float ez);
    Quat(Vec3 coords);
    Quat(float ang, Vec3 coords);
    ~Quat();
    
    
    
    Quat inverse();
    
    float* get();
    float& operator[](const int& index);
    Quat& operator=(const Quat& other);
    
    
    
    
    
};

Quat operator*(const Quat& L,const Quat& R);


struct Vertex{
    Vec3 pos;
    Vec4 clr;
    Vec3 norm;
    Vec2 texture;
    float tex_indx;
    
    Vertex& operator=(const Vertex& other);
    Vertex off_set(AMD::Vec3 vec);
    
};



struct Mat3{
    const unsigned int n =3;
    float m[3][3];
    Mat3();
    
    
    float* operator[](const int& index);
    Mat3& operator=(const Mat3& other);
    Mat3 add(const Mat3& other) const;
    Mat3 operator+(const Mat3& other) const;
    
    Mat3 multiply(const Mat3& other) const;
    Mat3 operator*(const Mat3& other) const;
    Vec3 multiply(Vec3& other) const;
    Vec3 operator*(Vec3& other) const;
    
    void assign_col(int col_idx, Vec3 col);
    void assign_row(int row_idx, Vec3 row);
    
    void Rotate(Vec3 ang);
    void Scale(float);
    void Scale(Vec3);
    void Transpose();
    
    void print();
};




struct Mat4{
    const unsigned int n =4;
    float m[4][4];
    Mat4();
    
    
    float* operator[](const int& index);
    Mat4& operator=(const Mat4& other);
    Mat4 add(const Mat4& other) const;
    Mat4 operator+(const Mat4& other) const;
    
    Mat4 multiply(const Mat4& other) const;
    Mat4 operator*(const Mat4& other) const;
    Vec4 multiply(Vec4& other) const;
    Vec4 operator*(Vec4& other) const;
    Vec3 operator*(Vec3& other) const;
    Vec4 Get_Col(int col) const;
    
    
    void assign_col(int col_idx, Vec4 col);
    void assign_row(int row_idx, Vec4 row);
    void assign_col(int col_idx, Vec3 col);
    void assign_row(int row_idx, Vec3 row);
    
    
    
    void Rotate(Vec3 ang);
    void Scale(float);
    void Scale(Vec3);
    void Translate(Vec3 vec);
    void Transpose();
    void Reset();
    void print();
};

struct Timer{
    std::string proc_name;
    std::chrono::steady_clock::time_point start, end;
    Timer(const char*);
    ~Timer();
};


Mat4 ID();

Mat4 ROTATION_MATRIX(Vec3 ang);

float Get_angle(const Vec3& A, const Vec3& B);

float Distance(const Vec3& A, const Vec3& B);
void Compute_norms(Vertex* verts,unsigned int* ints, int num);

void Map_Texture_Coords(Vertex* verts, int num_verts);

float Round(float val, int num_decimal);

Vec3 Round(const Vec3& vec, int decimals);

Mat4 Projection(Projection_Type, Vec4);
Mat4 Perspective_Matrix(Vec4);
Mat4 Orthographic_Matrix(Vec4);


int Min(int a, int b);
float Min(float a, float b);
int Max(int a, int b);
float Max(float a, float b);

float AMU_to_G(float u);



}




#endif /* AMDmath_hpp */
