//
//  My_Types.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 9/18/22.
//

#include "AMDmath.hpp"
using namespace AMD;



int AMD::Min(int a, int b){
    return (a<b) ? a : b;
}

float AMD::Min(float a, float b){
    return (a<b) ? a : b;
}

int AMD::Max(int a, int b){
    return (a>b) ? a : b;
}

float AMD::Max(float a, float b){
    return (a>b) ? a : b;
}


float AMD::AMU_to_G(float u){
    return u/U_2_G;
}



//############Ivec3########################################

Ivec3::Ivec3(int _d, int _r, int _c)
:d(_d), r(_r), c(_c)
{}

Ivec3::Ivec3()
:d(0), r(0), c(0)
{}

Ivec3::~Ivec3()
{}



int &Ivec3::operator[](const int index) {
    return get()[index];
}

int *Ivec3::get() {
    return &d;
}


AMD::Ivec3 &Ivec3::operator=(const AMD::Ivec3 &other) {
    if (this == &other) {
        return *this;
    }
    
    this->d = other.d;
    this->r = other.r;
    this->c = other.c;
    return *this;
}


//#############Float Vec[n] #################################
Vec2::Vec2()
:x(0.0), y(0.0)
{}

Vec2::Vec2(float x, float y)
:x(x), y(y)
{}
Vec2::~Vec2() {}

AMD::Vec2 Vec2::add(const AMD::Vec2 &other) const {
    return Vec2(x + other.x, y+other.y);
}

float *Vec2::get() {
    return &x;
}

float &Vec2::operator[](const int index) {
    return get()[index];
}


float Vec2::dot(const AMD::Vec2 &other) const {
    return x*other.x + y*other.y;
}

float Vec2::len() const {
    return sqrt(this->dot(*this));
}

AMD::Vec2 Vec2::operator*(float scale) {
    float _x = this->x*scale;
    float _y = this->y*scale;
    
    return Vec2(_x,_y);
}

AMD::Vec2 Vec2::operator+(const AMD::Vec2 &other) const {
    return add(other);
}

AMD::Vec2 Vec2::operator-(const AMD::Vec2 &other) const {
    float _x = this->x - other.x;
    float _y = this->y - other.y;
    
    return Vec2(_x,_y);
}

AMD::Vec2 &Vec2::operator=(const AMD::Vec2 &other) {
    if (this == &other) {
        return *this;
    }
    
    this->x = other.x;
    this->y = other.y;
    return *this;
}


AMD::Vec2 Vec2::operator/(float div) {
    return Vec2(this->x/div, this->y/div);
}

AMD::Vec2 Vec2::operator+=(const AMD::Vec2 &other) {
        this->x = this->x+ other.x;
        this->y = this->y + other.y;
        
        return *this;
}

AMD::Vec2 Vec2::operator*=(float scale) {
        this->x = this->x*scale;
        this->y = this->y*scale;
        
        return *this;
}

void Vec2::print() {
    std::cout << this->x << " " << this->y << std::endl;
}


Vec3::Vec3()
:x(0.0), y(0.0), z(0.0)
{}


Vec3::Vec3(float s)
:x(s) , y(s) , z(s)
{}

Vec3::Vec3(float e_x, float e_y, float e_z)
:x(e_x), y(e_y),z(e_z)
{}


float* Vec3::get(){
    return &x;
}


float& Vec3::operator[](const int index){
    return get()[index];
}

float Vec3::dot(const AMD::Vec3 &other) const {
    return x*other.x + y*other.y + z*other.z;
}

AMD::Vec3 Vec3::cross(const AMD::Vec3 &other) const {
    float _x = y*other.z - z*other.y;
    float _y = z*other.x - x*other.z;
    float _z = x*other.y - y*other.x;
    return Vec3(_x, _y, _z);
}

float Vec3::len() const{
    return sqrt(this->dot(*(this)));
}

