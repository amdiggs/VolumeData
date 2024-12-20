//
//  Render.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 7/14/22.
//

#include "Render.hpp"
#include "vertexbuffer.hpp"
#include "vertexarray.hpp"
#include "shader.hpp"
#include "Atomic.hpp"
#include "Meshes.hpp"
#include "FrameBuffer.hpp"
#include "Operations.hpp"
#include "Computes.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


//std::string save_file = "Users/diggs/Desktop/volume.png";
bool save = false;
static AMD::Vec4 CC(1.0,1.0,1.0,1.0);
int Ww =0;
int Wh = 0;
extern Operator* op;

ImGuiIO& init_io(){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    return io;
}

Renderer::Renderer(int w, int l, const char* name)
{   glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    this -> m_Window = glfwCreateWindow(w, l, name, NULL, NULL);
    check(m_Window);
    set_context();

    
}


Renderer::~Renderer(){
    std::string save_file = "/Users/diggs/Desktop/test.png";
    if(save){
        Write_Curr_Buffer(save_file);
    }
    glfwTerminate();
}


void Renderer::check(GLFWwindow* window){
    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }
}


void Renderer::set_context(){
    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);
    glewExperimental = GL_TRUE;
    glewInit();
    glfwGetFramebufferSize(m_Window, &Ww, &Wh);
}

void Renderer::Draw_Pass(){
    glClearColor(CC[0], CC[1], CC[2], 0.0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glfwGetFramebufferSize(m_Window, &Ww, &Wh);
    glViewport(0, 0, Ww, Wh);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glDisable(GL_CULL_FACE);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}





int Renderer::is_open(){
    return glfwWindowShouldClose(m_Window);
}

void Renderer::poll(){
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}





int Renderer::Write_Curr_Buffer(std::string file_name){
    stbi_flip_vertically_on_write(1);
    int width, height;
    glfwGetFramebufferSize(m_Window, &width, &height);
    const int num_pix = 3 * width * height;
    unsigned char* pixels = new unsigned char[num_pix];
    
    std::fstream outfile;
    outfile.open(file_name, std::ios::out);
    
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    if(stbi_write_png(file_name.c_str(), width, height, 3, pixels, width*3*sizeof(unsigned char))){
        return 1;
    }
    else return -1;
    
}




GLFWwindow* Renderer::Get_Window(){
    return m_Window;
}









//##################################################################################################
//THIS IS MY UI CLASS!!!!!!!!!!!

UI_Window::UI_Window(float pos_x, float pos_y, GLFWwindow* window)
: m_x(pos_x), m_y(pos_y), m_window(window), m_io(init_io())
{
    (void)m_io;
    m_cc[0] = 1.0;
    m_cc[1] = 1.0;
    m_cc[2] = 1.0;
    m_cc[3] = 0.0;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(m_version);
    
    
    ImFont* font = m_io.Fonts -> AddFontFromFileTTF("/System/Library/Fonts/Helvetica.ttc", 16.0);
    IM_ASSERT(font != NULL);
    //glfwGetFramebufferSize(m_window, &display_w, &display_h);
    view_x = 0.0;
    view_y = 0.0;
    
}

UI_Window::~UI_Window() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}





