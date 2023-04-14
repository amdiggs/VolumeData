#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_norm;
layout (location = 3) in vec2 v_tex;
layout (location = 4) in float v_indx;
layout (location = 5) in vec4 new_v_color;
layout (location = 6) in float height;

out DATA
{
    vec4 f_color;
    
}data_out;


uniform mat4 u_MVP;

void main()
{

    gl_Position = u_MVP * vec4(v_pos, 1.0);
    data_out.g_color = v_color;
    
}
#END
//###########################################################################################################
#SHADER GEOMETRY
#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 4) out;

out vec4 f_color;
out vec3 f_coords;



in DATA
{
    vec4 g_color;
}data_in[];



void main()
{
    

    vec4 off_set = vec4(2.5, 0.0, 0.0,0.0);
    gl_Position = gl_in[0].gl_Position;
    f_coords = gl_Position.xyz;
    f_color = vec4(1.0,0.0,0.0,1.0);
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + off_set;
    f_coords = gl_Position.xyz;
    f_color = data_in[0].g_color;
    EmitVertex();
    
    
    
    
    EndPrimitive();
}
#END
//########################################################################################################

#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

in vec4 f_color;
in vec3 f_coords;
in vec4 gl_FragCoord;
void main()
{
 
    vec2 UV = vec2(gl_FragCoord.x/3600.0,gl_FragCoord.x/2000.0);
    float dist = length(f_coords.xy - UV);
    
    color = vec4(UV.x, UV.y, 0.0,1.0);
}

#END