AMD::Vec3 &Vec3::operator=(const AMD::Vec3 &other) {
    if (&other == this){
        return  *this;
    }
    else {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
    }
    return *this;
}




AMD::Vec3 &Vec3::operator=(const AMD::Quat &other) {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
    return *this;
}


AMD::Vec3 Vec3::operator*(const AMD::Vec3 &other) const{
    return Vec3(x*other.x,y*other.y, z*other.z);
}



AMD::Vec3 Vec3::operator/(float div) const{
    return Vec3(x/div,y/div,z/div);
}


AMD::Vec3 Vec3::operator-(const Vec3& other) const{
    float _x = this->x - other.x;
    float _y = this->y - other.y;
    float _z = this->z - other.z;
    return Vec3(_x,_y,_z);
    
}




AMD::Vec3 Vec3::operator+(const Vec3& other) const{
    float _x = this->x + other.x;
    float _y = this->y + other.y;
    float _z = this->z + other.z;
    return Vec3(_x,_y,_z);
    
}




AMD::Vec3 Vec3::operator+=(const Vec3& other){
    this->x = this->x + other.x;
    this->y = this->y + other.y;
    this->z = this->z + other.z;
    
    return *this;
}

AMD::Vec3 Vec3::operator*=(float scale){
    *this = *this * scale;
    return *this;
    
}

bool Vec3::operator==(const Vec3& other) const{
    return (this->x == other.x && this->y == other.y && this->z == other.z);
}


bool Vec3::Is_Parallel(const Vec3& other) const{
    Vec3 rev = other*(-1.0);
    return (*this == other || *this == rev);
}

void AMD::Vec3::print(){
    std::cout << x << " " << y << " " << z << std::endl;
}


