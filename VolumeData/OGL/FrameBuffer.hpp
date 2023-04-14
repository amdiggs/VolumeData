//
//  FrameBuffer.hpp
//  OpenGL
//
//  Created by Andrew Diggs on 2/8/23.
//

#ifndef FrameBuffer_hpp
#define FrameBuffer_hpp

#include <stdio.h>
#include "shader.hpp"
#include "AMDmath.hpp"

class Quad_Mesh;                                 
class Light_Src;

struct PixelInfo{
    unsigned int ObjectID;
    unsigned int DrawID;
    unsigned int PrimID;
};

class RenderTexture {
private:
    unsigned int m_w, m_h;
    const int m_layer = 9;
    
public:
    const std::string shader_file = "/Users/diggs/Desktop/OpenGL/OpenGL/Shaders/picking.fs";
    Shader m_sh;
    unsigned int m_fbo;
    unsigned int m_tex;
    unsigned int m_rbo;
    unsigned int m_depth;
    
    RenderTexture();
    ~RenderTexture();
    
    void Init(unsigned int w, unsigned int h);
    void Bind();
    void UnBind();
    void Pass();
    void Draw(Quad_Mesh& qd);
    
    PixelInfo ReadPixel(unsigned int x, unsigned int y);
    
};

class ShadowMap{
    
    
public:
    const std::string shader_file = "/Users/diggs/Desktop/OpenGL/OpenGL/Shaders/shadowmap.vs";
    Shader m_sh;
    unsigned int m_fbo;
    unsigned int m_depth;
    unsigned int m_rbo;
    int m_w, m_h;
    const int m_layer = 8;
    
    ShadowMap();
    ShadowMap(unsigned int w, unsigned int h);
    ~ShadowMap();
    
    void Set_MLP( const Light_Src& lsrc) const;
    void Pass();
    void WriteBind();
    void ReadBind() const;
    void Enable();
    void UnBind();
    int Get_Layer()const;
};


#endif /* FrameBuffer_hpp */
