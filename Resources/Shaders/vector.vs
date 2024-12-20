#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;

out DATA
{
    vec3 g_pos;
    
}data_out;

uniform mat4 u_MVP;



void main()
{

    gl_Position = u_MVP * vec4(v_pos,1.0);
    data_out.g_pos = v_pos;
}
#END

#SHADER GEOMETRY
#version 330 core
layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

in DATA
{

    vec3 g_pos;
}data_in[];



vec4 OFFSET(){
    vec3 dir = data_in[1].g_pos - data_in[0].g_pos;
    float x = abs(dir.x);
    float y = abs(dir.y);
    float z = abs(dir.z);
    
    if(x > 0.1){
        return vec4(0.0,0.15,0.0,0.0);
    }
    else{
        return vec4(0.15,0.0,0.0,0.0);
    }
}

vec4 OFFSET2(){
    vec3 dir = data_in[1].g_pos - data_in[0].g_pos;
    float x = abs(dir.x);
    float y = abs(dir.y);
    float z = abs(dir.z);
    
    if(x > 0.1){
        return vec4(0.15,0.0,0.0,0.0);
    }
    else{
        return vec4(0.0,0.0,0.0,0.0);
    }
}


void main()
{
    vec4 offset = OFFSET();
    vec4 offset2 = OFFSET2();
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + offset;
    EmitVertex();
    
    gl_Position = gl_in[1].gl_Position + offset2;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + offset + offset2;
    EmitVertex();
    EndPrimitive();
}
#END



#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;


void main()
{
 
    color = vec4(0.0,0.0,0.0,1.0);
}

#END