void AMD::Vec3::Reset(){
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

void AMD::Vec3::Normalize(){
    float N = this->len();
    *this = (*this)/N;
}



void AMD::Vec3::Rotate_Quaternion(const Vec3& axis, float ang){
    AMD::Quat q(ang, axis);
    AMD::Quat qi = q.inverse();
    AMD::Quat q_this(*this);
    AMD::Quat temp = q*q_this*qi;
    *this = temp;
    
}


void AMD::Vec3::Vround(int decimals){
    x = Round(x, decimals);
    y = Round(y, decimals);
    z = Round(z, decimals);
}

AMD::Vec3 AMD::operator*(const float scale, const Vec3& other){
    return Vec3(other.x*scale,other.y*scale, other.z*scale);
}

AMD::Vec3 AMD::operator*(const Vec3& other, const float scale){
    return Vec3(other.x*scale,other.y*scale, other.z*scale);
}

//========================================================================================================

Vec4::Vec4()
:r(0.0) , g(0.0) , b(0.0) , a(0.0)
{}


Vec4::Vec4(float s)
:r(s) , g(s) , b(s) , a(s)
{}

Vec4::Vec4(float e_r, float e_g, float e_b, float e_a)
:r(e_r) , g(e_g) , b(e_b) , a(e_a)
{}

Vec4::Vec4(float* e_vec)
:r(e_vec[0]), g(e_vec[1]), b(e_vec[2]), a(e_vec[3]){}


Vec4::Vec4(AMD::Vec3& vec)
:r(vec[0]), g(vec[1]), b(vec[2]), a(1.0){}


float* Vec4::get(){
    return &r;
}


float& Vec4::operator[](const int& index){
    return get()[index];
}

AMD::Vec4 Vec4::operator+(const Vec4& other) const{
    
    float _x = this->r + other.r;
    float _y = this->g + other.g;
    float _z = this->b + other.b;
    float _w = this->a + other.a;
    return Vec4(_x,_y,_z, _w);
    }

void AMD::Vec4::Reset(){
    r = 0.0;
    g = 0.0;
    b = 0.0;
    a = 0.0;
}




//=========Quaternions==================================

Quat::Quat()
:w(0.0), x(0.0), y(0.0), z(0.0)
{}

Quat::Quat(float ew, float ex, float ey, float ez)
:w(ew), x(ex), y(ey), z(ez)
{}

Quat::Quat(Vec3 coords)
:w(0.0), x(coords.x), y(coords.y), z(coords.z)
{}

Quat::Quat(float ang, Vec3 coords)
:w(cos(ang/2.0)), x(sin(ang/2.0)*coords.x), y(sin(ang/2.0)*coords.y), z(sin(ang/2.0)* coords.z)
{}

Quat::~Quat(){}

float* Quat::get(){
    return &w;
}


float& Quat::operator[](const int& index){
    return get()[index];
}

AMD::Quat Quat::inverse(){
    return Quat(w, -x, -y, -z);
}

AMD::Quat& Quat::operator=(const Quat& other){
    if (&other == this){
        return  *this;
    }
    else {
        this->w = other.w;
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
    }
    return *this;
}

AMD::Quat AMD::operator*(const Quat& L, const Quat& R){
    float _w = L.w*R.w - L.x*R.x - L.y*R.y - L.z*R.z;
    float _x = L.w*R.x + L.x*R.w + L.y*R.z - L.z*R.y;
    float _y = L.w*R.y + L.y*R.w + L.z*R.x - L.x*R.z;
    float _z = L.w*R.z + L.z*R.w + L.x*R.y - L.y*R.x;
    return Quat(_w, _x, _y, _z);
}




// #################VOXEL CLASS#############################
Voxel::Voxel()
:d(0),r(0),c(0), value(0.0), num_Si(0.), num_Ox(0.), in_set(false)
{}

Voxel::Voxel(int _d, int _r, int _c)
:d(_d),r(_r),c(_c), value(0.0), num_Si(0.), num_Ox(0.), in_set(false)
{}

Voxel::Voxel(AMD::Vec3 c)
:d(0),r(0),c(0), value(0.0), num_Si(0), num_Ox(0), in_set(false), m_center(c)
{}


AMD::Voxel &Voxel::operator=(const AMD::Voxel &other) {
    if (this == &other) {
        return *this;
    }
    
    this->d = other.d;
    this->r = other.r;
    this->c = other.c;
    this->value = other.value;
    this->num_Si = other.num_Si;
    this->num_Ox = other.num_Ox;
    this->in_set = other.in_set;
    this->m_center = other.m_center;
    return *this;
}

Voxel::~Voxel() {}

int &Voxel::operator[](const int index) {
    return get()[index];
}

int *Voxel::get() {
    return &d;
}


bool Voxel::Is_Equal(const int a, const int b, const int c){
    int tmp = (int)(this->d == a) + (int)(this->r == b) + (int)(this->c == c);
    return (tmp == 3);
}

bool Voxel::operator==(const Voxel& other){

    return (this->d == other.d && this->r == other.r  && this->c == other.c);
}


void Voxel::Clear(){
    value = 0.;
    in_set = false;
}

void Voxel::Set_Center(){
    m_center.x = (c + 0.5)*lengths.x;
    m_center.y = (r + 0.5)*lengths.y;
    m_center.z = (d + 0.5)*lengths.z;
}

AMD::Vec3 Voxel::Get_Center(){
    
    return m_center;
}

float Voxel::Get_Vol(){
    return lengths.x*lengths.y*lengths.z;
}


AMD::Vec3 Voxel::lengths;











//==============MATRICIES==========================================

Mat3::Mat3() {
    for (int i = 0; i< 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == j){
                m[i][j] = 1.0;
            }
            else{m[i][j] = 0.0;}
        }
    }
}

float *Mat3::operator[](const int &index) {
    return m[index];
}

AMD::Mat3 &Mat3::operator=(const AMD::Mat3 &other) {
    if(this == &other){ return  *this;}
    else{
        for (int i = 0; i< 3; i++) {
            for (int j = 0; j < 3; j++) {
            this->m[i][j] = other.m[i][j];
            }

        }
    
        return *this;
    }
}

