#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_norm;
layout (location = 3) in vec2 v_tex;
layout (location = 4) in float indx;
layout (location = 5) in vec3 v_os;
layout (location = 6) in vec3 v_ang;
layout (location = 7) in vec2 v_types;
layout (location = 8) in float v_len;

out vec4 f_color;
out vec4 f_light;
out vec2 t_coords;
out float l_dot;
out float t_index;
out vec3 f_pos;
out vec2 f_types;


uniform mat4 u_Model;
uniform mat4 u_rot;
uniform mat4 u_View;
uniform mat4 u_Proj;

uniform vec4 l_color;
uniform vec3 l_src;


float DOT_PROD(vec3 a, vec3 b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

mat4 ROTATION(vec3 ang){
    float a = ang.x;
    float b = ang.y;
    float c = ang.z;
    
    vec4 c0 = vec4(cos(a)*cos(b),sin(a)*cos(b) , -sin(b) , 0.0);
    vec4 c1 = vec4(cos(a)*sin(b)*sin(c) - sin(a)*cos(c), sin(a)*sin(b)*sin(c) + cos(a)*cos(c), cos(b)*sin(c), 0.0);
    vec4 c2 = vec4(cos(a)*sin(b)*cos(c)+sin(a)*sin(c), sin(a)*sin(b)*cos(c)-cos(a)*sin(c), cos(b)*cos(c), 0.0);
    vec4 c3 = vec4(0.0, 0.0, 0.0, 1.0);
    return mat4(c0,c1,c2,c3);
}

void main()
{
    
    float z_scale = v_len/2.5;
    mat4 ob_rot = ROTATION(v_ang);
    gl_Position = u_Proj * u_View * u_Model *  (vec4(v_os,0.0) + (ob_rot * vec4(v_pos.x*0.2, v_pos.y*0.2, v_pos.z*z_scale,1.0)));
    vec4 n_norm = u_rot * vec4(v_norm, 1.0);
    l_dot = DOT_PROD(vec3(n_norm),normalize(l_src));
    f_color = v_color;
    t_coords = v_tex;
    f_light = l_dot*l_color;
    t_index = indx;
    f_pos = v_pos;
    f_types = v_types;
    
}
#END



#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;
in vec4 f_color;
in vec4 f_light;
in vec2 t_coords;
in float t_index;
in vec3 f_pos;
in vec2 f_types;

uniform sampler2D u_Tex;
vec4 t_color;

vec4 type_color(float typ){
    if(int(typ) == 2){
        return vec4(0.4,0.4,0.4,1.0);
    }
    else{return vec4(0.9,0.2,0.2,1.0);}
    
}




void main()
{
    t_color = texture(u_Tex, t_coords);
    vec4 m_color = type_color(f_types.x)*(0.5 - f_pos.z/3.0) + (0.5 + f_pos.z/3.0)*type_color(f_types.y);
    color = vec4(m_color.xyz,1.0);
}

#END

#SHADER GEOMETRY
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 f_pos;
out vec3 f_norm;
out vec3 f_cam_dir;
out vec4 f_color;

 
out vec2 f_t_coords;
out float f_t_index;


in DATA
{

    vec3 g_pos;
    vec3 g_norm;
    vec3 g_cam_dir;
    vec4 g_color;

    vec2 g_t_coords;
    float g_t_index;
}data_in[];



void main()
{
    //vec3 tmp = normalize(
    for(int i=0; i<3; i++)
    {
      gl_Position = gl_in[i].gl_Position;
        f_pos = data_in[i].g_pos;
        f_norm = data_in[i].g_norm;
        f_cam_dir = data_in[i].g_cam_dir;
        f_color = data_in[i].g_color;
        f_t_coords = data_in[i].g_t_coords;
        f_t_index = data_in[i].g_t_index;
      EmitVertex();
    }
    EndPrimitive();
}
#END
