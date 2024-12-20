//
//  CubeMarchTable.h
//  VolumeData
//
//  Created by Andrew Diggs on 5/10/23.
//

#ifndef CubeMarchTable_h
#define CubeMarchTable_h

#include <stdio.h>
#include <bitset>
#include "AMDmath.hpp"


std::bitset<8> BS_R90y(std::bitset<8> other, int num_rot){
    if(num_rot == 0){return other;}
    std::bitset<8> temp;
    temp[0] = other[4];
    temp[1] = other[0];
    temp[2] = other[3];
    temp[3] = other[7];
    temp[4] = other[5];
    temp[5] = other[1];
    temp[6] = other[2];
    temp[7] = other[6];
    
    return BS_R90y(temp, num_rot-1);
}


std::bitset<8> BS_R90z(std::bitset<8> other, int num_rot){
    if(num_rot == 0){return other;}
    std::bitset<8> temp;
    temp[0] = other[3];
    temp[1] = other[0];
    temp[2] = other[1];
    temp[3] = other[2];
    temp[4] = other[7];
    temp[5] = other[4];
    temp[6] = other[5];
    temp[7] = other[6];
    
    return BS_R90z(temp, num_rot-1);
}



std::bitset<8> BS_R90x(std::bitset<8> other, int num_rot){
    if(num_rot == 0){return other;}
    std::bitset<8> temp;
    temp[4] = other[0];
    temp[5] = other[1];
    temp[1] = other[2];
    temp[0] = other[3];
    temp[7] = other[4];
    temp[6] = other[5];
    temp[2] = other[6];
    temp[3] = other[7];
    
    return BS_R90x(temp, num_rot-1);
}








struct cube_table_el{
    bool is_set;
    int num_tri;
    AMD::Ivec3 tri_idx[4];
    cube_table_el():num_tri(0), is_set(false){};
    cube_table_el(int num, int triangles[][3]);
    cube_table_el(int num, AMD::Ivec3* triangles);
    
    void Print();
    void Flip();
};

class Cube_Table{
private:
    Cube_Table();
    Cube_Table(const Cube_Table&) = delete;
    static Cube_Table inst;
    void Build_Table();
    cube_table_el basis[15];
    std::bitset<8> base_words[15];
    
    cube_table_el R90x(cube_table_el el, int num_rot);
    cube_table_el R90y(cube_table_el el, int num_rot);
    cube_table_el R90z(cube_table_el el, int num_rot);
public:
    static Cube_Table* Get();
    ~Cube_Table();
    cube_table_el Table[256];
    void Test();
};



//###################################################

cube_table_el::cube_table_el(int num, int triangles[][3])
:num_tri(num),is_set(true)
{
    for (int i = 0; i<num_tri; i++) {
        tri_idx[i][0] = triangles[i][0];
        tri_idx[i][1] = triangles[i][1];
        tri_idx[i][2] = triangles[i][2];
    }
    
}


cube_table_el::cube_table_el(int num, AMD::Ivec3* triangles)
:num_tri(num),is_set(true)
{
    for (int i = 0; i<num_tri; i++) {
        tri_idx[i][0] = triangles[i][0];
        tri_idx[i][1] = triangles[i][1];
        tri_idx[i][2] = triangles[i][2];
    }
    
}


void cube_table_el::Flip(){
    int a,b,c;
    for (int i = 0; i<num_tri; i++) {
        a = tri_idx[i][0];
        b = tri_idx[i][1];
        c = tri_idx[i][2];
        
        tri_idx[i][0] = a;
        tri_idx[i][1] = c;
        tri_idx[i][2] = b;
    }
}


//################ CUBE TABLE ##################

Cube_Table::Cube_Table(){
    basis[0] = cube_table_el();
    const char* w0 = "00000000";
    base_words[0] = std::bitset<8>(w0);
    
    int one[1][3] = {0,3,8};
    basis[1] = cube_table_el(1, one);
    const char* w1 = "00000001";
    base_words[1] = std::bitset<8>(w1);
    
    int two_a[2][3] = {{8,9,1},{8,1,3}};
    basis[2] = cube_table_el(2, two_a);
    const char* w2 = "00000011";
    base_words[2] = std::bitset<8>(w2);
    
    int two_b[4][3] = {{0,3,8},{1,11,2},{2,3,0}, {0,1,2}};
    basis[3] = cube_table_el(4, two_b);
    const char* w3 = "00000101";
    base_words[3] = std::bitset<8>(w3);
    
    int two_c[2][3] = {{0,3,8},{5,6,11}};
    basis[4] = cube_table_el(2, two_c);
    const char* w4 = "01000001";
    base_words[4] = std::bitset<8>(w4);
    
    int three_a[3][3] = {{0,8,1},{1,8,7}, {1,7,5}};
    basis[5] = cube_table_el(3, three_a);
    const char* w5 = "00110010";
    base_words[5] = std::bitset<8>(w5);
    
    int three_b[3][3] = {{9,1,8},{1,3,8},{5,6,11}};
    basis[6] = cube_table_el(3, three_b);
    const char* w6 = "01000011";
    base_words[6] = std::bitset<8>(w6);
    
    int three_c[3][3] = {{0,9,1},{3,2,10},{5,6,11}};
    basis[7] = cube_table_el(3, three_c);
    const char* w7 = "01001010";
    base_words[7] = std::bitset<8>(w7);
    
    int four_a[2][3] = {{1,3,5},{5,3,7}};
    basis[8] = cube_table_el(2, four_a);
    const char* w8 = "00110011";
    base_words[8] = std::bitset<8>(w8);
    
    int four_b[4][3] = {{5,9,0},{5,0,6},{0,3,6},{6,3,10}};
    basis[9] = cube_table_el(4, four_b);
    const char* w9 = "10110001";
    base_words[9] = std::bitset<8>(w9);
    
    int four_c[4][3] = {{8,0,2},{2,10,8},{9,4,11},{6,11,4}};
    basis[10] = cube_table_el(4, four_c);
    const char* w10 ="01101001";
    base_words[10] = std::bitset<8>(w10);
    
    int four_d[4][3] = {{0,11,9},{0,7,11},{11,7,6},{0,3,7}};
    basis[11] = cube_table_el(4, four_d);
    const char* w11 ="01110001";
    base_words[11] = std::bitset<8>(w11);
    
    int four_e[4][3] = {{0,8,1},{1,8,7},{1,7,5},{3,2,10}};
    basis[12] = cube_table_el(4, four_e);
    const char* w12 ="00111010";
    base_words[12] = std::bitset<8>(w12);
    
    int four_f[4][3] = {{0,3,8},{9,4,5},{1,11,2},{10,6,7}};
    basis[13] = cube_table_el(4, four_f);
    const char* w13 ="10100101";
    base_words[13] = std::bitset<8>(w13);
    
    int four_g[4][3] = {{1,0,5},{0,10,5},{5,10,6},{0,8,10}};
    basis[14] = cube_table_el(4, four_g);
    const char* w14 ="10110010";
    base_words[14] = std::bitset<8>(w14);
    Build_Table();
    
}


