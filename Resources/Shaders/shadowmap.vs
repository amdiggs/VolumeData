#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;

uniform mat4 u_MLP;
 
void main()
{

    
    gl_Position = u_MLP*vec4(v_pos,1.0);

}
#END


#SHADER FRAGMENT
#version 330 core
void main()
{
    
}

#END