AMD::Mat3 Mat3::add(const AMD::Mat3 &other) const {
    Mat3 temp;
    for (int i =0; i<n; i++){
        for (int j = 0; j < n; j++) {
            temp.m[i][j] = m[i][j] + other.m[i][j];
        }
    }
    return temp;
}

AMD::Mat3 Mat3::operator+(const AMD::Mat3 &other) const {
    return this->add(other);
}

AMD::Mat3 Mat3::multiply(const AMD::Mat3 &other) const {
    Mat3 temp;
    float el;
    for (int col = 0; col<n; col++){
        for (int i =0; i<n; i++){
            el =0.0;
            for (int j = 0; j < n; j++) {
                el += other.m[i][j] * m[j][col];
            }
            temp.m[i][col] = el;
        }
    }
    return temp;
}

AMD::Mat3 Mat3::operator*(const AMD::Mat3 &other) const {
    return this->multiply(other);
}

AMD::Vec3 Mat3::multiply(AMD::Vec3 &other) const {
    Vec3 temp;
    for (int row = 0; row<n; row++){
        for (int col = 0; col<n;col++){
            temp[row] += m[col][row]*other[col];
        }
    }
    return temp;
}

AMD::Vec3 Mat3::operator*(AMD::Vec3 &other) const {
    return this->multiply(other);
}

void Mat3::assign_col(int col_idx, AMD::Vec3 col) {
    for (int i = 0; i<n; i++){
        m[i][col_idx] = col[i];
    }
}

void Mat3::assign_row(int row_idx, AMD::Vec3 row) {
    for (int i = 0; i<n; i++){
        m[row_idx][i] = row[i];
    }
}

void Mat3::Rotate(AMD::Vec3 ang) {
    float a = ang.x; float b = ang.y; float c = ang.z;
    AMD::Mat3 rot;
    AMD::Vec3 c0 (cos(a)*cos(b),  cos(a)*sin(b)*sin(c) - sin(a)*cos(c),  cos(a)*sin(b)*cos(c)+sin(a)*sin(c));
    AMD::Vec3 c1(sin(a)*cos(b), sin(a)*sin(b)*sin(c) + cos(a)*cos(c), sin(a)*sin(b)*cos(c)-cos(a)*sin(c));
    AMD::Vec3 c2(-sin(b), cos(b)*sin(c), cos(b)*cos(c));
    
    rot.assign_col(0, c0);
    rot.assign_col(1, c1);
    rot.assign_col(2, c2);
    *this = rot * (*this);
    
    return;
}

void Mat3::Scale(float scale) {
    Mat3 temp;
    temp[0][0] = scale;
    temp[1][1] = scale;
    temp[2][2] = scale;
    
    *this = temp * (*this);
    
    return;
}

void Mat3::Scale(AMD::Vec3 vec) {
    
    m[0][0] = m[0][0] * vec.x;
    m[1][1] = m[1][1] * vec.y;
    m[2][2] = m[2][2] * vec.z;
    
    
    return;
}

void Mat3::Transpose(){
    Mat3 temp = *this;
    
    for (int i = 0; i< n; i++) {
        for (int j = 0; j < n; j++) {
                m[i][j] = temp.m[j][i];
        }
    }
}

void Mat3::print() {
    std::cout <<  std::endl;
    for (int i =0; i<n; i++){
        for (int j = 0; j < n; j++) {
        std::cout << m[i][j] << ", ";
        }
       std::cout <<  std::endl;
    
    }
    std::cout <<  std::endl;
}


//================================================
Mat4::Mat4(){
    for (int i = 0; i< 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j){
                m[i][j] = 1.0;
            }
            else{m[i][j] = 0.0;}
        }
    }
}

