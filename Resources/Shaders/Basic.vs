#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_norm;
layout (location = 3) in vec2 v_tex;
layout (location = 4) in float v_indx;




out DATA
{
    vec3 g_norm;
    vec4 g_color;
    
}data_out;

uniform mat4 u_MVP;
uniform mat3 u_Normal;

void main()
{

    gl_Position = u_MVP * vec4(v_pos ,1.0);
    data_out.g_norm = u_Normal * v_norm;
    data_out.g_color = v_color;
    vec3 junk = u_Normal * vec3(1.0,1.0,1.0);

    
}
#END
//#####################################################################################
#SHADER GEOMETRY
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

out vec4 f_color;
out vec3 f_norm;


vec4 NORMAL(vec4 a, vec4 b, vec4 c){
    vec3 v01 = vec3(normalize(b - a).xyz);
    vec3 v02 = vec3(normalize(c - a).xyz);
    return vec4(cross(v01,v02),0.0);
}



in DATA
{

    vec3 g_norm;
    vec4 g_color;
}data_in[];



void main()
{
    vec4 norm = NORMAL(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);
    vec4 tes0 = gl_in[1].gl_Position - gl_in[0].gl_Position - norm;
    vec4 tes1 = gl_in[2].gl_Position - gl_in[1].gl_Position - norm;
    vec4 tes2 = gl_in[0].gl_Position - gl_in[2].gl_Position - norm;
    vec4 tess[3] = vec4[](tes0, tes1, tes2);

    //gl_Position = gl_in[0].gl_Position;
    //f_color = data_in[0].g_color;
    //f_norm = data_in[0].g_norm;
    //EmitVertex();
    //gl_Position = gl_in[1].gl_Position;
    //f_color = data_in[1].g_color;
    //f_norm = data_in[0].g_norm;
    //EmitVertex();
    gl_Position = gl_in[0].gl_Position;
    f_color = data_in[0].g_color;
    f_norm = data_in[0].g_norm;
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + 0.5*tess[0];
    f_color = data_in[0].g_color;
    f_norm = data_in[0].g_norm;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + 0.5*tess[2];
    f_color = data_in[2].g_color;
    f_norm = data_in[2].g_norm;
    EmitVertex();
    
    gl_Position = gl_in[1].gl_Position + 0.5*tess[1];
    f_color = vec4(1.0,0.0,0.0,1.0);
    f_norm = data_in[1].g_norm;
    EmitVertex();
    
    gl_Position = gl_in[2].gl_Position;
    f_color = vec4(1.0,0.0,0.0,1.0);
    f_norm = data_in[2].g_norm;
    EmitVertex();
    
    EndPrimitive();
}
#END

#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

in vec4 f_color;
in vec3 f_norm;


//Light Uniforms Struct
struct Light_Source{
    vec4 clr;
    vec3 dir;
    float sat;
    
};

uniform Light_Source l_src;


void main()
{
 
    vec4 f_junk = l_src.clr + vec4(l_src.dir,l_src.sat);
    float l_dot = dot(l_src.dir,f_norm);
    color = f_color;
}
