//
//  Texture.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 10/2/22.
//

#include "Texture.hpp"
#include <iostream>
#include <chrono>
#include <random>
#include "Atomic.hpp"
#include "shader.hpp"
#include "Render.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

extern int num_atoms;
const std::string Image_Folder =  "/Users/diggs/Desktop/OpenGL/OTHER/";

//static unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
//std::minstd_rand0 gen (seed);
//std::uniform_int_distribution<> color(0,255);

std::map<char, Character> CHAR_MAP;


Texture::Texture(int layer)
:m_layer(layer)
{}

Texture::~Texture() {
    glDeleteTextures(1, &m_ID);
}

void Texture::Bind() const {
    glActiveTexture(GL_TEXTURE0 + m_layer);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}


void Texture::UnBind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Gen_Tex_2D(void* dat){
    
    glGenTextures(1, &m_ID);
    glActiveTexture(GL_TEXTURE0 + m_layer);
    glBindTexture(GL_TEXTURE_2D, m_ID);
    if(m_bpp == 4){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_w, m_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, dat);
    }
    else if(m_bpp == 3){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_w, m_h, 0, GL_RGB, GL_UNSIGNED_BYTE, dat);
    }
    else{
        std::cout << "bits per pixel error" << std::endl;
        exit(4);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
}


void Texture::Load(std::string image) {
    stbi_set_flip_vertically_on_load(1);
    std::string _file = Image_Folder + image;
    unsigned char* image_data = stbi_load(_file.c_str(), &m_w, &m_h, &m_bpp, 0);
    
    if(!image_data){
        std::cout << "COULD NOT LOAD IMAGE " << _file << std::endl;
        exit(2);
    }
    
    Gen_Tex_2D((void*)image_data);
    
    stbi_image_free(image_data);
}

int Texture::Get_ID() const{
    return m_ID;
}

int Texture::Get_Layer() const{
    return m_layer;
}


//=======3D Texture class==============================================================
Texture3D::Texture3D(int layer)
:m_layer(layer)
{}

Texture3D::~Texture3D() {
    glDeleteTextures(1, &m_ID);
}

void Texture3D::Bind() const {
    glActiveTexture(GL_TEXTURE0 + m_layer);
    glBindTexture(GL_TEXTURE_3D, m_ID);
}


void Texture3D::UnBind() const {
    glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture3D::Gen_Tex_3D(void* dat){
    
    glGenTextures(1, &m_ID);
    glActiveTexture(GL_TEXTURE0 + m_layer);
    glBindTexture(GL_TEXTURE_3D, m_ID);
    if(m_bpp == 4){
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, m_w, m_h, m_z, 0, GL_RGBA, GL_UNSIGNED_BYTE, dat);
    }
    else if(m_bpp == 3){
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB8, m_w, m_h, m_z, 0, GL_RGB, GL_UNSIGNED_BYTE, dat);
    }
    else{
        std::cout << "bits per pixel error" << std::endl;
        exit(4);
    }
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    glBindTexture(GL_TEXTURE_3D, 0);
    
    
}


void Texture3D::Load(std::string image) {
    stbi_set_flip_vertically_on_load(1);
    std::string _file = Image_Folder + image;
    unsigned char* image_data = stbi_load(_file.c_str(), &m_w, &m_h, &m_bpp, 0);
    
    if(!image_data){
        std::cout << "COULD NOT LOAD IMAGE " << _file << std::endl;
        exit(2);
    }
    
    Gen_Tex_3D((void*)image_data);
    
    stbi_image_free(image_data);
}

int Texture3D::Get_ID() const{
    return m_ID;
}

int Texture3D::Get_Layer() const{
    return m_layer;
}


void Texture3D::Simple_Test(){
    m_w = IW;
    m_h = IW;
    m_z = IW;
    m_bpp = 3;
    GLubyte RGB_Data[IW][IW][IW][3];
    for (int i = 0; i< IW; i++){
        for (int j= 0; j<IW; j++){
            for (int k = 0; k<IW; k++){
            
                RGB_Data[i][j][k][0] = i*2;
                RGB_Data[i][j][k][1] = i*2;
                RGB_Data[i][j][k][2] = k*2;
            }
        }
    }
    Gen_Tex_3D((void*)RGB_Data);
}


void Texture3D::Cavity(){
    m_w = IW;
    m_h = IW;
    m_z = IW;
    m_bpp = 3;
    GLubyte RGB_Data[IW][IW][IW][3];
    float dist;
    for (int i = 0; i< IW; i++){
        for (int j= 0; j<IW; j++){
            for (int k = 0; k<IW; k++){
                float lsq = (i-64)*(i-64) + (j-64)*(j-64) + (k-64)*(k-64);
                dist = sqrt(lsq);
                if(dist < 64.0){
                    RGB_Data[i][j][k][0] = 100;
                    RGB_Data[i][j][k][1] = 0;
                    RGB_Data[i][j][k][2] = 0;
                }
                else{
                    RGB_Data[i][j][k][0] = 0;
                    RGB_Data[i][j][k][1] = 0;
                    RGB_Data[i][j][k][2] = 0;
                    }
            }
        }
    }
    Gen_Tex_3D((void*)RGB_Data);
}



