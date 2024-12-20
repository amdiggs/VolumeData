#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_clr;


uniform mat4 u_MVP;
uniform mat4 u_MLPV;
out VS_OUT{
    vec4 gs_pos;
    vec4 gs_clr;
} vs_out;

void main()
{
    vec4 junk = u_MVP * vec4(1.0);
    gl_Position = u_MVP * vec4(v_pos ,1.0);
    vs_out.gs_pos = u_MLPV * vec4(v_pos ,1.0);
    vs_out.gs_clr = v_clr;
    
}
#END
//#####################################################################################
#SHADER GEOMETRY
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 fs_norm;
out vec4 fs_pos;
out vec4 fs_clr;
in VS_OUT{
    vec4 gs_pos;
    vec4 gs_clr;
}gs_in[];

vec3 NORMAL(vec4 a, vec4 b, vec4 c){
    vec3 v01 = vec3(normalize(b - a).xyz);
    vec3 v02 = vec3(normalize(c - a).xyz);
    return cross(v02,v01);
}


void main()
{
    vec3 norm = NORMAL(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);

    gl_Position = gl_in[0].gl_Position;
    fs_pos = gs_in[0].gs_pos;
    fs_norm = norm;
    fs_clr = gs_in[2].gs_clr;
    EmitVertex();
    
    gl_Position = gl_in[1].gl_Position;
    fs_pos = gs_in[1].gs_pos;
    fs_norm = norm;
    fs_clr = gs_in[2].gs_clr;
    EmitVertex();
    
    gl_Position = gl_in[2].gl_Position;
    fs_pos = gs_in[2].gs_pos;
    fs_norm = norm;
    fs_clr = gs_in[2].gs_clr;
    EmitVertex();
    
    
    EndPrimitive();
}
#END

#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

in vec3 fs_norm;
in vec4 fs_pos;
in vec4 fs_clr;

//Light Uniforms Struct
struct Light_Source{
    vec4 clr;
    vec3 dir;
    float sat;
    
};

uniform Light_Source l_src;
uniform sampler2DShadow ShadowMap;
float Shadow_Factor(){
    float bias = 0.01*(1.0 - abs(dot(l_src.dir,fs_norm)));
    vec3 proj_coords = fs_pos.xyz/fs_pos.w;
    vec3 UV_Coords;
    UV_Coords.x = 0.5*proj_coords.x + 0.5;
    UV_Coords.y = 0.5*proj_coords.y + 0.5;
    UV_Coords.z = 0.5*(proj_coords.z - bias) + 0.5;
    return 0.3*texture(ShadowMap, UV_Coords) + 0.7;

}


void main()
{
 
    float sf = Shadow_Factor();
    vec4 f_junk = l_src.clr + vec4(l_src.dir,l_src.sat);
    float l_dot = 0.7*abs(dot(l_src.dir,fs_norm)) + 0.5;
    color = fs_clr;
}


