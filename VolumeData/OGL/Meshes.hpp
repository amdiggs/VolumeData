//
//  Simulation.hpp
//  OpenGL
//
//  Created by Andrew Diggs on 10/21/22.
//

#ifndef Simulation_hpp
#define Simulation_hpp

#include <stdio.h>
#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <sstream>
#include "AMDmath.hpp"
#include "vertexbuffer.hpp"
#include "vertexarray.hpp"
#include "shader.hpp"


// This will contain the objects that will be a complete simulation:
// It will have a simbox, atoms and bonds:
// The simbox will be 12 lines:
// The atoms will need a sphere mesh, a VAO, a VBO, an IBO and a shader

class Light_Src;
class Texture;
class Texture3D;
class ShadowMap;
class Hist_2D;
class Hist_3D;


class Atoms_Mesh{
private:
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    const std::string shader_file = "/Users/diggs/Desktop/OpenGL/OpenGL/Shaders/Atom.vs";
    int m_num_atoms;
    int neb_IDs[150000][2];
    int num_bonds =0;
    std::string file_name;
    friend class Bonds;
public:
    
    Atoms_Mesh(std::string atomfile);
    ~Atoms_Mesh();
    void Add_Instance_Layout();
    void Bind();
    void UnBind();
    void Draw(int num);
    void Bind_Texture(Texture& tx, int layer);

    void Set_Op(Light_Src& l);
    void Set_texture(Texture& tx);
    Shader m_sh;
    unsigned int num_idx();
    
    
    
    
};

// an axis needs to know which axis it repersents, the length of that axis, the world space origin of that axis,
// the number of tick labels, the range of values on the axis, and the type of values it repersents. It should be a a mesh with, and for 3D it needs to know what plane to be drawn in:
// ---|----|----|
//    1    2    3
class Axis{
    
private:
    AMD::Vec3 m_dir;
    float m_length;
    AMD::Vec3 m_origin;
    int m_num_ticks;
    float m_range;
    float m_lo;
    std::string m_data_type;
    char m_plane[2];
    
    
    AMD::Vec3 m_positions[25];
    float m_values[25];
    
    void Set_Positions();
    void Set_Values();
    
public:
    Axis(char axis, float lo, float high, AMD::Vec3 start,AMD::Vec3 end, int num_ticks, const char* plane, const char* type);
    Axis(char axis, float* data,int num_vals, AMD::Vec3 start,AMD::Vec3 end, int num_ticks, const char* plane, const char* type);
    ~Axis();
    
    void Update_Data(float* dat);
    void Update_Data(float lo, float high);
    
    AMD::Vec3* Get_Positions();
    float* Get_Values();
    
    void Draw();
};

class Hist_2D_Grid_Mesh{
private:
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/Histogram.vs";
    VertexArray m_VAO;
    IndexBuffer m_line_IBO;
    IndexBuffer m_tri_IBO;
    
    Shader m_sh;
    AMD::Vec4 m_clrs[1000];
    AMD::Vec3 m_verts[1000];
    unsigned int m_inst_vb_counts;
    unsigned int m_inst_vb_colors;
    unsigned int m_num_bins;
    
    
public:
    Hist_2D_Grid_Mesh();
    Hist_2D_Grid_Mesh(Hist_2D& hist);
    ~Hist_2D_Grid_Mesh();
    
    void Set_Data(float** data, int num_x, int num_z);
    void Set_Data(Hist_2D& hist);
    void Set_Uniforms(Light_Src& l);
    void Set_Shader();
    void Bind();
    void UnBind();
    void Draw();
    void Clear();
    unsigned int num_idx();
    float** m_counts;
    int m_rows, m_cols;
    float m_row_width, m_col_width;

};



class Hist_2D_Bar_Mesh{
private:
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/Bar_Hist.vs";
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    Shader m_sh;
    AMD::Vec4 m_colors[1000];
    AMD::Vec3 m_coords[1000];
    unsigned int m_inst_vb_counts;
    unsigned int m_inst_vb_colors;
    unsigned int m_num_bins;
    
    
public:
    Hist_2D_Bar_Mesh();
    Hist_2D_Bar_Mesh(Hist_2D& hist);
    ~Hist_2D_Bar_Mesh();
    
    void Set_Data(float** data, int num_x, int num_z);
    void Set_Data(Hist_2D& hist);
    void Set_Uniforms(Light_Src& l);
    void Set_Shader();
    void Center();
    void Bind();
    void UnBind();
    void Draw();
    void Clear();
    unsigned int Num_Points();
    float** m_counts;
    int m_rows, m_cols;

};



class Quad_Mesh{
private:
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    AMD::Vec3 points[4];
    
    
public:
    Quad_Mesh();
    Quad_Mesh(AMD::Vec3 LL, AMD::Vec3 LR, AMD::Vec3 UR, AMD::Vec3 UL);
    ~Quad_Mesh();
    unsigned int num_idx();
    void Bind();
    void UnBind();
};


class Environment{
private:
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/environment.vs";
    const char* m_sampler = "Env_Tex";
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    
    
    
public:
    Shader m_sh;
    
    Environment();
    ~Environment();
    
    void Bind();
    void UnBind();
    void Set_Uniforms(Light_Src& l);
    void Set_Shader();
    void Draw();
    void Attach_Texture(Texture& tx);
};





class Text_Mesh{
private:
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/Glyph.vs";
    const char* m_sampler = "Text_Tex";
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    Shader m_sh;
    unsigned int m_pos_buffer;
    unsigned int m_color_buffer;
    unsigned int m_tex_buffer;
    unsigned int m_num_chars;
    unsigned int m_num_verts;
    AMD::Vec3 m_verts[4];
    AMD::Vec4 m_colors[4];
    AMD::Vec2 m_tex_coords[4];
    void Draw();
    
public:
    Text_Mesh();
    ~Text_Mesh();
    void Set_Uinforms();
    void Set_Shader();
    void Render_Text(const char* word, AMD::Vec3 pos, float size, const char* plane);
    void Render_Tick_Labels(AMD::Vec3* positions, float* values, int num_vals, float size, const char* plane);
    
};



class Voxel_Mesh{
private:
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/Voxel.fs";
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    Shader m_sh;
    AMD::Vec3* m_centers;
    unsigned int m_inst_vb_centers;
    unsigned int m_num_bins;
    float* m_counts;
    unsigned int m_inst_vb_counts;
    int m_rows, m_cols, m_depth;
    
public:
    Voxel_Mesh();
    Voxel_Mesh(Hist_3D& hist);
    ~Voxel_Mesh();
    
    void Set_Data(float*** data, int rows, int cols, int depth);
    void Set_Data(Hist_3D& hist);
    void Set_Uniforms(Light_Src& l);
    void Set_Shader();
    void Center();
    void Draw();
    void Clear();
    unsigned int Num_Points();
    
    
};


char* ftos(float val, int percsion);
void ftos2(float val, int percsion);



#endif /* Simulation_hpp */
