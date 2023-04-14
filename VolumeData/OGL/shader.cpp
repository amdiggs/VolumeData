//
//  shader.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 7/28/22.
//

#include "shader.hpp"
#include "Texture.hpp"
#include "Shapes.hpp"
#include "Operations.hpp"
#include "FrameBuffer.hpp"

extern Operator* op;


Shader::Shader(std::string file_name){
    std::string vertexShader = get_sh_str("VERTEX", file_name);
    std::string geometryShader = get_sh_str("GEOMETRY",file_name);
    std::string fragmentShader = get_sh_str("FRAGMENT", file_name);
    
    if(!geometryShader.empty()){
        CreateShader(vertexShader, geometryShader, fragmentShader);
    }
    else{CreateShader(vertexShader, fragmentShader);}

}


Shader::~Shader(){
    glDeleteProgram(m_ID);
}



void Shader::bind() const{
    glUseProgram(m_ID);
}

void Shader::unbind() const{
    glUseProgram(0);
}



std::string Shader::get_sh_str(std::string sh_type, std::string sh_file){
    std::ifstream in_file;
    std::string shader_string;
    in_file.open(sh_file);
    if (!in_file.is_open()){
        std::cout << "ERROR Loading Shader file!!" << std::endl;
    }
    std::string line;
    while (getline(in_file, line)) {
        if (comment(line, "#SHADER " + sh_type)){
            while (getline(in_file, line)){
                if (!line.empty()){
                    if (comment(line, "#END")) {
                        break;
                    }
                    else{shader_string.append(line + "\n");}
                }
            }
        }
    }
    return shader_string;
}




unsigned int Shader::CompileShader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << message << std::endl;
    }
    
    return id;
    
    
}
void Shader::CreateShader(const std::string& vertexShader,const std::string& geometryShader, const std::string& fragmentShader){
    m_ID = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    glAttachShader(m_ID, vs);
    glAttachShader(m_ID, gs);
    glAttachShader(m_ID, fs);
    glLinkProgram(m_ID);
    int link_result;
    glGetShaderiv(m_ID, GL_LINK_STATUS, &link_result);
    if (link_result == GL_FALSE)
    {
        int length;
        glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(m_ID, length, &length, message);
        std::cout << message << std::endl;
    }
    
    glValidateProgram(m_ID);
    
    glDeleteShader(vs);
    glDeleteShader(gs);
    glDeleteShader(fs);
    
    return;
    
    
}

void Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
    m_ID = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    glAttachShader(m_ID, vs);
    glAttachShader(m_ID, fs);
    glLinkProgram(m_ID);
    glValidateProgram(m_ID);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return;
    
    
}

int Shader::UniformLoc(const char* name) const{
    int loc = glGetUniformLocation(m_ID, name);
    if(loc == -1){
        std::cout << "Uniform " << name << " not found" << std::endl;
        exit(2);
    }
    else{return loc;}
}



void Shader::Set_Uniform_MVP() const{
    bind();
    Set_Uniform_Mat4("u_MVP", op->MVP_ptr);
    
    
}

void Shader::Set_Uniform_Normal() const{
    bind();
    Set_Uniform_Mat3("u_Normal", op->Norm_ptr);
}


void Shader::Set_Uinform_LightSource(Light_Src& lsrc){
    Set_Uniform_Vec3("l_src.dir", lsrc.Get_Direction_vec());
    Set_Uniform_Vec4("l_src.clr", lsrc.Get_Color_vec());
    Set_Uniform_Float("l_src.sat", lsrc.Get_Saturation());
}


void Shader::Set_Uniform_Mat4(const char* name, const float* mptr) const{
    int loc = UniformLoc(name);
    glUniformMatrix4fv(loc,1,GL_FALSE, mptr);
}

void Shader::Set_Uniform_Mat3(const char* name, const float* mat_ptr) const {
    int loc = UniformLoc(name);
    glUniformMatrix3fv(loc,1,GL_FALSE, mat_ptr);
}


void Shader::Set_Uniform_Vec2(const char* name, const AMD::Vec2& vec) const {
    int loc = UniformLoc(name);
    glUniform2fv(loc, 1,(float*) &vec);
}


void Shader::Set_Uniform_Vec3(const char* name, const AMD::Vec3& vec) const {
    int loc = UniformLoc(name);
    glUniform3fv(loc, 1,(float*) &vec);
}


void Shader::Set_Uniform_Vec4(const char* name, const AMD::Vec4& vec) const {
    int loc = UniformLoc(name);
    glUniform4fv(loc,1,(float*) &vec);
}


void Shader::Set_Uniform_Float(const char* name, const float& f) const {
    int loc = UniformLoc(name);
    glUniform1f(loc,f);
}



void Shader::Set_Texture(const char* name, const Texture& text){
    int loc = UniformLoc(name);
    bind();
    text.Bind();
    glUniform1i(loc, text.Get_Layer());
    
}


void Shader::Set_Texture(const char* name, const Texture3D& text){
    int loc = UniformLoc(name);
    bind();
    text.Bind();
    glUniform1i(loc, text.Get_Layer());
    
}


void Shader::Set_ShadowMap(const char* name, const ShadowMap& sm){
    int loc = UniformLoc(name);
    bind();
    sm.ReadBind();
    glUniform1i(loc, sm.Get_Layer());
}


bool comment(std::string str, std::string reg_ex){
    std::regex reg(reg_ex);
    std::smatch m;
    std::regex_search(str, m, reg);
    return !m.empty();
};








