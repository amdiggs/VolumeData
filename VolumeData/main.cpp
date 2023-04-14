//
//  main.cpp
//  VolumeData
//
//  Created by Andrew Diggs on 3/9/23.
//


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
#include "Computes.hpp"


Renderer rend(1800,1000, "Volume Data");
UI_Window ui(0.0,0.0,rend.Get_Window());

//init Operator and Light Source
extern Operator* op;
extern Simulation* Sim;

Light_Src l_src;
const char* atom_file = "/Users/diggs/Desktop/TOPCon/OUT-FILES/out-Hy220-03-19-23/Hy2-1400.dump";
const std::string basic_shader2 = "/Users/diggs/Desktop/VolumeData/Resources/Shaders/Basic2.vs";




void junk(float bw){
    
    Hist_3D hist;
    hist.Compute_Ave_Density_3D(bw);
   // hist.Print_Slice(0);
    Voxel_Mesh vox(hist);
    Text_Mesh txt;
    
    
    while (!rend.is_open()) {
        //General stuff
        if(op->need_update){
            op->Set();
            txt.Set_Uinforms();
            vox.Set_Uniforms(l_src);
            op->need_update = false;
        }
        
        //UI stuff
        ui.NewFrame();
        ui.Simple_window(l_src, rend.m_w, rend.m_h);
        //ui.log_window(hist);
        
        rend.Draw_Pass();
        
        //txt.Set_Shader();
        //txt.Render_Tick_Labels(hist.Get_Centers(), hist.Get_Counts()[0], 10, 3.0, "xz");
        
        vox.Set_Shader();
        vox.Draw();
        
        ui.render();
        rend.poll();
        if (Sim->Need_Update()) {
            hist.Compute_Ave_Density_3D(bw);
            vox.Set_Data(hist);
            Sim->Updated();
        }
    }
    
}



int main(int argc, const char * argv[]) {
    float bin_width = 1.3575;
    Sim->Init(atom_file);
    AMD::Timer tr("main");
    Gen_Char_Map();
    junk(bin_width);
    
    
    
    
    return 0;
}
