//
//  Texture.hpp
//  OpenGL
//
//  Created by Andrew Diggs on 10/2/22.
//

#ifndef Texture_hpp
#define Texture_hpp
#define IW 128



#include <stdio.h>
#include "Render.hpp"
enum Texture_Type{Object_Color_Texture,Object_Normal_Map, Environment_Color, Shadow_Map_Texture, Render_Texture};

class Atom;
class Shader;
class Texture{
private:
    
    GLenum m_type = GL_TEXTURE_2D;
    void Gen_Tex_2D(void* dat);
    int m_w, m_h, m_bpp;
    unsigned int m_ID;
    int m_layer;
    
    
public:
    Texture(int layer);
    
    ~Texture();
    
    void Bind() const;
    void UnBind() const;
    void Load(std::string image);
    void Gen_Cube_Map(std::string image);
    
    int Get_Layer() const;
    int Get_ID() const;
    
};


class Texture3D{
private:
    
    GLenum m_type = GL_TEXTURE_3D;
    void Gen_Tex_3D(void* dat);
    int m_w, m_h,m_z, m_bpp;
    unsigned int m_ID;
    int m_layer;
    
    
public:
    Texture3D(int layer);
    
    ~Texture3D();
    
    void Bind() const;
    void UnBind() const;
    void Load(std::string image);
    
    
    void Simple_Test();
    void Cavity();
    void Cavity(AMD::Vec3* vecs, int num_vecs, AMD::Vec3 box_bounds);
    int Get_Layer() const;
    int Get_ID() const;
    
};


struct Character {
    unsigned int m_id;
    AMD::Vec2 Size;
    AMD::Vec2 Bearing;
    unsigned int Advance;
    Character();
    Character(unsigned int tex_id, AMD::Vec2 s, AMD::Vec2 B, unsigned int advance);
};

void Gen_Char_Map();

#endif /* Texture_hpp */

/*
class Rand{
public:
    Rand(const Rand&) = delete;
    
    static Rand& Get();
    double r_u_d(double, double);
    int r_uni_int(int, int);
    int rand_color();
    double rand_normal_color();
    double return_rate();
    double escape_rate();
    double get_time(double);
    
    void set_temp(float temp);
private:
    Rand() {}
    float m_temp;
    

};

float Letter_Check(int i, int j, char let);
void X_hat(GLubyte (*dat)[128][4]);
void Y_hat(GLubyte (*dat)[128][4]);
void Z_hat(GLubyte (*dat)[128][4]);
void bind_multi(int layer);
void axis_texture(int layer,const Shader& sh);
void Off_set(Atom* ats,const Shader& sh);
void Gen_Gaussian(int layer,const Shader& sh);
*/

