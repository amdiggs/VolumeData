//
//  shader.hpp
//  OpenGL
//
//  Created by Andrew Diggs on 7/28/22.
//

#ifndef shader_hpp
#define shader_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include "AMDmath.hpp"
class Texture;
class Texture3D;
class Light_Src;
class ShadowMap;

const int num_locs = 15;


class Shader{
private:
    unsigned int m_ID;
    int m_locs[num_locs];
    unsigned int CompileShader(unsigned int type, const std::string& source);
    void CreateShader(const std::string& vertexShader,const std::string& geometryShader, const std::string& fragmentShader);
    void CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    std::string get_sh_str(std::string sh_type, std::string sh_file);
    

    
public:
    Shader(std::string file_name);
    ~Shader();
    
    void bind() const;
    void unbind() const;
    int UniformLoc(const char* name) const;
    
    void Set_Uniform_MVP() const;
    void Set_Uniform_Normal() const;
    void Set_Uinform_LightSource(Light_Src& lsrc);
    void Set_Uniform_Mat4(const char* name, const float* mptr) const;
    void Set_Uniform_Mat3(const char* name, const float* mat_ptr) const;

    void Set_Uniform_Vec4(const char* name, const AMD::Vec4& vec) const;
    void Set_Uniform_Vec3(const char* name, const AMD::Vec3& vec) const;
    void Set_Uniform_Vec2(const char* name, const AMD::Vec2& vec) const;
    void Set_Uniform_Float(const char* name, const float& f) const;
    
    
    void Set_Texture(const char* name, const Texture& tex);
    void Set_Texture(const char* name, const Texture3D& tex);
    void Set_ShadowMap(const char* name, const ShadowMap& sm);
    
    
    
};


bool comment(std::string str, std::string reg_ex);
#endif /* shader_hpp */