AMD::Mat4 Mat4::add(const AMD::Mat4 &other) const {
    Mat4 temp;
    for (int i =0; i<n; i++){
        for (int j = 0; j < n; j++) {
            temp.m[i][j] = m[i][j] + other.m[i][j];
        }
    }
    return temp;
}

AMD::Mat4 Mat4::operator+(const AMD::Mat4 &other) const {
    return add(other);
}

AMD::Mat4 Mat4::multiply(const AMD::Mat4 &other) const {
    Mat4 temp;
    for (int col = 0; col<4; col++){
        AMD::Vec4 Oth_Col = other.Get_Col(col);
        AMD::Vec4 Temp_Col = multiply(Oth_Col);
        temp.assign_col(col, Temp_Col);
    }
    return temp;
}


AMD::Vec4 Mat4::multiply(AMD::Vec4 &other) const {
    Vec4 temp;
    for (int col = 0; col<4; col++){
        for (int i = 0; i<4;i++){
            temp[col] += m[col][i]*other[i];
        }
    }
    return temp;
}

AMD::Mat4 Mat4::operator*(const AMD::Mat4 &other) const{
    return multiply(other);
}


AMD::Vec4 Mat4::operator*(AMD::Vec4& other) const {
    
    return multiply(other);
    
}


AMD::Vec4 Mat4::Get_Col(int col) const {
    return AMD::Vec4(m[0][col], m[1][col], m[2][col], m[3][col]);
}

AMD::Vec3 Mat4::operator*(Vec3& other) const{
    Vec4 temp(other.x, other.y, other.z, 1.0);
    temp = multiply(temp);
    return Vec3(temp.r, temp.g, temp.b);
    
    
    
}

float* Mat4::operator[](const int &index){
    
    return m[index];
}

void Mat4::print(){
    std::cout <<  std::endl;
    for (int i =0; i<4; i++){
        for (int j = 0; j < 4; j++) {
        std::cout << m[i][j] << ", ";
        }
       std::cout <<  std::endl;
    
    }
    std::cout <<  std::endl;
}

AMD::Mat4& Mat4::operator=(const AMD::Mat4 &other) {
    if (&other == this){
        return  *this;
    }
    
    else{
        for (int i =0; i<4; i++){
            for (int j = 0; j < 4; j++) {
                 this->m[i][j] = other.m[i][j];
            }
        }
        return *this;
    }
}

void Mat4::assign_col(int col_idx, Vec4 col){
    
    for (int i = 0; i<4; i++){
        m[i][col_idx] = col[i];
    }
    
}

void Mat4::assign_row(int row_idx, Vec4 row){
    
    for (int i = 0; i<4; i++){
        m[row_idx][i] = row[i];
    }
    
}



void Mat4::assign_col(int col_idx, Vec3 col){
    
    for (int i = 0; i<3; i++){
        m[i][col_idx] = col[i];
    }
    
}

void Mat4::assign_row(int row_idx, Vec3 row){
    
    for (int i = 0; i<3; i++){
        m[row_idx][i] = row[i];
    }
    
}


void Mat4::Rotate(Vec3 ang){
    float a = ang.x; float b = ang.y; float c = ang.z;
    AMD::Mat4 rot;
    AMD::Vec4 c0 (cos(a)*cos(b),  cos(a)*sin(b)*sin(c) - sin(a)*cos(c),  cos(a)*sin(b)*cos(c)+sin(a)*sin(c), 0.0);
    AMD::Vec4 c1(sin(a)*cos(b), sin(a)*sin(b)*sin(c) + cos(a)*cos(c), sin(a)*sin(b)*cos(c)-cos(a)*sin(c), 0.0);
    AMD::Vec4 c2(-sin(b), cos(b)*sin(c), cos(b)*cos(c), 0.0);
    AMD::Vec4 c3(0.0, 0.0, 0.0, 1.0);
    
    rot.assign_row(0, c0);
    rot.assign_row(1, c1);
    rot.assign_row(2, c2);
    rot.assign_row(3, c3);
    *this = rot * (*this);
    
    return;
}



