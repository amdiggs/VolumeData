#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
//layout (location = 2) in vec3 v_norm;
//layout (location = 3) in vec2 v_tex;
//layout (location = 4) in float v_indx;
//layout (location = 5) in vec4 new_v_color;
//layout (location = 6) in float height;



out vec4 f_color;

uniform mat4 u_MVP;
uniform mat3 u_Normal;
uniform float color_mod;

void main()
{

    gl_Position = u_MVP * vec4(v_pos, 1.0);
    f_color = color_mod * v_color;
    
    vec3 junk = u_Normal * vec3(0.0,0.0,0.0);
    
}
#END


#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

in vec4 f_color;

void main()
{
 
    
    color = vec4(f_color.xyz, 1.0);
}

#END

