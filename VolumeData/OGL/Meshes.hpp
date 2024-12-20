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
class TOPCon;
class PinHole;
class RHO_3D;

class Atoms_Mesh{
private:
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/Atom.vs";
    int m_num_atoms;
    Shader m_sh;
    
    
    unsigned int m_offset_vbo, m_types_vbo;
    AMD::Vec3* m_offsets = NULL;
    float* m_types = NULL;
    
    friend class Bonds;
public:
    
    Atoms_Mesh();
    ~Atoms_Mesh();
    void Set_Data();
    void Set_Data(RHO_3D& rho);
    void Draw();

    void Set_Shader();
    void Set_Uniforms();
    void Set_Uniforms(Light_Src& src);
    void Sort(char n);
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


class Wire_Frame{
private:
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/wire.vs";
    VertexArray m_VAO;
    IndexBuffer m_tri_IBO;
    IndexBuffer m_line_IBO;
    
    Shader m_sh;
    AMD::Vec3* m_verts = NULL;
    unsigned int m_inst_vb;
    unsigned int m_num = 0;
    unsigned int m_w, m_h;
    bool init = false;
    
public:
    Wire_Frame();
    ~Wire_Frame();
    
    void Set_Data(const char* file);
    void Set_Uniforms();
    void Set_Shader();
    void Draw();
    void Clear();

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



class Grid_3D{
private:
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/Simple_Grid.vs";
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    Shader m_sh;
    unsigned int m_vb_verts;
    unsigned int m_inst_vb;
    unsigned int m_num_bins;
    
    
public:
    Grid_3D();
    Grid_3D(float w);
    Grid_3D(TOPCon& hist);
    Grid_3D(int d, int r, int c);
    ~Grid_3D();
    
    void Set_Uniforms();
    void Set_Shader();
    void Draw();

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
    int m_rows, m_cols, m_depth;
    unsigned int m_num_bins;
    
    AMD::Vec3* m_centers = NULL;
    unsigned int m_inst_vb_centers;
    
    float* m_counts = NULL;
    unsigned int m_inst_vb_counts;
    
    AMD::Vec3 m_offset;
    AMD::Vec3 m_box_dims;
    AMD::Vec2 iface;
    float z_offset = 0.;
    bool init = false;
    
public:
    Voxel_Mesh();
    ~Voxel_Mesh();
    
    void Init(int num);
    void Set_Data(AMD::Voxel* vox, int num);
    void Set_Data(TOPCon& hist);
    void Set_Data(RHO_3D& hist);
    void Set_Data(PinHole& ph);
    void Draw_Set(TOPCon& hist);
    void Set_Uniforms();
    void Set_Shader();
    void Center();
    void Draw();
    void Clear();
    unsigned int Num_Points();
    
    
};




class Voxel_Mesh2{
private:
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/voxel.fs";
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    Shader m_sh;
    int m_rows, m_cols, m_depth;
    unsigned int m_num_bins;
    
    AMD::Vec3* m_centers = NULL;
    unsigned int m_inst_vb_centers;
    
    float* m_counts = NULL;
    unsigned int m_inst_vb_counts;
    
    AMD::Vec3 m_offset;
    AMD::Vec3 m_box_dims;
    AMD::Vec2 iface;
    float z_offset = 0.;
    bool init = false;
    
public:
    Voxel_Mesh2();
    ~Voxel_Mesh2();
    
    void Init(int num);
    void Set_Data(AMD::Voxel* vox, int num);
    void Set_Data(TOPCon& hist);
    void Set_Data(PinHole& ph);
    void Draw_Set(TOPCon& hist);
    void Set_Uniforms();
    void Set_Shader();
    void Center();
    void Draw();
    void Clear();
    unsigned int Num_Points();
    
    
};



class Sphere_Mesh{
private:
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/Sphere.vs";
    int m_num_sp;
    Shader m_sh;
    
    
    unsigned int m_offset_vbo, m_types_vbo;
    AMD::Vec3* m_offsets = NULL;
    float* m_types = NULL;
    
public:
    
    Sphere_Mesh();
    ~Sphere_Mesh();
    void Set_Data(float*** dat, int d, int r, int c);
    void Draw();

    void Set_Shader();
    void Set_Uniforms();
};


class Vector_Mesh{
private:
    VertexArray m_VAO;
    //IndexBuffer m_IBO;
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/vector.vs";
    int m_num;
    Shader m_sh;
    
    
    unsigned int m_pos_vbo, m_clr_vbo;
    AMD::Vec3* m_pos = NULL;
    //AMD::Vec4* m_clr = NULL;
    
public:
    
    Vector_Mesh();
    ~Vector_Mesh();
    void Set_Data(AMD::Vec3* dat, int num_vec);
    void Draw();

    void Set_Shader();
    void Set_Uniforms();
};



class Sim_Box_Mesh{
private:
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/vector.vs";
    Shader m_sh;
    
    
    unsigned int m_pos_vbo;
    AMD::Vec3 m_pos[8];
    unsigned int indices[24] = {0,1, 1,2, 3,2, 3,0, 0,4, 1,5, 2,6, 3,7, 4,5, 5,6, 7,6, 7,4};
public:
    
    Sim_Box_Mesh();
    ~Sim_Box_Mesh();
    void Draw();

    void Set_Shader();
    void Set_Uniforms();
};





class Iso_Mesh{
private:
    const std::string shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/iso2.vs";
    const std::string grid_shader_file = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/iso.vs";
    VertexArray m_VAO;
    Shader m_sh;
    Shader grid_sh;
    AMD::Vec3* m_pos = NULL;
    AMD::Vec4* m_clrs = NULL;
    AMD::Vec4 m_clr;
    unsigned int m_pos_vbo, m_clr_vbo;
    unsigned int m_count;
    
    unsigned int m_max_el;
    
    int hist_depth, hist_rows, hist_cols;
    AMD::Vec3 vox_len;
    AMD::Vec3 m_offset;
    float Curr_Cube[8];
    
    int m_edge_index[12][2];
    AMD::Vec3 m_edges[12];
    void Set_Edges();
    
    void Boundary_Wrapped_Index(int* d, int* r, int* c);
    
    void Evaluate_Cube(std::bitset<8> number);
    void Look_Up_Table(std::bitset<8> number);
    void Add_Triangle(AMD::Ivec3& tri);
    void Add_Triangle_Test(AMD::Ivec3& tri);
    
    AMD::Vec3 LERP(int idx);
    float m_cut;
    
    float initial_area = 0.0;
    bool init = false;
    float ph_area = 0.0;
    float ph_vol = 0.0;
    void Compute_Pinhole_Size();
    
    AMD::Vec3* m_norms = NULL;
public:
    Iso_Mesh();
    Iso_Mesh(float cut);
    Iso_Mesh(TOPCon& hist);
    ~Iso_Mesh();
    
    void Set_Data(TOPCon& hist);
    void Set_Data(RHO_3D& rho);
    void Set_Data(float*** data, int nd, int nr, int nc);
    void Set_Uniforms();
    void Set_Uniforms(Light_Src& light);
    void Set_ShadowMap(ShadowMap& sm);
    void Set_Shader();
    void Set_Grid_Shader();
    void Set_Color(AMD::Vec4& clr);
    void Center();
    void Draw();
    void Clear();
    unsigned int Num_Points();
    
    float Get_Area();
    void Draw_Test(int idx);
    void Compute_Normals();
    AMD::Vec3* Get_Normals();
    int num_normals = 0;
    
};


#endif /* Simulation_hpp */