void Mat4::Scale(AMD::Vec3 vec) {
    Mat4 temp;
    temp.m[0][0] = vec.x;
    temp.m[1][1] = vec.y;
    temp.m[2][2] = vec.z;
    *this = temp * (*this);
    return;
}

void Mat4::Translate(AMD::Vec3 vec) {
    Mat4 temp;
    Vec4 temp_vec(vec.x,vec.y,vec.z,1.0);
    temp.assign_col(3, temp_vec);
    *this = temp * (*this);
    return;
}

void Mat4::Transpose(){
    Mat4 temp = *this;
    
    for (int i = 0; i< 4; i++) {
        for (int j = 0; j < 4; j++) {
                m[i][j] = temp.m[j][i];
        }
    }
}


void Mat4::Reset(){
    for (int i = 0; i< 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j){
                m[i][j] = 1.0;
            }
            else{m[i][j] = 0.0;}
        }
    }
    
}


Basis2::Basis2(const Vec3& norm) {
    AMD::Vec3 X_hat(1.,0.,0.);
    AMD::Vec3 Y_hat(0.,1.,0.);
    AMD::Vec3 Z_hat(0.,0.,1.);
    if(norm.Is_Parallel(X_hat)){
        e1 = Y_hat;
        e2 = Z_hat;
    }
    else if(norm.Is_Parallel(Y_hat)){
        e1 = X_hat;
        e2 = Z_hat;
    }
    else if(norm.Is_Parallel(Z_hat)){
        e1 = X_hat;
        e2 = Y_hat;
        
    }
    
    else{
        e1 = norm;
        e2 = norm;
    }
}




AMD::Mat4 ID(){
    return Mat4();
}


AMD::Mat4 AMD::ROTATION_MATRIX(Vec3 ang){
    
    float a = ang.x; float b = ang.y; float c = ang.z;
    AMD::Mat4 rot;
    AMD::Vec4 r0 (cos(a)*cos(b),  cos(a)*sin(b)*sin(c) - sin(a)*cos(c),  cos(a)*sin(b)*cos(c)+sin(a)*sin(c), 0.0);
    AMD::Vec4 r1(sin(a)*cos(b), sin(a)*sin(b)*sin(c) + cos(a)*cos(c), sin(a)*sin(b)*cos(c)-cos(a)*sin(c), 0.0);
    AMD::Vec4 r2(-sin(b), cos(b)*sin(c), cos(b)*cos(c), 0.0);
    AMD::Vec4 r3(0.0, 0.0, 0.0, 1.0);
    
    rot.assign_row(0, r0);
    rot.assign_row(1, r1);
    rot.assign_row(2, r2);
    rot.assign_row(3, r3);
    
    
    return rot;
}


float AMD::Round(float val, int num_decimal){
    float mod = 10.0;
    for( int i = 0; i<num_decimal; i++){
        mod*=10.0;
    }
    int temp = (int)(val*mod);
    return (float)temp / mod;
}

float AMD::Get_angle(const Vec3& A, const Vec3& B){
    if(A.len() <= 0.00001 || B.len() <= 0.00001){
        return 1.57079;
    }
    else{
        float c_th = A.dot(B)/(A.len()*B.len());
        return acos(c_th);
    }
}



float AMD::Distance(const Vec3& A, const Vec3& B){
    AMD::Vec3 temp = A - B;
    return temp.len();
}

AMD::Vec3 AMD::Round(const Vec3& vec, int decimals){
    return AMD::Vec3(Round(vec.x, decimals),Round(vec.y, decimals),Round(vec.z, decimals));
}



