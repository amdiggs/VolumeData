#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;


uniform mat4 u_MVP;

void main()
{
    gl_Position = u_MVP * vec4(v_pos ,1.0);
    
}
#END
//#####################################################################################
#SHADER GEOMETRY
#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 4) out;


void main()
{

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    
    EndPrimitive();
}
#END

#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

uniform float u_a;

void main()
{
    float m_r = 1.0 - u_a;
    color = vec4(u_a,0.1,m_r,0.5);
}