void UI_Window::Simple_window(Light_Src& light_src){
    static float theta = 0.0;
    static float phi = 0.0;
    static float M_theta= 1.57;
    static float M_phi= 0.0;
    static bool play = false;
    static int counter = 1;
    static float C_theta = 0.0;
    static float C_phi = 0.0;
    static int timestep = 0;
    static AMD::Vec3 trans;
    static AMD::Vec3 look;
    static AMD::Vec3 Cam_Pos(-5.5,0.0,-65.0);
    static AMD::Vec4 Proj(3.0, 3.0, 1.0, 100.0);
    const ImGuiKey m_keys[4] ={ImGuiKey_UpArrow, ImGuiKey_DownArrow, ImGuiKey_RightArrow, ImGuiKey_LeftArrow};
    op->need_update = false;
    
    ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));


    ImGui::Begin("UI prarmeters");
    //####These are the functions that act on the Operator!!##################################
    for(int i = 0; i< num_items; i++){
        ImGui::Text("%s", m_names[i]);
        ImGui::SameLine();
        ImGui::Text(" %.2f", *(m_items[i]));
    }
    ImGui::Text("display w = %d, h = %d", Ww, Wh);// Edit 1 float using a slider from 0.0f to 1.0f
    if(abs(op->w_scale - ((float)Wh / (float)Ww)) > 0.001){
        op->need_update = true;
        ImGui::Text("%s", "window resize");
    }

    //Projection operations
   
    
    
    if(ImGui::InputFloat("View X", &Cam_Pos.x, 0.05f, 1.0f, "%.2f")){op->need_update = true;}
    if(ImGui::InputFloat("View Y", &Cam_Pos.y, 0.05f, 1.0f, "%.2f")){op->need_update = true;}
    if(ImGui::InputFloat("View Z", &Cam_Pos.z, 0.05f, 1.0f, "%.2f")){op->need_update = true;}
    
    if(ImGui::InputFloat("Near", &Proj.b, 1.0f, 1.0f, "%.2f")){
        op->Get_Proj_vec() = Proj;
        op->need_update = true;
        
    }
    if(ImGui::InputFloat("Far", &Proj.a, 1.0f, 100.0f, "%.2f")){
        op->Get_Proj_vec() = Proj;
        op->need_update = true;
        
    }
    
    
    for ( int i = 0; i < 4; i++){
    if (ImGui::IsKeyDown(m_keys[i]) && !m_io.KeyShift && !m_io.KeyAlt){
        
        switch (i) {
            case 0:
                //Cam_Pos.y += 0.1;
                op->need_update = true;
                C_theta -= 0.1;
                break;
                
            case 1:
                //Cam_Pos.y -= 0.1;
                op->need_update = true;
                C_theta += 0.1;
                break;
            case 2:
                //op.m_Cam.Move_LeftRight(-0.01);
                C_phi +=0.1;
                op->need_update = true;
                break;
                
            case 3:
                //op.m_Cam.Move_LeftRight(0.01);
                C_phi -=0.1;
                op->need_update = true;
                break;
                
                
                
            default:
                break;
        }
        
    }
    }
    
    if (m_io.KeyShift){
        
        if (ImGui::IsKeyDown(ImGuiKey_UpArrow)){
            Cam_Pos.z += 0.5;
            op->need_update = true;
            
        }
        else if (ImGui::IsKeyDown(ImGuiKey_DownArrow)){
            Cam_Pos.z -= 0.5;
            op->need_update = true;
            
        }
        else if (ImGui::IsKeyDown(ImGuiKey_LeftArrow)){
            Cam_Pos.x -= 0.5;
            op->need_update = true;
        }
        else if (ImGui::IsKeyDown(ImGuiKey_RightArrow)){
            Cam_Pos.x += 0.5;
            op->need_update = true;
        }
    }
    
    
    if(!m_io.WantCaptureMouse){
        if(ImGui::IsMouseDragging(0)){
            if(abs(m_io.MouseDelta.x) > abs(m_io.MouseDelta.y)){
                M_phi += 0.005*m_io.MouseDelta.x;
            }
            else{M_theta  += 0.005*m_io.MouseDelta.y;}
            op->need_update = true;
            
        }
    }
    float wheel_test = m_io.MouseWheel;
    if(wheel_test){
        Cam_Pos.z += 0.25*wheel_test;
        op->need_update = true;
    }
    ImGui::Text("Light Color:"); ImGui::SameLine(); ImGui::Text("Clear Color:");
    float w = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.y) * 0.40f;
    ImGui::SetNextItemWidth(w);
    if(ImGui::ColorPicker4("##MyColor##2", light_src.Get_Color_ptr(), ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha)){
        op->need_update = true;
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(w);
    ImGui::ColorPicker3("##MyColor##3", CC.get(), ImGuiColorEditFlags_NoInputs);


    static int div1 = 0;
    static int div2 = 0;
    ImGui::VSliderInt("theta", ImVec2(30, 100), &div1, 0, 24);
    ImGui::SameLine(100.0f);
    ImGui::VSliderInt("phi", ImVec2(30, 100), &div2, 0, 24);
    theta = PI*(div1/10.0);
    phi = PI*(div2/10.0);
    
    light_src.Set_Pos(theta, phi);
    AMD::Vec3 light = light_src.Get_Pos();
    ImGui::Text("Camera");
    ImGui::SameLine();
    ImGui::Text("theta = %f, phi = %f", C_theta, C_phi);
    ImGui::Text("theta = %f, phi = %f", theta, phi);
    ImGui::Text("Light Source Position");
    ImGui::SameLine();
    ImGui::Text("x = %.2f, y = %.2f, z = %.2f", light.x, light.y,light.z);
    ImGui::Text("Target");
    ImGui::SameLine();
    ImGui::Text("x = %.2f, y = %.2f, z = %.2f", light_src.Get_Target().x,light_src.Get_Target().z,light_src.Get_Target().z);
    ImGui::Text("Direction");
    ImGui::SameLine();
    ImGui::Text("x = %.2f, y = %.2f, z = %.2f", light_src.Get_Direction_vec().x,light_src.Get_Direction_vec().y,light_src.Get_Direction_vec().z);
    
    static float sx = 0.0;
    static float sy = 0.0;
    static float sz = 0.0;
    if(ImGui::VSliderFloat("Shift X", ImVec2(30, 100), &sx, 0.0, 1.0)){
        Simulation::Get()->shift.x = sx;
        Simulation::Get()->Step_Zero();
        timestep = Simulation::Get()->Timestep();
        
    }
    ImGui::SameLine(120.0f);
    if(ImGui::VSliderFloat("Shift Y", ImVec2(30, 100), &sy, 0.0, 1.0)){
        Simulation::Get()->shift.y = sy;
        Simulation::Get()->Step_Zero();
        timestep = Simulation::Get()->Timestep();
        
    }
    ImGui::SameLine(240.0f);
    if(ImGui::VSliderFloat("Shift z", ImVec2(30, 100), &sz, 0.0, 1.0)){
        Simulation::Get()->shift.z = sz;
        Simulation::Get()->Step_Zero();
        timestep = Simulation::Get()->Timestep();
        
    }
    //op.m_Cam.Look_At(AMD::Vec3(0.0,0.0,10.0));
    ImGui::InputFloat("Color Saturation", light_src.Get_Dir_ptr(), 0.01f, 1.0f, "%.2f");
    
    
        if (ImGui::Button("SAVE")){
            std::string dir = "/Users/diggs/Desktop/";
            std::string file = "volume-" + std::to_string(timestep) + ".png";
            std::string  save = dir + file;
            //save = true;
            Write_Buffer(save);
        }
    
    //ImGui::Text("%s", save_file.c_str());
    ImGui::Text("%s","Play");
    if(!play){
        if(ImGui::Button(">")){
            play = true;
        }
    }
    if(play){
        if(ImGui::Button("||")){
            play = false;
        }
        if(!(counter%100)){
            Simulation::Get()->Step_Forward();
            timestep = Simulation::Get()->Timestep();
            counter = 0;
        }
        counter++;
    }
    
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Timestep");
    ImGui::SameLine();
    
    ImGui::PushID(0);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3.0 / 7.0f, 0.6f, 0.6f));
    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
        Simulation::Get()->Step_Bacward();
        timestep = Simulation::Get()->Timestep();
    }
    ImGui::PopStyleColor(1);
    ImGui::PopID();
    
    ImGui::SameLine(0.0f, spacing);
    
    ImGui::PushID(1);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3.0 / 7.0f, 0.6f, 0.6f));
    if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
        Simulation::Get()->Step_Forward();
        timestep = Simulation::Get()->Timestep();
    }
    ImGui::PopStyleColor(1);
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::Text(" %d",timestep);
    
    
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
    
    if(op->need_update){
        op->Set_W_Scale(Ww, Wh);
        op->m_Cam.Move_To(Cam_Pos);
        op->m_Cam.Look_UpDown(C_theta);
        op->m_Cam.Look_LeftRight(C_phi);
        op->m_model.Rotate_Pole(M_theta);
        op->m_model.Rotate_Azimuth(M_phi);
    }
}



