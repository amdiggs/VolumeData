//
//  Render.hpp
//  OpenGL
//
//  Created by Andrew Diggs on 7/14/22.
//

#ifndef Render_hpp
#define Render_hpp

#include <stdio.h>
#include <iostream>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "AMDmath.hpp"






class VertexArray;
class Shader;
class Light_Src;
class IndexBuffer;
class UI_Window;
class Atom;
class Bond;
class Atoms_Mesh;
class Bonds;
class Grid_Mesh;
class Quad_Mesh;
class Box_Bounds;
class Test_Object;
class RenderTexture;
class ShadowMap;
class Hist_2D_Grid_Mesh;
class Hist_2D;



class Renderer{
private:
    
    GLFWwindow* m_Window;
    void check(GLFWwindow* window);
    AMD::Vec4 m_cc;

    
    
public:
    Renderer(int w, int l, const char* name);
    ~Renderer();
    GLFWwindow* Get_Window();
    void set_context();
    //void set_color(AMD::Vec4 clr);
    
    void Draw_Pass();
    int is_open();
    void poll();
    int Write_Curr_Buffer(std::string file_name);
    int m_w, m_h;
    
    
    
};

ImGuiIO& init_io();

class UI_Window{
private:
    GLFWwindow* m_window;
    ImGuiIO& m_io;
    AMD::Vec4 m_cc;
    AMD::Vec4 Ob_clr;
 
    float view_x, view_y, view_z;
    float m_x, m_y;
    int display_w, display_h;
    float  ui_x, ui_y;
    const char* m_version = "#version 150";
 
    float projection[4] = {1.0, -3.0, 3.0, 3.0};
    
public:
    UI_Window(float pos_x, float pos_y,GLFWwindow* window);
    ~UI_Window();
    
    
    void Simple_window(Light_Src& l, int w, int h);
    void Simple_window(Light_Src& l, Hist_2D_Grid_Mesh& hist, int w, int h);
    void render() const;
    void NewFrame() const;
    
    AMD::Vec4& get_color();
    
    
    void mouse_drag(AMD::Vec3& vec);
    
    void log_window(Atoms_Mesh& ats);
    void log_window( Bond* bd, int num);
    void log_window( AMD::Vertex* verts, int num);
    void log_window( AMD::Vertex* verts, unsigned int* idx, int num);
    void log_window(Hist_2D& hist);
    void log_window( AMD::Mat4 mat, bool T);
    void log_window(int neb[][2], int num);
    void Write_Buffer(std::string file_name);
    
    
};



#endif /* Render_hpp */




//void Draw_Instanced(IndexBuffer& ib, VertexArray& va, Shader& sh, GLenum draw_type, int num);
//void Draw(IndexBuffer& ib, VertexArray& va, Shader& sh, GLenum draw_type);
//static void glfw_error_callback(int error, const char* description);