void Texture3D::Cavity(AMD::Vec3* vecs, int num_vecs, AMD::Vec3 box_bounds){
    m_w = IW;
    m_h = IW;
    m_z = IW;
    m_bpp = 3;
    GLubyte RGB_Data[IW][IW][IW][3];
    for (int i = 0; i< IW; i++){
        for (int j= 0; j<IW; j++){
            for (int k = 0; k<IW; k++){
                    RGB_Data[i][j][k][0] = 0;
                    RGB_Data[i][j][k][1] = 0;
                    RGB_Data[i][j][k][2] = 0;
            }
        }
    }
    float sx = m_w/box_bounds.x;
    float sy = m_h/box_bounds.y;
    float sz = m_z/box_bounds.z;
    float dist;
    int center[3];
    for (int i = 0; i < num_vecs; i++){
        center[0] = int(vecs[i].x*sx) + m_w/2;
        center[1] = int(vecs[i].y*sy) + m_h/2;
        center[2] = int(vecs[i].z*sz) + m_z/2;
        for (int j = -32; j< 32; j++){
            for (int k= -32; k<32; k++){
                for (int n = -32; n<32; n++){
                    int a = center[0] + j;
                    int b = center[1] + k;
                    int c = center[2] + n;
                    if(a<0 || b<0 || c<0 || a > m_w -1 || b > m_h -1 || c > m_z - 1){continue;}
                    dist = sqrt(j*j + k*k +n*n);
                    if(dist < 32){
                        RGB_Data[a][b][c][0] = 255;
                        RGB_Data[a][b][c][1] = 0;
                        RGB_Data[a][b][c][2] = 0;
                    }
                    else{continue;}
                }
            }
        }
        
    }
    
    Gen_Tex_3D((void*)RGB_Data);
}



//#####################################################################
Character::Character(unsigned int tex_id, AMD::Vec2 s, AMD::Vec2 B, unsigned int advance)
:m_id(tex_id), Size(s.x, s.y), Bearing(B.x, B.y), Advance(advance){}

Character::Character()
:m_id(0), Size(0.0, 0.0), Bearing(0.0, 0.0), Advance(0){}

void Gen_Char_Map(){
    FT_Library lib;
    
    FT_Error error = FT_Init_FreeType(&lib);
    
    if(error){
        std::cout << "error loading free_type library!!!\n";
    }
    FT_Face face;
    
    error = FT_New_Face(lib, "/System/Library/Fonts/Helvetica.ttc", 0, &face);
    
    if(error == FT_Err_Unknown_File_Format){
        std::cout << "error loading free_type face!!!\n";
    }
    
    else if (error){
        std::cout << "error unknown error face!!!\n";
    }
    
    FT_Set_Pixel_Sizes(face, 0, 48);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    for (unsigned char c = 0; c<128; c++){
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)){
            std::cout << "error failed to load GLYPH!!!\n";
            continue;
        }
        // generate texture
           unsigned int texture;
           glGenTextures(1, &texture);
           glBindTexture(GL_TEXTURE_2D, texture);
           glTexImage2D(
               GL_TEXTURE_2D,
               0,
               GL_RED,
               face->glyph->bitmap.width,
               face->glyph->bitmap.rows,
               0,
               GL_RED,
               GL_UNSIGNED_BYTE,
               face->glyph->bitmap.buffer
           );
           // set texture options
           glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
           glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
           glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
           glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
           // now store character for later use
           Character character(texture,
               AMD::Vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
               AMD::Vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
               (unsigned int)face->glyph->advance.x);
        
        CHAR_MAP[c] = character;
    }
   
    FT_Done_Face(face);
    FT_Done_FreeType(lib);
    
}
































/*
void Texture::Load_RGB(int layer, std::string image) {
    int w, h, bpp;
    stbi_set_flip_vertically_on_load(true);
    std::string _file = Image_Folder + image;
    unsigned char* image_data = stbi_load(_file.c_str(), &w, &h, &bpp, 0);
    unsigned long s = sizeof(image_data);
    if(!image_data){
        std::cout << "COULD NOT LOAD IMAGE " << _file << std::endl;
        exit(2);
    }
    
    glGenTextures(1, &m_ID);
    glActiveTexture(GL_TEXTURE0 + layer);
    glBindTexture(GL_TEXTURE_2D, m_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    stbi_image_free(image_data);
    
}

*/

