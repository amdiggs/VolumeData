#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_norm;
layout (location = 3) in vec3 v_offset;
layout (location = 4) in float v_type;


out vec3 f_norm;
out vec4 f_color;

uniform mat4 u_MVP;
uniform mat3 u_Normal;


vec4 colors[2] = vec4[2](vec4(0.2,0.2,1.0,1.0),vec4(0.9,0.1,0.1,1.0));

int type;

void main()
{

    type = int(v_type);
    vec3 new_pos = v_offset + v_pos;
    gl_Position = u_MVP * vec4(new_pos,1.0);
    f_norm = u_Normal * v_norm;
    f_color = colors[type];
    
    

    
}
#END


#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

in vec3 f_norm;
in vec4 f_color;


void main()
{
 
    color = f_color;
}

#END






/*
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 /////////////////////////////////////////////////////////////////////////////////////
 */
















