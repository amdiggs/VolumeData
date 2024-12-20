//
//  main.cpp
//  VolumeData
//
//  Created by Andrew Diggs on 3/9/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <iostream>
#include "OGL/Render.hpp"
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "Atomic.hpp"
#include "Logging.h"
#include "OGL/Meshes.hpp"
#include "OGL/Operations.hpp"
#include "OGL/Texture.hpp"
#include "OGL/FrameBuffer.hpp"
#include "Computes.hpp"
#include <random>
#include <time.h>
#include <bitset>

#define _GNU_SOURCE
#include "dlfcn.h"
unsigned long tot_calls = 0;
unsigned long long tot_mem = 0;
typedef void* (*malloc_like_func)(size_t);
void* malloc(size_t size){
    tot_calls ++;
    tot_mem += size;
    malloc_like_func sys_malloc =(malloc_like_func)dlsym(RTLD_NEXT, "malloc");
    return sys_malloc(size);
}

typedef void (*free_func)(void*);
void free(void* ptr){
    tot_calls --;
    free_func sys_free = (free_func)dlsym(RTLD_NEXT, "free");
    sys_free(ptr);
    if (tot_calls < 1) {
        printf("total call = %lu\n", tot_calls);
    }
}





std::default_random_engine gen;
std::uniform_real_distribution<float> rnd(0, 1);

Renderer rend(1800,1000, "Volume Data");
UI_Window ui(0.0,0.0,rend.Get_Window());

//init Operator and Light Source
extern Operator* op;
extern Simulation* Sim;

Light_Src l_src;
const char* atom_file = "/Users/diggs/Desktop/VolumeData/hydrogen-1900-0.0.dump";
//const char* atom_file = "/Users/diggs/Desktop/TOPCon/OUT-FILES/HCON/Hcon-Combined-Relaxed/Hcon-1400-1-relaxed.dump";

const char* rho_file = "/Users/diggs/Desktop/VolumeData/out-Q0-rho-10-01-24/Q0-rho.dat";

void Draw_Vox_Full(float bw);
void Draw_Vox_Pinhole(float bw);
void Draw_Iso(float bw);
void Draw_Iso_Atoms(float bw);
void Iso_Test(float bw);
void Draw_Wire();
void Draw_Rho();
float f_vals[4][16] = {{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,0.0,0.0, 0.0,0.0,0.0,0.0},
    {1.0,1.0,1.0,1.0, 1.0,1.0,1.0,1.0, 1.0,1.0,1.0,1.0, 1.0,1.0,0.0,1.0},
    {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}
};


float*** Build_Arr(int depth, int rows, int cols){
    float*** vals = (float***)malloc(depth*sizeof(float**));
    int count;
    for (int d = 0; d<depth; d++){
        vals[d] = (float**)malloc(rows*sizeof(float*));
        count = 0;
        for(int r = 0; r< rows; r++){
            vals[d][r] = (float*)malloc(cols*sizeof(float));
            for(int c = 0; c<cols; c++){
                vals[d][r][c] = f_vals[d][count];
                count++;
            }
        }
    }
    return vals;
}


void Delete_Arr(float*** arr,int depth, int rows){
    for (int i =0; i<depth; i++){
        for(int j = 0; j < rows; j++){
            free(arr[i][j]);
        }
        free(arr[i]);
    }
    
    free(arr);
}




void Draw_Vox_Sphere(){
    Voxelize sp;
    Voxel_Mesh vx;
    vx.Set_Data(sp.Get_Vox(), sp.Get_Num_Vox());
    while (!rend.is_open()) {
        //General stuff
        if(op->need_update){
            op->Set();
            vx.Set_Uniforms();
            op->need_update = false;
        }
        
        //UI stuff
        ui.NewFrame();
        ui.Simple_window(l_src);
        
        
        rend.Draw_Pass();
        
        vx.Set_Shader();
        vx.Draw();
       
        
        ui.render();
        rend.poll();
        
        if (Sim->Need_Update()) {
            Sim->Updated();
        }
         
    }
     
    
}






