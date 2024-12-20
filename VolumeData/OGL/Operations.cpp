//
//  Operations.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 8/3/22.
//

#include "Operations.hpp"
#include "shader.hpp"

//====Model Operator Class=====================

Operator* op = Operator::Get_Inst();

Model::Model()
:m_pos(0.0, 0.0, 0.0), m_up(0.0, 1.0, 0.0), m_normal(0.0, 0.0, 1.0), m_theta(0.0), m_phi(0.0)
{}

Model::~Model(){}

AMD::Vec3& Model::Get_Up(){
    return m_up;
}

AMD::Vec3& Model::Get_Pos(){
    return m_pos;
}

AMD::Vec3& Model::Get_Normal(){
    return m_normal;
}

AMD::Mat4 Model::Get_Matrix() const{
    
    AMD::Vec3 N = m_normal;
    N.Normalize();
    
    AMD::Vec3 UpNorm = m_up;
    UpNorm.Normalize();
    
    AMD::Vec3 U = UpNorm.cross(N);
    U.Normalize();
    
    AMD::Vec3 V = N.cross(U);
    
    
    AMD::Mat4 camT;
    AMD::Mat4 camR;
    AMD::Vec3 cam_trans = m_pos * -1.0;
    camT.Translate(cam_trans);
    camR.assign_row(0, U);
    camR.assign_row(1, V);
    camR.assign_row(2, N);
    return camR * camT;
}


AMD::Mat3 Model::Get_Rotation_Matrix() const{
    
    AMD::Vec3 N = m_normal;
    N.Normalize();
    
    AMD::Vec3 UpNorm = m_up;
    UpNorm.Normalize();
    
    AMD::Vec3 U = UpNorm.cross(N);
    U.Normalize();
    
    AMD::Vec3 V = N.cross(U);
    
    
    
    AMD::Mat3 rot;
    rot.assign_row(0, U);
    rot.assign_row(1, V);
    rot.assign_row(2, N);
    
    return rot;
}



void Model::Set_Scale(float del){
    m_pos += (m_normal * del);
}


void Model::Move_LeftRight(float del){
    AMD::Vec3 left = m_up.cross(m_normal);
    m_pos += (left * del);
}

void Model::Move_UpDown(float del){
    m_pos += (m_up * del);
}

void Model::Move_To(AMD::Vec3 pos){
    m_pos = pos;
}


void Model::Rotate_Pole(float ang){
    if(m_theta == ang){return;}
    else{
        float del = ang - m_theta;
        AMD::Vec3 left = m_up.cross(m_normal);
        m_up.Rotate_Quaternion(left, del);
        m_normal.Rotate_Quaternion(left, del);
        m_theta += del;
    }
    m_normal.Normalize();
    m_up.Normalize();
}


void Model::Rotate_Azimuth(float ang){
    if(m_phi == ang){return;}
    else{
        float del = ang - m_phi;
        m_normal.Rotate_Quaternion(m_up, del);
        m_phi += del;
    }
    m_normal.Normalize();
}


void Model::Set_Normal(AMD::Vec3 Cam_Target){
    m_normal = Cam_Target;
}


//=====Camera Class=============

Camera::Camera()
:m_pos(0.0, 0.0, -2.0), m_up(0.0, 1.0, 0.0), m_target(0.0, 0.0, 1.0), m_theta(0.0), m_phi(0.0), m_zoom(1.0)
{}

Camera::~Camera(){}

AMD::Vec3& Camera::Get_Up(){
    return m_up;
}

AMD::Vec3& Camera::Get_Pos(){
    return m_pos;
}

AMD::Vec3& Camera::Get_Target(){
    return m_target;
}

AMD::Mat4 Camera::Get_Matrix() const{
    
    AMD::Vec3 N = m_target;
    N.Normalize();
    
    AMD::Vec3 UpNorm = m_up;
    UpNorm.Normalize();
    
    AMD::Vec3 U = UpNorm.cross(N);
    U.Normalize();
    
    AMD::Vec3 V = N.cross(U);
    
    
    AMD::Mat4 camT;
    AMD::Mat4 camR;
    AMD::Vec3 cam_trans = m_pos* -1.0;
    camT.Translate(cam_trans);
    camR.assign_row(0, U);
    camR.assign_row(1, V);
    camR.assign_row(2, N);
    
    return camR * camT;
}


