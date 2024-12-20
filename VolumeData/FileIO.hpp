//
//  FileIO.hpp
//  OpenGL
//
//  Created by Andrew Diggs on 8/21/22.
//

#ifndef FileIO_hpp
#define FileIO_hpp

#include <stdio.h>
#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "AMDmath.hpp"
#define MAX_WORD 250
#define MAX_TS 1000


#define TIMESTEP "ITEM: TIMESTEP"
#define NUM "ITEM: NUMBER OF ATOMS"
#define BOX_BOUNDS "ITEM: BOX BOUNDS pp pp pp"
#define ATOM_LINE "ITEM: ATOMS id type xs ys zs"



struct Data_2D{
    float** m_dat = NULL;
    int m_num_el = 0;
    int m_num_rows = 0;
    
    Data_2D(const char* file);
    ~Data_2D();
};


struct Data_3D {
    bool init = false;
    float*** m_dat = NULL;
    int m_nx, m_ny, m_nz;
    AMD::Vec3 m_bin_widths;
    
    Data_3D();
    ~Data_3D();
};

class Rho_Data_3D : public Data_3D{
public:
    int num_ats = 0;
    float* at_types = NULL;
    AMD::Vec3* at_coords = NULL;
    bool at_init = false;
    Rho_Data_3D();
    ~Rho_Data_3D();
};

struct String_List{
    std::string m_words[25];
    std::string& operator[](const int index);
    int m_num_el = 0;
    String_List(std::string line);
    ~String_List();
    
    
};

const std::regex re_TS(TIMESTEP);
const std::regex re_NA(NUM);
const std::regex re_BB(BOX_BOUNDS);
const std::regex re_AL(ATOM_LINE);


bool match(std::string input, std::string m_type);
bool search(std::string input, std::string m_type);
bool match_int(std::string input);
bool match_float(std::string input);
bool match_space(std::string input);
bool match_atom_file(std::string input);
bool match_comment(std::string input);

bool match_TS(const char* line);
bool match_Num_Atoms(const char* line);
bool match_Box_Bounds(const char* line);
bool match_Atom_Line(const char* line);

unsigned int Get_Num_El(std::string line);

unsigned int Hash(const char* line);

int get_int(std::string int_str);
float get_float(std::string fl_str);

bool ITEM(std::string str, std::string reg_ex);
char* Read_Line(std::ifstream& infile);
char** Read_Dump_File(const char* file,int& num_blocks, int* block_idx, int& num_lines);
Rho_Data_3D Read_Charge_Density(const char* file);


#endif /* FileIO_hpp */