int main(int argc, const char * argv[]) {
    Sim->Init(atom_file);
  
    Draw_Rho();
    return 0;
}














 void Draw_Vox_Full(float bw){
     TOPCon hist;
     hist.GPU_Compute_X(bw);
     Voxel_Mesh vx;
     vx.Set_Data(hist);
     
     //PinHole ph;
     //ph.Compute_Volume(hist);
     //Voxel_Mesh2 vx2;
     //vx2.Set_Data(ph);
     
     
     //float v, si, ox;
     //ui.Push_Item("average x", &v);
     //ui.Push_Item("Num Si", &si);
     //ui.Push_Item("Num O", &ox);
     //si = ph.Get_Num_Si();
     //ox = ph.Get_Num_Ox();
     //v = ox / si;
     //ph.Write_Data();
     while (!rend.is_open()) {
         //General stuff
         if(op->need_update){
             op->Set();
             vx.Set_Uniforms();
             //vx2.Set_Uniforms();
             op->need_update = false;
         }
         
         //UI stuff
         ui.NewFrame();
         ui.Simple_window(l_src);
         
         
         rend.Draw_Pass();
         
         
         //vx2.Set_Shader();
         //vx2.Draw();
         
         vx.Set_Shader();
         vx.Draw();
        
         
         ui.render();
         rend.poll();
         
         if (Sim->Need_Update()) {
             hist.GPU_Compute_X(bw);
             //ph.Compute_Volume(hist);
             vx.Set_Data(hist);
             //vx2.Set_Data(ph);
             
             //si = ph.Get_Num_Si();
             //ox = ph.Get_Num_Ox();
             //v = ox / si;
             //ph.Write_Data();
             Sim->Updated();
         }
          
     }
      
     
 }


void Draw_Rho(){
    ShadowMap sm(1000,1000);
    RHO_3D rho;
    rho.Init(rho_file);
    AMD::Vec4 clr1(0.5,0.75,1.0,0.6);
    AMD::Vec4 clr2(1.0,0.6,0.0,0.4);
    Iso_Mesh iso(0.2);
    iso.Set_Color(clr1);
    iso.Set_Data(rho);
    //Iso_Mesh iso2(0.1);
    //iso2.Set_Color(clr2);
    //iso2.Set_Data(rho);
    
    Atoms_Mesh ats;
    ats.Set_Data(rho);
    
    
    
    while (!rend.is_open()) {
        //General stuff
        if(op->need_update){
            op->Set();
            iso.Set_Uniforms(l_src);
            ats.Set_Uniforms(l_src);
            op->need_update = false;
        }
        
        //UI stuff
        ui.NewFrame();
        ui.Simple_window(l_src);
        
        //Framebuffer/Shadowmap pass
        sm.Set_MLP(l_src);
        sm.Pass();
        iso.Draw();
        
        rend.Draw_Pass();
        
        ats.Set_Shader();
        ats.Draw();
        
        //iso.Set_Shader();
        //iso.Set_ShadowMap(sm);
        //iso.Draw();
        
        iso.Set_Grid_Shader();
        iso.Draw();
        
        
        
        ui.render();
        rend.poll();
         
    }
     
    
}


void Draw_Vox_Pinhole(float bw){
    
    TOPCon hist;
    //hist.Compute_X(bw);
    hist.GPU_Compute_X(bw);
    PinHole ph;
    ph.Compute_Volume(hist);
    Voxel_Mesh vx;
    vx.Set_Data(ph);
    
    float v, si, ox;
    ui.Push_Item("Pinhole Volume", &v);
    ui.Push_Item("Num Si", &si);
    ui.Push_Item("Num O", &ox);
    v = ph.Get_Volume();
    si = (float)ph.Get_Num_Si();
    ox = (float)ph.Get_Num_Ox();
    ph.Write_Data();
    while (!rend.is_open()) {
        //General stuff
        if(op->need_update){
            op->Set();
            vx.Set_Uniforms();
            op->need_update = false;
        }
        
        //UI stuff
        ui.NewFrame();
        ui.Simple_window(l_src, ph);
        
        
        rend.Draw_Pass();
        
        vx.Set_Shader();
        vx.Draw();
       
        
        ui.render();
        rend.poll();
        
        if (Sim->Need_Update()) {
            hist.GPU_Compute_X(bw);
            ph.Compute_Volume(hist);
            v = ph.Get_Volume();
            si = (float)ph.Get_Num_Si();
            ox = (float)ph.Get_Num_Ox();
            vx.Set_Data(ph);
            ph.Write_Data();
            Sim->Updated();
        }
         
    }
         
    
}



