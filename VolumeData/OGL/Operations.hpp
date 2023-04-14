//
//  Operations.hpp
//  OpenGL
//
//  Created by Andrew Diggs on 8/3/22.
//

#ifndef Operations_hpp
#define Operations_hpp

#include <stdio.h>
#include <cmath>
#include "AMDmath.hpp"

enum Projection_Type {Orthographic, Perspective};


class Shader;
class Light_Src;
class Camera{
  
private:
    AMD::Vec3 m_pos;;
    AMD::Vec3 m_up;
    AMD::Vec3 m_target;
    
    
public:
    Camera();
    ~Camera();
    
    
    
    AMD::Vec3& Get_Pos();
    AMD::Vec3& Get_Up();
    AMD::Vec3& Get_Target();
    
    AMD::Mat4 Get_Matrix() const;
    float m_theta, m_phi;
    float m_zoom;
    
    void Move_ForwardBackward(float del);
    void Move_LeftRight(float del);
    void Move_UpDown(float del);
    void Move_To(AMD::Vec3 pos);
    
    
    void Look_UpDown(float ang);
    void Look_LeftRight(float ang);
    void Look_At(AMD::Vec3 p);
    
};


class Model{
private:
    float m_scale =1.0;
    AMD::Vec3 m_pos;;
    AMD::Vec3 m_up;
    AMD::Vec3 m_normal;
    
    
public:
    Model();
    ~Model();
    
    AMD::Vec3& Get_Pos();
    AMD::Vec3& Get_Up();
    AMD::Vec3& Get_Normal();
    float& Get_Scale();
    
    AMD::Mat4 Get_Matrix() const;
    AMD::Mat3 Get_Rotation_Matrix() const;
    float m_theta, m_phi;
    
    void Set_Scale(float del);
    void Set_Normal(AMD::Vec3 Cam_Target);
    void Move_LeftRight(float del);
    void Move_UpDown(float del);
    void Move_To(AMD::Vec3 pos);
    
    
    void Rotate_Azimuth(float ang);
    void Rotate_Pole(float ang);
    
    
};

//#########################################################
class Operator{
private:
    Operator();
    Operator(const Operator&) = delete;
    static Operator inst;
    
    
public:
    int MVP_loc, Norm_loc;
    bool uniform_locs_set;
    bool need_update;
    
    float w_scale = 9.0/16.0;
    
    //Normal Operator
    AMD::Mat3 Normal_mat;
    const float* Norm_ptr = &Normal_mat.m[0][0];
    
    
    
    //Camera
    Model m_model;
    
    Camera m_Cam;
    
    AMD::Mat4 MV_mat;
    const float* MV_ptr = &MV_mat.m[0][0];
    
    
    //Projection Matrix
    AMD::Mat4 Proj_mat;
    const float* Proj_ptr = &Proj_mat.m[0][0];
    AMD::Vec4 Proj_vec;
    
    
    AMD::Mat4 MVP_mat;
    const float* MVP_ptr = &MVP_mat.m[0][0];
    
    
    //static Operator s_ints;
    static Operator* Get_Inst();
    ~Operator();
    
    
    
    void Set_W_Scale(int w, int h);
    

    void Projection(AMD::Vec4 vec);
    void set_projection();
    AMD::Vec4& Get_Proj_vec();
    
    
    void set_MV();
    void set_MVP();
    void Set();
    
    AMD::Mat3 Get_Normal() const;
    AMD::Mat4 Get_MV() const;
    AMD::Mat4 Get_MVP() const;
    AMD::Mat4 Get_Model() const;
    AMD::Mat4 Get_Proj() const;
    AMD::Mat4 Get_VP() const;
    AMD::Mat4 Get_MLP(const Light_Src& lsrc) const;
    
    bool Is_Set();
    
    
};


class Light_Src{
private:
    bool uniform_locs_set;
    
    AMD::Vec3 m_pos;
    AMD::Vec3 m_target;
    AMD::Vec4 m_color;
    AMD::Vec3 m_dir;
    AMD::Mat4 m_MLP;
    float* const MLP_ptr = &m_MLP[0][0];
    
    float m_sat =1.0;
    
    
public:
    int dir_loc, clr_loc, sat_loc;
    
    Light_Src();
    ~Light_Src();
    void Set_Pos(AMD::Vec3 src);
    void Set_Pos(float theta, float phi);
    void Set_Color(AMD::Vec4 l_clr);
    void Set_Saturation(float);
    void Look_At(AMD::Vec3 pos);
    void Set_MLP(const Operator& op);
    void Set_Uniform_Locs(const Shader& sh);
    
    AMD::Vec3 Get_Pos() const;
    AMD::Vec3 Get_Target() const;
    
    AMD::Vec3& Get_Pos_vec();
    AMD::Vec3& Get_Target_vec();
    AMD::Vec4& Get_Color_vec();
    AMD::Vec3& Get_Direction_vec();
    float& Get_Saturation();
    
    float* Get_Dir_ptr();
    float* Get_Color_ptr();
    const float* const Get_Saturation_ptr() const;

    bool Is_Set();
    
    
};



#endif /* Operations_hpp */
