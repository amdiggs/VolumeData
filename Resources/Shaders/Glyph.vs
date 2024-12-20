#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec2 v_tex;



out vec4 f_color;
out vec2 t_coords;

uniform mat4 u_MVP;
uniform mat4 plane_rot;


void main()
{

    gl_Position = u_MVP * plane_rot * vec4(v_pos,1.0);
    f_color = v_color;
    t_coords = v_tex;
    
}
#END


#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

in vec4 f_color;
in vec2 t_coords;

//Texture Uniforms
uniform sampler2D Text_Tex;
vec4 t_color;


void main()
{
 
    t_color = vec4(1.0,1.0,1.0,texture(Text_Tex, t_coords).r);
    float cull = t_color.a;
    if(cull < 0.5){
        discard;
    }
    else{
        color = f_color;
    }
    
    
    
}

#END

/*
if(cull < 0.5){
    discard;
}
else{
    color = f_color;
}
*/
