#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_norm;
layout (location = 3) in vec2 v_tex;
layout (location = 4) in float v_indx;


uniform mat4 u_MVP;
out float t_index;
void main()
{

    
    gl_Position = vec4(v_pos,1.0);
    t_index = v_indx;
    vec4 junk = u_MVP * vec4(1.0,1.0,1.0,1.0);
    
    
}
#END


#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

in float t_index;

void main()
{
    float cmod = t_index/5.0;
    //color = vec4(1.0 - cmod,0.0,cmod,1.0);
    color = vec4(0.0,0.0,0.0,0.2);
}

#END