Cube_Table::~Cube_Table(){};

Cube_Table Cube_Table::inst;

Cube_Table* Cube_Table::Get(){
    return &inst;
}

cube_table_el Cube_Table::R90x(cube_table_el el, int num_rot){
    int rot[12] = {4,9,0,8,6,11,2,10,7,5,3,1};
    if(num_rot == 0){return el;}
    AMD::Ivec3 tmp[4];
    for (int i = 0; i<el.num_tri; i++){
        AMD::Ivec3 old = el.tri_idx[i];
        tmp[i].d = rot[old.d];
        tmp[i].r = rot[old.r];
        tmp[i].c = rot[old.c];
    }
    cube_table_el cb(el.num_tri, tmp);
    return R90x(cb, num_rot -1);
}


cube_table_el Cube_Table::R90y(cube_table_el el, int num_rot){
    int rot[12] = {9,5,11,1,8,7,10,3,0,4,2,6};
    if(num_rot == 0){return el;}
    AMD::Ivec3 tmp[4];
    for (int i = 0; i<el.num_tri; i++){
        AMD::Ivec3 old = el.tri_idx[i];
        tmp[i].d = rot[old.d];
        tmp[i].r = rot[old.r];
        tmp[i].c = rot[old.c];
    }
    cube_table_el cb(el.num_tri, tmp);
    return R90y(cb, num_rot -1);
}

cube_table_el Cube_Table::R90z(cube_table_el el, int num_rot){
    int rot[12] = {1,2,3,0,5,6,7,4,9,11,8,10};
        if(num_rot == 0){return el;}
        AMD::Ivec3 tmp[4];
        for (int i = 0; i<el.num_tri; i++){
            AMD::Ivec3 old = el.tri_idx[i];
            tmp[i].d = rot[old.d];
            tmp[i].r = rot[old.r];
            tmp[i].c = rot[old.c];
        }
        cube_table_el cb(el.num_tri, tmp);
        return R90z(cb, num_rot -1);
}


void cube_table_el::Print(){
    for (int j = 0; j < num_tri; j++){
        AMD::Ivec3 temp = tri_idx[j];
        printf("tri %d: %d %d %d\n", j, temp.d, temp.r, temp.c);
    }
}

void Cube_Table::Build_Table(){
    std::bitset<8> curr_word;
    std::bitset<8> base;
    int val = 0;
    for(int i = 0; i< 15; i++){
        base = base_words[i];
        curr_word = base;
        
        val = (int)curr_word.to_ulong();
        Table[val] = basis[i];
        curr_word.flip();
        basis[i].Flip();
        val = (int)curr_word.to_ulong();
        Table[val] = basis[i];
        curr_word.flip();
        basis[i].Flip();
        cube_table_el rot;
        for (int y = 0; y < 4; y++){
            
            
            for (int z =  0; z < 4; z++){
                
                for (int x = 0; x < 4; x++){
                    curr_word = BS_R90y(base, y);
                    curr_word = BS_R90z(curr_word, z);
                    curr_word = BS_R90x(curr_word, x);
                    
                    val = (int)curr_word.to_ulong();
                    if(Table[val].is_set){continue;}
                    rot = R90y(basis[i], y);
                    rot = R90z(rot, z);
                    rot = R90x(rot, x);
                    //std::cout << curr_word << std::endl;
                    //rot.Print();
                    
                    Table[val] = rot;
                    curr_word.flip();
                    rot.Flip();
                    val = (int)curr_word.to_ulong();
                    Table[val] = rot;
                    curr_word.flip();
                    rot.Flip();
                }
            }
        }
    }
}


void Cube_Table::Test(){
    std::bitset<8> temp;
    for (int i = 0; i<8; i++) {
        int idx = pow(2.0, i);
        if(!Table[idx].is_set){
            temp = idx;
            std::cout << temp << std::endl;
         
        }
    }
}




#endif /* CubeMarchTable_h */
