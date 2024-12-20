#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 offset;

uniform mat4 u_MVP;


void main()
{

    vec4 pos = vec4(v_pos + offset,1.0);
    gl_Position = u_MVP * pos;
    
}
#END


#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

void main()
{
 
    color = vec4(0.2,0.2,0.2,1.0);
}

#END
