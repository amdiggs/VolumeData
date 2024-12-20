//
//  FrameBuffer.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 2/8/23.
//

#include "FrameBuffer.hpp"
#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include "Meshes.hpp"
#include "vertexarray.hpp"
#include "vertexbuffer.hpp"
#include "Operations.hpp"


extern Operator* op;

RenderTexture::RenderTexture(): m_sh(shader_file) {}

RenderTexture::~RenderTexture() {}

void RenderTexture::Init(unsigned int w, unsigned int h) {
    m_w = w;
    m_h = h;
    //Create Framebuffer
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    
    //create Texture
    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    
    //Create DepthBuffer
    // OGLdev picking style
    glGenTextures(1, &m_depth);
    glBindTexture(GL_TEXTURE_2D, m_depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth,0);
    
    
    //configure
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth,0);
    
    //maybe not necessary yet!!
    //GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    //glDrawBuffers(1,drawBuffers);
    
    
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    if (Status != GL_FRAMEBUFFER_COMPLETE){
        std::cout << "Frame Buffer Error:" << Status << std::endl;
        std::cout << w << "   " << h << std::endl;
        exit(25);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTexture::Bind() {
    m_sh.bind();
}

void RenderTexture::UnBind() {
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void RenderTexture::Pass(){
    m_sh.bind();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_w, m_h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
}

void RenderTexture::Draw(Quad_Mesh& qd){
    Bind();
    qd.Bind();
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glDrawElements(GL_TRIANGLES,qd.num_idx(), GL_UNSIGNED_INT, 0);
    glEnable(GL_DEPTH_TEST);
}


PixelInfo RenderTexture::ReadPixel(unsigned int x, unsigned int y) { 
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    
    PixelInfo pixel;
    
    glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &pixel);
    
    glReadBuffer(GL_NONE);
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    
    return pixel;
}

//==============================================================================

ShadowMap::ShadowMap()
:m_sh(shader_file)
{
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    
    glGenTextures(1, &m_depth);
    glBindTexture(GL_TEXTURE_2D, m_depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1000,1000, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_depth,0);
}

ShadowMap::ShadowMap(unsigned int w, unsigned int h)
:m_sh(shader_file), m_w(w), m_h(h)
{
    //create frame buffer
    glGenFramebuffers(1, &m_fbo);
    
    //create depth texture
    glGenTextures(1, &m_depth);
    glActiveTexture(GL_TEXTURE0 + m_layer);
    glBindTexture(GL_TEXTURE_2D, m_depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w,h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth,0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    if (Status != GL_FRAMEBUFFER_COMPLETE){
        std::cout << "Frame Buffer Error:" << Status << std::endl;
        std::cout << w << "   " << h << std::endl;
        exit(25);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

ShadowMap::~ShadowMap() {}

void ShadowMap::WriteBind() {
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_w, m_h);
    glClear(GL_DEPTH_BUFFER_BIT);
    
}

void ShadowMap::ReadBind() const{
    glActiveTexture(GL_TEXTURE0 + m_layer);
    glBindTexture(GL_TEXTURE_2D, m_depth);
}

void ShadowMap::Enable(){
    m_sh.bind();
}

void ShadowMap::UnBind() {
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void ShadowMap::Set_MLP(const Light_Src& lsrc) const{
    m_sh.bind();
    AMD::Mat4 MLP = op->Get_MLP(lsrc);
    m_sh.Set_Uniform_Mat4("u_MLP", &MLP[0][0]);
    m_sh.unbind();
}

void ShadowMap::Pass(){
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_w, m_h);
    glClear(GL_DEPTH_BUFFER_BIT);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    m_sh.bind();
}

int ShadowMap::Get_Layer()const{
    return m_layer;
}