void AMD::Compute_norms(Vertex_TX* verts,unsigned int* ints, int num){
    Vec3 A, B, _norm;
    int a,b,c;
    for (int i = 0; i< num; i+=3){
        a = ints[i]; b = ints[i+1]; c = ints[i+2];
        A = verts[b].pos - verts[a].pos;
        B = verts[c].pos - verts[a].pos;
        _norm = A.cross(B);
        
        verts[a].norm+=_norm;
        verts[b].norm+=_norm;
        verts[c].norm+=_norm;
        
        verts[a].norm *= 1.0 / verts[a].norm.len();
        verts[b].norm *= 1.0 / verts[b].norm.len();
        verts[c].norm *= 1.0 / verts[c].norm.len();
        
        
    }
}



void AMD::Map_Texture_Coords(Vertex_TX* verts, int num_verts){
    float MAX_X = 0.0; float MAX_Y = 0.0;
    for (int i = 0; i< num_verts; i++){
        if (verts[i].texture.x > MAX_X) {
            MAX_X = verts[i].texture.x;
            }
        
        if (verts[i].texture.y > MAX_Y) {
            MAX_Y = verts[i].texture.y;
            }
    }
    
    for (int j = 0; j<num_verts; j++){
        verts[j].texture.x*= 1.0/MAX_X;
        verts[j].texture.y*= 1.0/MAX_Y;
    }
    
    
}



AMD::Vertex_Basic& Vertex_Basic::operator=(const Vertex_TX& other){
    this->pos = other.pos;
    this->clr = other.clr;
    this->norm = other.norm;
    return *this;
}


AMD::Vertex_TX& Vertex_TX::operator=(const Vertex_TX& other){
    if (this == &other) {
        return *this;
    }
    
    this->pos = other.pos;
    this->clr = other.clr;
    this->norm = other.norm;
    this->texture = other.texture;
    this->tex_indx = other.tex_indx;
    
    return  *this;
    
}


AMD::Vertex_TX Vertex_TX::off_set(Vec3 vec){
    this->pos = this->pos + vec;
    return *this;
}


AMD::Mat4 AMD::Perspective_Matrix(Vec4 vec){
    Mat4 Proj_mat;
    float near = vec.b;
    float far = vec.a;
    float alpha = 3.14159/4;
    float a = 1.0 / (tan(alpha));
    float b = a*(16.0/9.0);
    
    
    Proj_mat[0][0] = a;
    Proj_mat[1][1] = b;
    Proj_mat[2][2] = (1.0*far) / (far - near);
    Proj_mat[2][3] = (-2.0*far*near) / (far - near);
    Proj_mat[3][2] = 1.0;
    Proj_mat[3][3] = 0.0;
    
    return Proj_mat;
}


AMD::Mat4 AMD::Orthographic_Matrix(Vec4 vec){
    Mat4 Proj_mat;
    
    float left = vec.r;
    float top = vec.g*(9.0/16.0);
    float near = vec.b;
    float far = vec.a;
    
    
    Proj_mat[0][0] = 1.0/ left;
    Proj_mat[1][1] = 1.0/ top;
    Proj_mat[2][2] = 2.0 / (far - near);
    Proj_mat[2][3] = (-1.0*(far+near)) / (far - near);
    Proj_mat[3][2] = 0.0;
    Proj_mat[3][3] = 1.0;
    
    return Proj_mat;
}


AMD::Mat4 AMD::Projection(Projection_Type typ, AMD::Vec4 vec){
    switch (typ) {
        case Perspective:
            return Perspective_Matrix(vec);
            break;
            
        case Orthographic:
            return Orthographic_Matrix(vec);
            break;
            
        default:
            return AMD::Mat4();
    }
    
}






Timer::Timer(const char* proc)
:proc_name(proc)
{
    start = std::chrono::steady_clock::now();
}

Timer::~Timer()
{
    end = std::chrono::steady_clock::now();
    std::chrono::duration<double> length = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    std::cout << proc_name << " took " << 1000*length.count() << " ms.\n";
}




