//
//  Computes.hpp
//  VolumeData
//
//  Created by Andrew Diggs on 3/22/23.
//

#ifndef Computes_hpp
#define Computes_hpp

#include <stdio.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include "OGL/AMDmath.hpp"

class Atom;
class Iso_Mesh;
class TOPCon;


struct Vox_Sphere{
    AMD::Vec3* m_el = nullptr;
    int m_num_el;
    float r = 5.0;
    
    Vox_Sphere();
    ~Vox_Sphere();
};

struct Set{
    int m_ID;
    int num_el;
    int min_z;
    int max_z;
    AMD::Voxel m_vox[4500];
    
    Set();
    Set(int num);
    
    void Push_El(AMD::Voxel& vox);
    bool Check_Neighbors(AMD::Voxel& vox);
    float Delta_Z();
    int Num_El();
    void Print();
    
    void Merge(Set& other);
    

    
};

bool Merge_Check(Set& A, Set& B);



// a 3D histogram should have rows, columns, depth,

class TOPCon {
private:
    bool init;
    int m_rows, m_cols, m_depth;
    float m_min, m_max;
    AMD::Vec3 m_bin_width;
    float m_bin_vol;
    
    
    // cuttoff value for finding voids:
    float m_density_cutoff = 0.1;
    
    
    AMD::Voxel*** m_voxels = NULL;
    int m_num_vox;
    
    float*** m_temp_values = NULL;
    
    float*** counts_Si = NULL;
    float*** counts_Ox = NULL;
    
    
    Atom* m_atoms[12][19][19][10];
    
    //Private functions
    
    void Compute_Fraction(AMD::Vec3& coords, int type, int depth, int row, int col);
    void Compute_Dist_Value(AMD::Vec3& coords, int type);
    void Boundary_Wrapped_Index(int* d, int* r, int* c);
    void Set_MinMax();
    float Average(int depth, int row, int col);
    float Average_X(int depth, int row, int col);
    float No_Ave_X(int depth, int row, int col);
    void Set_Fraction_X(AMD::Vec3& coords, int type);
    void Set_Fraction_X(Atom& at);
    void Clear();
    
    
    
    
    
    
public:
    TOPCon();
    ~TOPCon();
    void Init(float bw);
    
    void Compute_Ave_Density_3D(float bin_width);
    void Compute_Density(float bin_width);
    void Compute_X(float bin_width);
    void GPU_Compute_X(float bw);
    
    void Compute_Hcon(float bin_width);
    
    unsigned int Get_Rows() const;
    unsigned int Get_Cols() const;
    unsigned int Get_Depth() const;
    AMD::Ivec3 Get_Dims() const;
    
    AMD::Vec3 Get_Bin_Widths() const;
    AMD::Voxel*** Get_Counts();
    
    float Get_Bin_Vol() const;
    float Get_Min();
    float Get_Max();
    
    
    
    void Print_Slice(int d);
};


class RHO_3D{
    bool init;
    int m_rows, m_cols, m_depth;
    float m_min, m_max;
    AMD::Vec3 m_bin_width;
    
    
    
    AMD::Voxel*** m_voxels = NULL;
    int m_num_vox;
    
    int m_num_ats = 0;
    AMD::Vec3* m_at_coords = NULL;
    float* m_at_types = NULL;
    
public:
    
    RHO_3D();
    ~RHO_3D();
    
    void Init(const char* file);
    
    AMD::Vec3 Get_Bin_Widths() const;
    AMD::Voxel*** Get_Counts();
    unsigned int Get_Rows() const;
    unsigned int Get_Cols() const;
    unsigned int Get_Depth() const;
    AMD::Ivec3 Get_Dims() const;
    float Get_Bin_Vol() const;
    float Get_Min();
    float Get_Max();
    float* Get_At_Types();
    AMD::Vec3* Get_At_Coords();
    int Get_Num_Ats();
    void Normalize();
    AMD::Vec3 Get_Center();
};



float Find_Interface(TOPCon& dat, const char pos);
AMD::Vec2 Find_Interfaces(TOPCon& dat);

class PinHole{
private:
    AMD::Vec2 Iface;
    AMD::Voxel* m_vox;
    AMD::Vec3 vox_lens;
    int m_num_vox;
    float m_vol;
    float cut = 0.5;
    // for 5.43^3 cell M Si = 3.728729 X 10^(-22) g
    float den = 2.33;
    //float mass_tot = 5.0e-23;
    float mass_tot = 0.;
    float mass_si = 4.66e-23;
    float mass_ox = 2.66e-23;
    float m_num_Si = 0.;
    float m_num_Ox = 0.;
    bool In_Pinhole(AMD::Voxel vox);
    void Compute_Num_Atoms();
    
    std::ofstream m_file;
    const char* m_file_name = "/Users/diggs/Desktop/VolumeData/Output/Volume.txt";
    bool file_is_open = false;

    void Build_Set();
    
    
    
public:
    PinHole();
    ~PinHole();
    
    void Compute_Volume(TOPCon& dat);
    AMD::Voxel* Get_Voxels();
    int Get_Num_Vox();
    float Get_Num_Si();
    float Get_Num_Ox();
    float Get_Volume();
    AMD::Vec2 Get_Interface();
    void Write_Data();
    void Write_Atoms();
    bool write = false;
    
};


class Voxelize{
private:
    int m_num_vox;
    AMD::Voxel* m_vox;
public:
    Voxelize();
    ~Voxelize();
    
    int Get_Num_Vox();
    AMD::Voxel* Get_Vox();
    
    
};
void Compute_MSD(int T);

int Compute_Defects(Iso_Mesh& iso);

#endif /* Computes_hpp */
