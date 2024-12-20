#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_norm;
layout (location = 3) in vec2 v_tex;
layout (location = 4) in float v_indx;
layout (location = 5) in vec3 off_set;
layout (location = 6) in vec4 inst_color;

out DATA
{
    vec4 f_color;
    vec3 f_norm;
    
}data_out;


uniform mat4 u_MVP;
uniform mat3 u_Normal;

vec4 Compute_Coords(vec3 a, vec3 off){
    float shift_x = a.x + off.x;
    float shift_z = a.z + off.z;
    float height = max((0.5*a.y*off.y),0.0);
    return vec4(shift_x,height,shift_z,1.0);
}

void main()
{
    vec4 inst_coords = Compute_Coords(v_pos,off_set);
    gl_Position = u_MVP * inst_coords;
    data_out.f_color = inst_color;
    data_out.f_norm = u_Normal * v_norm;
    
}
#END
//###########################################################################################################

//########################################################################################################

#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

in DATA
{
    vec4 f_color;
    vec3 f_norm;
}data_in;

void main()
{
 
    
    color = data_in.f_color;
}

#END

