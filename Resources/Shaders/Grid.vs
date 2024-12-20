#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
//layout (location = 2) in vec3 v_norm;
//layout (location = 3) in vec2 v_tex;
//layout (location = 4) in float v_indx;
//layout (location = 5) in vec4 new_v_color;
//layout (location = 6) in float height;



out DATA
{
    vec4 g_color;
}data_out;



uniform mat4 u_MVP;
uniform mat3 u_Normal;

void main()
{

    gl_Position = u_MVP * vec4(v_pos, 1.0);
    data_out.g_color = v_color;
    
    vec3 junk = u_Normal * vec3(0.0,0.0,0.0);
    
}
#END
//#####################################################################################
#SHADER GEOMETRY
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

out vec4 f_color;

vec3 NORMAL(vec4 a, vec4 b, vec4 c){
    vec3 v01 = vec3(normalize(b - a).xyz);
    vec3 v02 = vec3(normalize(c - a).xyz);
    return cross(v01,v02);
}



in DATA
{
    vec4 g_color;
}data_in[];



void main()
{
    vec3 norm = NORMAL(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);
    vec3 tes0 = vec3((gl_in[1].gl_Position - gl_in[0].gl_Position).xyz) + 0.5*norm;
    vec3 tes1 = vec3((gl_in[2].gl_Position - gl_in[1].gl_Position).xyz) + 0.5*norm;
    vec3 tes2 = vec3((gl_in[0].gl_Position - gl_in[2].gl_Position).xyz) + 0.5*norm;
    vec3 tess[3] = vec3[](tes0, tes1, tes2);

    gl_Position = gl_in[0].gl_Position;
    f_color = data_in[0].g_color;
    EmitVertex();
    //gl_Position = gl_in[1].gl_Position;
    //f_color = data_in[1].g_color;
    //EmitVertex();
    //gl_Position = gl_in[2].gl_Position;
    //f_color = data_in[2].g_color;
    //EmitVertex();
    gl_Position = gl_in[0].gl_Position + 0.5*vec4(tess[0],gl_in[0].gl_Position.w);
    f_color = data_in[0].g_color;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position - 0.5*vec4(tess[2],gl_in[0].gl_Position.w);
    f_color = data_in[0].g_color;
    EmitVertex();
    
    
    
    EndPrimitive();
}
#END

#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

in vec4 f_color;

void main()
{
 
    
    color = f_color;
}

#END