void Draw_Iso(float bw){
    ShadowMap sm(1000,1000);
    
    TOPCon hist;
    hist.GPU_Compute_X(bw);
    Iso_Mesh iso;
    iso.Set_Data(hist);
    
    float del;
    ui.Push_Item("Pinhole Surface area", &del);
    del = iso.Get_Area();
    
    while (!rend.is_open()) {
        //General stuff
        if(op->need_update){
            op->Set();
            iso.Set_Uniforms(l_src);
            op->need_update = false;
        }
        
        //UI stuff
        ui.NewFrame();
        ui.Simple_window(l_src);
        //ui.log_window(hist);
        
        //Framebuffer/Shadowmap pass
        sm.Set_MLP(l_src);
        sm.Pass();
        iso.Draw();
        
        rend.Draw_Pass();
        
        //txt.Set_Shader();
        //txt.Render_Tick_Labels(hist.Get_Centers(), hist.Get_Counts()[0], 10, 3.0, "xz");
        
        iso.Set_Shader();
        iso.Set_ShadowMap(sm);
        iso.Draw();
        
        iso.Set_Grid_Shader();
        iso.Draw();
        
        ui.render();
        rend.poll();
        
        if (Sim->Need_Update()) {
            hist.Compute_X(bw);
            iso.Set_Data(hist);
            del = iso.Get_Area();
            Sim->Updated();
        }
         
    }
         
    
}


void Draw_Iso_Atoms(float bw){
    ShadowMap sm(1000,1000);
    
    TOPCon hist;
    hist.Compute_X(bw);
    
    Iso_Mesh iso;
    iso.Set_Data(hist);
    iso.Compute_Normals();
    
    Sim_Box_Mesh sbm;
    
    Atoms_Mesh ats;
    
    float def;
    ui.Push_Item("Number of defects", &def);
    
    def = (float)Compute_Defects(iso);
    while (!rend.is_open()) {
        //General stuff
        if(op->need_update){
            op->Set();
            iso.Set_Uniforms(l_src);
            sbm.Set_Uniforms();
            ats.Set_Uniforms(l_src);
            op->need_update = false;
        }
        
        //UI stuff
        ui.NewFrame();
        ui.Simple_window(l_src);
        
        //Framebuffer/Shadowmap pass
        sm.Set_MLP(l_src);
        sm.Pass();
        iso.Draw();
        
        rend.Draw_Pass();
        
        
        iso.Set_Shader();
        iso.Set_ShadowMap(sm);
        iso.Draw();
        
        iso.Set_Grid_Shader();
        iso.Draw();
        
        sbm.Set_Shader();
        sbm.Draw();
        
        ats.Set_Shader();
        ats.Draw();
        
        
        ui.render();
        rend.poll();
        
        if (Sim->Need_Update()) {
            hist.Compute_X(bw);
            iso.Set_Data(hist);
            iso.Compute_Normals();
            def = (float)Compute_Defects(iso);
            //vm.Set_Data(iso.Get_Normals(), iso.num_normals);
            ats.Set_Data();
            //ats.Sort('y');
            Sim->Updated();
        }
         
    }
   
}

