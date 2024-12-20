//
//  Hist2D.hpp
//  VolumeData
//
//  Created by Andrew Diggs on 9/12/23.
//

#ifndef Hist2D_hpp
#define Hist2D_hpp

#include <stdio.h>
#include <cstdlib>
#include "OGL/AMDmath.hpp"

class Atom;
class Hist_2D_Bar_Mesh;
class Hist_2D_Grid_Mesh;
class Hist_2D;


class Hist_2D{
private:
    
    int m_cols, m_rows;
    int m_bins[2];
    AMD::Vec3* m_bin_centers;
    float m_bin_vol;
    float** m_counts;
    bool init = false;
    
    friend Hist_2D_Bar_Mesh;
    friend Hist_2D_Grid_Mesh;
    
    
public:
    Hist_2D();
    Hist_2D(int num_bins_x, int num_bins_y);
    Hist_2D(const Hist_2D&) = delete;
    ~Hist_2D();
    
    void Init(int num_bins_x, int num_bins_y);
    void Free();
    void Clear();
    void Resize(int num_bins_x, int num_bins_y);
    
    void Compute(float **data);
    void Compute(float data[][1000]);
    void Compute_XY_Number_Density(float bin_size);
    void Compute_XY_Number_Density(float bin_size, int type);
    void Compute_XY_Mass_Density(float bin_size);
    void Compute_XY_Mass_Density(float bin_size, int type);
    
    void Compute_XZ_Mass_Density(float bin_size);
    void Compute_XZ_Mass_Density(float bin_size, int type);
    
    
    float m_min, m_max; // don't leave public
    float m_bin_width_x, m_bin_width_y, m_bin_width_z;
    void Set_MinMax();
    
    
    int Get_Rows();
    int Get_Cols();
    float Get_Bin_Vol();
    float** Get_Counts();
    AMD::Vec3* Get_Centers();
    float Get_Delta();
    
    float Compute_Total_Density();
    void Boundary_Wrapped_Index(int* r, int* c);
    
    void Get_Portions(AMD::Vec3& coords, int type, int row, float row_mod, int col, float col_mod);
    
    
    void Print();
    
    
};


#endif /* Hist2D_hpp */