void Camera::Move_ForwardBackward(float del){
    m_pos += (m_target * del);
}


void Camera::Move_LeftRight(float del){
    AMD::Vec3 left = m_up.cross(m_target);
    m_pos += (left * del);
}

void Camera::Move_UpDown(float del){
    m_pos += (m_up * del);
}

void Camera::Move_To(AMD::Vec3 pos){
    m_pos = pos;
}


void Camera::Look_UpDown(float ang){
    if(m_theta == ang){return;}
    else{
        float del = ang - m_theta;
        AMD::Vec3 left = m_up.cross(m_target);
        m_up.Rotate_Quaternion(left, del);
        m_target.Rotate_Quaternion(left, del);
        m_theta += del;
    }
    m_target.Normalize();
    m_up.Normalize();
}


void Camera::Look_LeftRight(float ang){
    if(m_phi == ang){return;}
    else{
        float del = ang - m_phi;
        m_target.Rotate_Quaternion(m_up, del);
        m_phi += del;
    }
    m_target.Normalize();
}


void Camera::Look_At(AMD::Vec3 p){
    //m_phi is relative to (0,0,1)
    // so my new phi should be the angle of V1 relative to (0,0,1)
    AMD::Vec3 V1 = AMD::Round(p - m_pos, 5);
    V1.Normalize();
    AMD::Vec3 z_hat(0.0,0.0,1.0);
    AMD::Vec3 V_proj_xz(V1.x,0.0,V1.z);
    float temp_phi = AMD::Get_angle(z_hat, V_proj_xz);
    if (V1.x >= 0.0){
        Look_LeftRight(temp_phi);
        
    }
    else{
        Look_LeftRight(6.28318 - temp_phi);
    }
    
    AMD::Vec3 y_hat(0.0,1.0, 0.0);
    AMD::Vec3 V_proj_y(0.0,V1.y,0.0);
    float temp_theta = AMD::Get_angle(y_hat, V1);
    float rel_angle = AMD::Round(temp_theta - 1.570796327, 5);
    Look_UpDown(rel_angle);
    
    
    
    
    
}


//=======Operator Class==================

Operator::Operator()
:Proj_vec(3.0, 3.0, 1.0, 100.0), uniform_locs_set(false), need_update(true)
{}



Operator::~Operator(){}

Operator Operator::inst;

Operator* Operator::Get_Inst(){
    return &inst;
}

void Operator::Set_W_Scale(int w, int h){
    w_scale = (float)h / (float)w;
}





void Operator::Projection(AMD::Vec4 vec){
    float near = vec.b;
    float far = vec.a;
    float alpha = 3.14159/4;
    float a = 1.0 / (tan(alpha));
    float b = a/w_scale; //1.0 / (tan(beta));
    
    
    Proj_mat[0][0] = a;
    Proj_mat[1][1] = b;
    Proj_mat[2][2] = (1.0*far) / (far - near);
    Proj_mat[2][3] = (-2.0*far*near) / (far - near);
    Proj_mat[3][2] = 1.0;
    Proj_mat[3][3] = 0.0;
    
    
    
}


void Operator::set_MV(){
    
    MV_mat = m_Cam.Get_Matrix() * m_model.Get_Matrix();
    MV_mat.Transpose();
}

void Operator::set_projection() {
    Projection(Proj_vec);
}


AMD::Vec4& Operator::Get_Proj_vec(){
    return this->Proj_vec;
}


void Operator::set_MVP(){
    
    MVP_mat = Proj_mat * m_Cam.Get_Matrix() * m_model.Get_Matrix();
    MVP_mat.Transpose();
    
}

void Operator::Set()
{
    set_projection();
    Normal_mat = m_model.Get_Rotation_Matrix();
    Normal_mat.Transpose();
    set_MVP();
}

AMD::Mat4 Operator::Get_MVP() const{
    return MVP_mat;
}