void Iso_Test(float bw){
    ShadowMap sm(1000,1000);
    int d = 4;
    int r = 4;
    int c = 4;
    float*** fake_dat = Build_Arr(d, r, c);
    AMD::Vec3 basis[4];
    
    basis[0] = AMD::Vec3(0.0,0.0,0.0);
    basis[1] = AMD::Vec3(1.0,0.0,0.0);
    basis[2] = AMD::Vec3(1.0,1.0,0.0);
    basis[3] = AMD::Vec3(0.0,1.0,0.0);
    
    Iso_Mesh iso;
    iso.Set_Data(fake_dat,d,r,c);
    iso.Compute_Normals();
    
    Sphere_Mesh spm;
    spm.Set_Data(fake_dat, d, r, c);
    
    Vector_Mesh vm;
    vm.Set_Data(iso.Get_Normals(), iso.num_normals);
    
    Grid_3D gr(d,r,c);
    
    
    while (!rend.is_open()) {
        //General stuff
        if(op->need_update){
            op->Set();
            iso.Set_Uniforms(l_src);
            spm.Set_Uniforms();
            vm.Set_Uniforms();
            gr.Set_Uniforms();
            op->need_update = false;
        }
        
        //UI stuff
        ui.NewFrame();
        ui.Simple_window(l_src);
        //ui.log_window(hist);
        
        //Framebuffer/Shadowmap pass
        sm.Set_MLP(l_src);
        sm.Pass();
        iso.Draw();
        
        rend.Draw_Pass();
        
        
        iso.Set_Shader();
        iso.Set_ShadowMap(sm);
        iso.Draw();
        
        iso.Set_Grid_Shader();
        iso.Draw();
        
        
        spm.Set_Shader();
        spm.Draw();
        
        vm.Set_Shader();
        vm.Draw();
        gr.Set_Shader();
        gr.Draw();
        ui.render();
        rend.poll();
        
        if (Sim->Need_Update()) {
            iso.Set_Data(fake_dat,d,r,c);
            Sim->Updated();
        }
         
    }
    Delete_Arr(fake_dat, d, r);
    
}

void Draw_Wire(){
    const char* file = "/Users/diggs/Desktop/PES/LJ.txt";
    Wire_Frame wf;
    wf.Set_Data(file);
    while (!rend.is_open()) {
        //General stuff
        if(op->need_update){
            op->Set();
            wf.Set_Uniforms();
            op->need_update = false;
        }
        
        //UI stuff
        ui.NewFrame();
        ui.Simple_window(l_src);
        
        
        rend.Draw_Pass();
        
        wf.Set_Shader();
        wf.Draw();
       
        
        ui.render();
        rend.poll();
        
      
         
    }
     
    
}




 
 /*
 
 void Draw_Hcon(float bw){
     
     Data_3D hist;
     hist.Compute_Hcon(bw);
     Voxel_Mesh vx;
     vx.Set_Data(hist);
     
     while (!rend.is_open()) {
         //General stuff
         if(op->need_update){
             op->Set();
             vx.Set_Uniforms();
             op->need_update = false;
         }
         
         //UI stuff
         ui.NewFrame();
         ui.Simple_window(l_src);
         
         
         rend.Draw_Pass();
         
         vx.Set_Shader();
         vx.Draw();
        
         
         ui.render();
         rend.poll();
         
         if (Sim->Need_Update()) {
             hist.Compute_Hcon(bw);
             vx.Set_Data(hist);
             Sim->Updated();
         }
          
     }
          
     
 }


 

  
  float*** Build_Arr(int depth, int rows, int cols){
      float*** vals = (float***)malloc(depth*sizeof(float**));
      for (int i = 0; i<depth; i++){
          vals[i] = (float**)malloc(rows*sizeof(float*));
          for(int j = 0; j< rows; j++){
              vals[i][j] = (float*)malloc(cols*sizeof(float));
              for(int k = 0; k<cols; k++){
                  if(j == 1){vals[i][j][k] = 1.0;}
                  else{vals[i][j][k] = 0.0;} //rnd(gen);
              }
          }
      }
      return vals;
  }


  void Delete_Arr(float*** arr,int depth, int rows){
      for (int i =0; i<depth; i++){
          for(int j = 0; j < rows; j++){
              free(arr[i][j]);
          }
          free(arr[i]);
      }
      
      free(arr);
  }



 */