void UI_Window::Push_Item(const char* name, float* item){
    m_items[num_items] = item;
    m_names[num_items] = name;
    num_items++;
}



 void UI_Window::NewFrame() const{
     ImGui_ImplOpenGL3_NewFrame();
     ImGui_ImplGlfw_NewFrame();
     ImGui::NewFrame();
 }

 void UI_Window::render() const{
     ImGui::Render();
     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 }



void UI_Window::Write_Buffer(std::string file_name){
    stbi_flip_vertically_on_write(1);
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    width-=600;
    const int num_pix = 3 * width * height;
    unsigned char* pixels = new unsigned char[num_pix];
    
    std::fstream outfile;
    outfile.open(file_name, std::ios::out);
    
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadBuffer(GL_FRONT);
    glReadPixels(600, 0, width , height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    if(stbi_write_png(file_name.c_str(), width, height, 3, pixels, width*3*sizeof(unsigned char))){
        return;
    }
    else return;
    
}


void UI_Window::Simple_window(Light_Src& light_src, PinHole& ph){
    static float theta = 0.0;
    static float phi = 0.0;
    static float M_theta= 1.57;
    static float M_phi= 0.0;
    static bool play = false;
    static int counter = 1;
    static float C_theta = 0.0;
    static float C_phi = 0.0;
    static int timestep = 0;
    static AMD::Vec3 trans;
    static AMD::Vec3 look;
    static AMD::Vec3 Cam_Pos(-5.5,0.0,-65.0);
    const ImGuiKey m_keys[4] ={ImGuiKey_UpArrow, ImGuiKey_DownArrow, ImGuiKey_RightArrow, ImGuiKey_LeftArrow};
    op->need_update = false;
    
    ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));


    ImGui::Begin("UI prarmeters");
    //####These are the functions that act on the Operator!!##################################
    for(int i = 0; i< num_items; i++){
        ImGui::Text("%s", m_names[i]);
        ImGui::SameLine();
        ImGui::Text(" %.2f", *(m_items[i]));
    }
    ImGui::Text("display w = %d, h = %d", Ww, Wh);// Edit 1 float using a slider from 0.0f to 1.0f
    if(abs(op->w_scale - ((float)Wh / (float)Ww)) > 0.001){
        op->need_update = true;
        ImGui::Text("%s", "window resize");
    }

    //Projection operations
   
    
    
    if(ImGui::InputFloat("View X", &Cam_Pos.x, 0.05f, 1.0f, "%.2f")){op->need_update = true;}
    if(ImGui::InputFloat("View Y", &Cam_Pos.y, 0.05f, 1.0f, "%.2f")){op->need_update = true;}
    if(ImGui::InputFloat("View Z", &Cam_Pos.z, 0.05f, 1.0f, "%.2f")){op->need_update = true;}
    
    
    for ( int i = 0; i < 4; i++){
    if (ImGui::IsKeyDown(m_keys[i]) && !m_io.KeyShift && !m_io.KeyAlt){
        
        switch (i) {
            case 0:
                Cam_Pos.y += 0.1;
                op->need_update = true;
                //C_theta -= 0.1;
                break;
                
            case 1:
                Cam_Pos.y -= 0.1;
                op->need_update = true;
                //C_theta += 0.1;
                break;
            case 2:
                //op.m_Cam.Move_LeftRight(-0.01);
                C_phi +=0.1;
                op->need_update = true;
                break;
                
            case 3:
                //op.m_Cam.Move_LeftRight(0.01);
                C_phi -=0.1;
                op->need_update = true;
                break;
                
                
                
            default:
                break;
        }
        
    }
    }
    
    if (m_io.KeyShift){
        
        if (ImGui::IsKeyDown(ImGuiKey_UpArrow)){
            Cam_Pos.z += 0.5;
            op->need_update = true;
            
        }
        else if (ImGui::IsKeyDown(ImGuiKey_DownArrow)){
            Cam_Pos.z -= 0.5;
            op->need_update = true;
            
        }
        else if (ImGui::IsKeyDown(ImGuiKey_LeftArrow)){
            Cam_Pos.x -= 0.5;
            op->need_update = true;
        }
        else if (ImGui::IsKeyDown(ImGuiKey_RightArrow)){
            Cam_Pos.x += 0.5;
            op->need_update = true;
        }
    }
    
    
    if(!m_io.WantCaptureMouse){
        if(ImGui::IsMouseDragging(0)){
            if(abs(m_io.MouseDelta.x) > abs(m_io.MouseDelta.y)){
                M_phi += 0.005*m_io.MouseDelta.x;
            }
            else{M_theta  += 0.005*m_io.MouseDelta.y;}
            op->need_update = true;
            
        }
    }
   
    
    
    ImGui::Text("Light Color:"); ImGui::SameLine(); ImGui::Text("Clear Color:");
    float w = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.y) * 0.40f;
    ImGui::SetNextItemWidth(w);
    if(ImGui::ColorPicker4("##MyColor##2", light_src.Get_Color_ptr(), ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha)){
        op->need_update = true;
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(w);
    ImGui::ColorPicker3("##MyColor##3", CC.get(), ImGuiColorEditFlags_NoInputs);


    static int div1 = 0;
    static int div2 = 0;
    ImGui::VSliderInt("theta", ImVec2(30, 100), &div1, 0, 24);
    ImGui::SameLine(100.0f);
    ImGui::VSliderInt("phi", ImVec2(30, 100), &div2, 0, 24);
    theta = PI*(div1/10.0);
    phi = PI*(div2/10.0);
    
    light_src.Set_Pos(theta, phi);
    AMD::Vec3 light = light_src.Get_Pos();
    ImGui::Text("Camera");
    ImGui::SameLine();
    ImGui::Text("theta = %f, phi = %f", C_theta, C_phi);
    ImGui::Text("theta = %f, phi = %f", theta, phi);
    ImGui::Text("Light Source Position");
    ImGui::SameLine();
    ImGui::Text("x = %.2f, y = %.2f, z = %.2f", light.x, light.y,light.z);
    ImGui::Text("Target");
    ImGui::SameLine();
    ImGui::Text("x = %.2f, y = %.2f, z = %.2f", light_src.Get_Target().x,light_src.Get_Target().z,light_src.Get_Target().z);
    ImGui::Text("Direction");
    ImGui::SameLine();
    ImGui::Text("x = %.2f, y = %.2f, z = %.2f", light_src.Get_Direction_vec().x,light_src.Get_Direction_vec().y,light_src.Get_Direction_vec().z);
    
    static float sx = 0.0;
    static float sy = 0.0;
    static float sz = 0.0;
    if(ImGui::VSliderFloat("Shift X", ImVec2(30, 100), &sx, 0.0, 1.0)){
        Simulation::Get()->shift.x = sx;
        Simulation::Get()->Step_Zero();
        timestep = Simulation::Get()->Timestep();
        
    }
    ImGui::SameLine(120.0f);
    if(ImGui::VSliderFloat("Shift Y", ImVec2(30, 100), &sy, 0.0, 1.0)){
        Simulation::Get()->shift.y = sy;
        Simulation::Get()->Step_Zero();
        timestep = Simulation::Get()->Timestep();
        
    }
    ImGui::SameLine(240.0f);
    if(ImGui::VSliderFloat("Shift z", ImVec2(30, 100), &sz, 0.0, 1.0)){
        Simulation::Get()->shift.z = sz;
        Simulation::Get()->Step_Zero();
        timestep = Simulation::Get()->Timestep();
        
    }
    //op.m_Cam.Look_At(AMD::Vec3(0.0,0.0,10.0));
    ImGui::InputFloat("Color Saturation", light_src.Get_Dir_ptr(), 0.01f, 1.0f, "%.2f");
    
    
        if (ImGui::Button("SAVE")){
            std::string dir = "/Users/diggs/Desktop/";
            std::string file = "volume-" + std::to_string(timestep) + ".png";
            std::string  save = dir + file;
            //save = true;
            Write_Buffer(save);
        }
    
    ImGui::SameLine(100.0);
    if(!ph.write)
    if (ImGui::Button("Write")){
        ph.write = true;
    }
    
    if(ph.write){
        if(ImGui::Button("Stop Write")){
            ph.write = false;
        }
    }
        
    ImGui::Text("%s","Play");
    if(!play){
        if(ImGui::Button(">")){
            play = true;
        }
    }
    if(play){
        if(ImGui::Button("||")){
            play = false;
        }
        if(!(counter%100)){
            Simulation::Get()->Step_Forward();
            timestep = Simulation::Get()->Timestep();
            counter = 0;
        }
        counter++;
    }
    
    ImGui::SameLine(100.0);
    if (ImGui::Button("Write Atoms")){
        ph.Write_Atoms();
    }
    
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Timestep");
    ImGui::SameLine();
    
    ImGui::PushID(0);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3.0 / 7.0f, 0.6f, 0.6f));
    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
        Simulation::Get()->Step_Bacward();
        timestep = Simulation::Get()->Timestep();
    }
    ImGui::PopStyleColor(1);
    ImGui::PopID();
    
    ImGui::SameLine(0.0f, spacing);
    
    ImGui::PushID(1);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3.0 / 7.0f, 0.6f, 0.6f));
    if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
        Simulation::Get()->Step_Forward();
        timestep = Simulation::Get()->Timestep();
    }
    ImGui::PopStyleColor(1);
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::Text(" %d",timestep);
    
    
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
    
    if(op->need_update){
        op->Set_W_Scale(Ww, Wh);
        op->m_Cam.Move_To(Cam_Pos);
        op->m_Cam.Look_UpDown(C_theta);
        op->m_Cam.Look_LeftRight(C_phi);
        op->m_model.Rotate_Pole(M_theta);
        op->m_model.Rotate_Azimuth(M_phi);
    }
}





