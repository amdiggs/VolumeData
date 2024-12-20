#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_norm;
layout (location = 3) in vec3 v_offset;
layout (location = 4) in float v_type;


out vec3 f_norm;
out vec4 f_color;
out vec4 f_pos;
uniform mat4 u_MVP;
uniform mat3 u_Normal;


vec4 colors[4] = vec4[4](vec4(0.85,0.75,0.75,1.0),vec4(1.0,0.85,0.4,1.0),vec4(1.0,0.85,0.4,1.0),vec4(0.15,0.95,0.31,1.0));
float sizes[4] = float[4](0.75, 0.5, 0.5, 0.95);

int at_type;
float size;

void main()
{

    at_type = int(v_type);
    size = sizes[at_type - 1];
    vec3 new_pos = v_offset + size*v_pos;
    gl_Position =  u_MVP * vec4(new_pos,1.0);
    f_norm = u_Normal * v_norm;
    f_color = colors[at_type - 1];
    f_pos =  u_MVP * vec4(new_pos,1.0);
    
    

    
}
#END

/////////////////////////////////////////////////////////////////////////////////////

#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;


in vec3 f_norm;
in vec4 f_color;
in vec4 f_pos;
//Light Uniforms Struct
struct Light_Source{
    vec4 clr;
    vec3 dir;
    float sat;
    
};

uniform Light_Source l_src;

float Compute_Fog(vec4 pos){
    //float dist = abs(-65.0 - (pos.z/pos.w));
    // pos min = 40.0 pos max = 75.0 ?
    float dist = (pos.z - 40.0) / 35.0;
    return  1.0 - smoothstep(0.0,0.3,dist);
}

vec4 Fog_Color = vec4(1.0,1.0,1.0,0.5);
void main()
{
    float l_dot = 0.7*(dot(-l_src.dir,f_norm) + 1.0);
    float ma = f_color.a;
    color = l_dot*f_color;
    color.a = 1.0;
    
    
    
}

#END



temp_clr.a = 1.0;
float ff = Compute_Fog(f_pos);
temp_clr = mix(Fog_Color, temp_clr, ff);
color = temp_clr;
