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
    glDisable(GL_CULL_FACE);
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


void UI_Window::Simple_window(Light_Src& light_src, int m_w, int m_h){
    static float theta = 0.0;
    static float phi = 0.0;
    static float M_theta= 0.0;
    static float M_phi= 0.0;
    static float zoom = 1.0;
    
    static float C_theta = 0.0;
    static float C_phi = 0.0;
    static int timestep = 0;
    static AMD::Vec3 trans;
    static AMD::Vec3 look;
    static AMD::Vec3 Cam_Pos;
    const ImGuiKey m_keys[4] ={ImGuiKey_UpArrow, ImGuiKey_DownArrow, ImGuiKey_RightArrow, ImGuiKey_LeftArrow};
    
    ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));


    ImGui::Begin("UI prarmeters");
    //####These are the functions that act on the Operator!!##################################
    ImGui::SliderFloat("Zoom", &zoom, 0.0f, 2.0f);
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
            M_phi += 0.005*m_io.MouseDelta.x; M_theta  += 0.005*m_io.MouseDelta.y;
            op->need_update = true;
            
        }
    }
   
    
    
    ImGui::Text("Light Color:"); ImGui::SameLine(); ImGui::Text("Clear Color:");
    float w = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.y) * 0.40f;
    ImGui::SetNextItemWidth(w);
    ImGui::ColorPicker4("##MyColor##2", light_src.Get_Color_ptr(), ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(w);
    ImGui::ColorPicker3("##MyColor##3", CC.get(), ImGuiColorEditFlags_NoInputs);


    static int div1 = 0;
    static int div2 = 0;
    ImGui::VSliderInt("theta", ImVec2(30, 160), &div1, 0, 24);
    ImGui::SameLine();
    ImGui::VSliderInt("phi", ImVec2(30, 160), &div2, 0, 24);
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
    ImGui::Text("x = %f, y = %f, z = %f", light.x, light.y,light.z);
    ImGui::Text("Target");
    ImGui::SameLine();
    ImGui::Text("x = %f, y = %f, z = %f", light_src.Get_Target().x,light_src.Get_Target().z,light_src.Get_Target().z);
    ImGui::Text("Direction");
    ImGui::SameLine();
    ImGui::Text("x = %f, y = %f, z = %f", light_src.Get_Direction_vec().x,light_src.Get_Direction_vec().y,light_src.Get_Direction_vec().z);
    
    
    //op.m_Cam.Look_At(AMD::Vec3(0.0,0.0,10.0));
    ImGui::InputFloat("Color Saturation", light_src.Get_Dir_ptr(), 0.01f, 1.0f, "%.2f");
    
    
        if (ImGui::Button("SAVE")){
            save = true;
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


void UI_Window::log_window(Bond* bd, int num){
 
    if (ImGui::BeginTable("table1", 2, ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupColumn("Angles");
        ImGui::TableSetupColumn("Off Set");
        ImGui::TableHeadersRow();
        for (int row = 0; row < num; row++)
        {
            AMD::Vec3 temp1 = bd[row].get_angles();
            AMD::Vec3 temp2 = bd[row].get_off_set();
            ImGui::TableNextRow();
            

            ImGui::TableNextColumn();
            ImGui::Text("%.2f,%.2f, %.2f",temp1.x, temp1.y,temp1.z);
            
            ImGui::TableNextColumn();
            ImGui::Text("%.2f,%.2f, %.2f",temp2.x, temp2.y,temp2.z);
            
        }
        ImGui::EndTable();
    }
}

void UI_Window::log_window( AMD::Vertex* verts, int num){
    
    if (ImGui::BeginTable("table1", 4, ImGuiTableFlags_Hideable))
    {
        //ImGui::BeginTable("table1", 3, ImGuiTableFlags_Hideable);
        // Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each column.
        // (Later we will show how TableSetupColumn() has other uses, optional flags, sizing weight etc.)
        ImGui::TableSetupColumn("Coords");
        ImGui::TableSetupColumn("Color");
        ImGui::TableSetupColumn("Normal");
        ImGui::TableSetupColumn("Text Coords");
        ImGui::TableHeadersRow();
        for (int row = 0; row < num; row++)
        {
            ImGui::TableNextRow();
            

            ImGui::TableNextColumn();
            ImGui::Text("%.2f,%.2f, %.2f",verts[row].pos.x,verts[row].pos.y,verts[row].pos.z);
            
            ImGui::TableNextColumn();
            ImGui::Text("%.2f,%.2f, %.2f, %.2f",verts[row].clr.r,verts[row].clr.g,verts[row].clr.b, verts[row].clr.a);
            
            ImGui::TableNextColumn();
            ImGui::Text("%.2f,%.2f, %.2f",verts[row].norm.x,verts[row].norm.y,verts[row].norm.z);
            
            ImGui::TableNextColumn();
            ImGui::Text("%.2f %.2f", verts[row].texture.x,verts[row].texture.y );
        }
        ImGui::EndTable();
    }
}



void UI_Window::log_window( AMD::Vertex* verts, unsigned int* idx ,int num){
    int a, b, c;
    if (ImGui::BeginTable("table1", 3, ImGuiTableFlags_Hideable))
    {
        //ImGui::BeginTable("table1", 3, ImGuiTableFlags_Hideable);
        // Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each column.
        // (Later we will show how TableSetupColumn() has other uses, optional flags, sizing weight etc.)
        ImGui::TableSetupColumn("V1");
        ImGui::TableSetupColumn("V2");
        ImGui::TableSetupColumn("V3");
        ImGui::TableHeadersRow();
        for (int row = 0; row < num; row+=3)
        {
            ImGui::TableNextRow();
            a = idx[row]; b = idx[row + 1]; c = idx[row + 2];

            ImGui::TableNextColumn();
            ImGui::Text("%d: %.2f,%.2f, %.2f",a, verts[a].pos.x,verts[a].pos.y,verts[a].pos.z);
            
            ImGui::TableNextColumn();
            ImGui::Text("%d: %.2f,%.2f, %.2f",b, verts[b].pos.x,verts[b].pos.y,verts[b].pos.z);
            
            ImGui::TableNextColumn();
            ImGui::Text("%d: %.2f,%.2f, %.2f",c, verts[c].pos.x,verts[c].pos.y,verts[c].pos.z);

        }
        ImGui::EndTable();
    }
}


void UI_Window::log_window( AMD::Mat4 mat, bool T){
    if (ImGui::BeginTable("table1", 4, ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupColumn("1");
        ImGui::TableSetupColumn("2");
        ImGui::TableSetupColumn("3");
        ImGui::TableSetupColumn("4");
        ImGui::TableHeadersRow();
        for (int row = 0; row < 4; row++)
        {
            ImGui::TableNextRow();

            ImGui::TableNextRow();
            for (int column = 0; column < 4; column++)
            {
                ImGui::TableSetColumnIndex(column);
                if(T){
                    ImGui::Text("%f", mat[column][row]);
                }
                else{
                    ImGui::Text("%f", mat[row][column]);
                }
            }

        }
        ImGui::EndTable();
    }
}


void UI_Window::log_window(Hist_2D& hist){
    int rows = hist.Get_Rows(); int cols = hist.Get_Cols();
    float** vals = hist.Get_Counts();
    if (ImGui::BeginTable("Hist", cols + 1, ImGuiTableFlags_Hideable))
    {
        // set up cols
        for (int i = 0; i<cols + 1; i++){
            char buffer[100];
            std::snprintf(buffer,99, "(%.2f)" ,i*hist.m_bin_width_x);
            ImGui::TableSetupColumn(buffer);
        }
        ImGui::TableHeadersRow();
        for (int row = 0; row < rows ; row++)
        {
            ImGui::TableNextRow();

            ImGui::TableNextRow();
            char buffer[100];
            ImGui::TableSetColumnIndex(0);
            std::snprintf(buffer,99, "(%.2f)" ,(row+1)*hist.m_bin_width_z);
            ImGui::Text("%s",buffer);
            for (int col = 1; col < cols + 1; col++)
            {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%.2f", vals[row][col-1]);
                
            }

        }
        ImGui::EndTable();
    }
}




void UI_Window::log_window(int nebs[][2], int num){
    if (ImGui::BeginTable("table1", 1, ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupColumn("1");
        for (int row = 0; row <num; row++)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("i = %d, j= %d", nebs[row][0] +1, nebs[row][1] +1);

        }

        
        ImGui::EndTable();
    }
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









/*
 void Renderer::Draw_Instanced(IndexBuffer& ib, VertexArray& va, Shader& sh, GLenum draw_type, int num){
     glfwGetFramebufferSize(m_Window, &m_w, &m_h);
     glViewport(0, 0, m_w, m_h);
     glUniform2f(m_res_loc,float(m_w),float(m_h));
     sh.bind();
     va.bind();
     ib.bind();
     glDrawElementsInstanced(draw_type,ib.get_num(), GL_UNSIGNED_INT,0, num);
     
 }


 void Renderer::Draw(IndexBuffer& ib, VertexArray& va, Shader& sh, GLenum draw_type){
     glfwGetFramebufferSize(m_Window, &m_w, &m_h);
     glViewport(0, 0, m_w, m_h);
     sh.bind();
     va.bind();
     ib.bind();
     glDrawElements(draw_type,ib.get_num(), GL_UNSIGNED_INT, 0);
 }
 
 
 void Renderer::Write_Buffer(std::string file_name){
     int count = 0;
     int width, height;
     glfwGetFramebufferSize(m_Window, &width, &height);
     const int num_pix = 3 * width * height;
     unsigned char* pixels = new unsigned char[num_pix];
     
     std::fstream outfile;
     outfile.open(file_name, std::ios::out);
     
     glPixelStorei(GL_PACK_ALIGNMENT,1);
     glReadBuffer(GL_FRONT);
     glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
     
     if(outfile.is_open()){
         outfile << "P3\n" << width << " " << height << "\n255" << std::endl;
         for (int i = 0; i < width*height; i++){
             outfile << int(pixels[count]) << " "
             << int(pixels[count + 1]) << " "
             << int(pixels[count + 2]) << " " << std::endl;
             count+=3;
         }
     }
     else{
         glfw_error_callback(21, "go fuck youself");
     }
     
     outfile.close();
     exit(0);
     return;
     
 }

 void Renderer::Draw_ShadowMap(Quad_Mesh& qd, ShadowMap& sm){
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
     sm.Enable();
     sm.ReadBind(0);
     qd.Bind();
     glDisable(GL_DEPTH_TEST);
     glBindTexture(GL_TEXTURE_2D, sm.m_depth);
     glDrawElements(GL_TRIANGLES,qd.num_idx(), GL_UNSIGNED_INT, 0);
     
 }
 
 
 
 static void glfw_error_callback(int error, const char* description)
 {
     fprintf(stderr, "Glfw Error %d: %s\n", error, description);
 }
 */





