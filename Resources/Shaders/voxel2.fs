#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_norm;
layout (location = 3) in vec3 inst_off_set;
layout (location = 4) in float inst_weight;



out vec4 f_color;
out float density;

uniform mat4 u_MVP;
uniform vec3 dim;


void main()
{
    vec3 scaled = v_pos*dim;
    vec3 shifted = scaled + inst_off_set;
    gl_Position = u_MVP * vec4(shifted,1.0);
    f_color = v_color;
    density = inst_weight;
    
    
}
#END


#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

in vec4 f_color;
in float density;



void main()
{
    color = vec4(density,0.0,1.0 - density, 1.0);
    
}

#END