AMD::Mat4 Operator::Get_MV() const{
    return MV_mat;
}

AMD::Mat3 Operator::Get_Normal() const{
    return Normal_mat;
}


AMD::Mat4 Operator::Get_Model() const{
    return m_model.Get_Matrix();
}

AMD::Mat4 Operator::Get_Proj() const{
    return Proj_mat;
}
AMD::Mat4 Operator::Get_VP() const{
    AMD::Mat4 temp = Proj_mat * m_Cam.Get_Matrix();
    temp.Transpose();
    return temp;
}

AMD::Mat4 Operator::Get_MLP(const Light_Src& lsrc) const{
    AMD::Mat4 temp_M = m_model.Get_Matrix();
    AMD::Vec4 L_proj_vec(55.0,55.0, 1.0, 100.0);
    AMD::Mat4 temp_P = AMD::Projection(AMD::Orthographic, L_proj_vec);
    Camera temp_cam;
    temp_cam.Move_To(lsrc.Get_Pos());
    temp_cam.Look_At(lsrc.Get_Target());
    AMD::Mat4 temp = temp_P * temp_cam.Get_Matrix() * temp_M;
    temp.Transpose();
    return temp;
}


bool Operator::Is_Set(){
    return uniform_locs_set;
}


/*
 v1 + v' = v2; v' points v1->v2
 v' = v2 - v1; v' points v1->v2
 => v2 - v1 points v1->v2
 */




Light_Src::Light_Src()
:m_pos(0.0,50.0,0.0),m_color(0.15, 0.9, 1.0, 1.0), m_target(0.0,0.0,0.0), uniform_locs_set(false)
{}

Light_Src::~Light_Src(){}


void Light_Src::Set_Pos(AMD::Vec3 src) { 
    m_pos = src;
}

void Light_Src::Set_Pos(float theta, float phi) {
    float r = m_pos.len();
    m_pos.x = r*cos(phi)*sin(theta);
    m_pos.z = r*sin(phi)*sin(theta);
    m_pos.y = r*cos(theta);
}

void Light_Src::Set_Color(AMD::Vec4 l_clr) { 
    m_color = l_clr;
}

void Light_Src::Set_Saturation(float val) {
    m_sat = val;
}

AMD::Vec3 &Light_Src::Get_Pos_vec(){
    return this->m_pos;
}

AMD::Vec3 &Light_Src::Get_Target_vec(){
    return this->m_target;
}

AMD::Vec4 &Light_Src::Get_Color_vec(){

    return this->m_color;
}

AMD::Vec3 &Light_Src::Get_Direction_vec(){
    m_dir = m_target - m_pos;
    m_dir.Normalize();
    return this->m_dir;
}

AMD::Vec3 Light_Src::Get_Pos() const{
    return m_pos;
}

AMD::Vec3 Light_Src::Get_Target()const{
    return m_target;
}

float &Light_Src::Get_Saturation(){ 
    return this->m_sat;
}

void Light_Src::Set_Uniform_Locs(const Shader& sh){
    dir_loc = sh.UniformLoc("l_src.dir");
    clr_loc = sh.UniformLoc("l_src.clr");
    sat_loc = sh.UniformLoc("l_src.sat");
    uniform_locs_set = true;
    
}

float* Light_Src::Get_Dir_ptr(){
    return m_pos.get();
}

float* Light_Src::Get_Color_ptr(){
    return m_color.get();
}

const float* const Light_Src::Get_Saturation_ptr() const{
    return &m_sat;
}


void Light_Src::Look_At(AMD::Vec3 pos){
    m_target = pos;
}

void Light_Src::Set_MLP(const Operator& op){
    AMD::Mat4 temp_M = op.Get_Model();
    AMD::Mat4 temp_P = op.Get_Proj();
    Camera temp_cam;
    temp_cam.Move_To(m_pos);
    temp_cam.Look_At(m_target);
    m_MLP = temp_P * temp_cam.Get_Matrix() * temp_M;
    m_MLP.Transpose();
}


bool Light_Src::Is_Set(){
    return uniform_locs_set;
}






