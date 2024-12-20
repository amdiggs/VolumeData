#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_norm;
layout (location = 3) in vec2 v_texCoords;
layout (location = 4) in float v_index;

out vec4 f_color;

uniform mat4 u_VP;
//For shadow map
uniform mat4 u_LightMVP;
out vec4 s_pos;


void main()
{

    
    gl_Position = u_VP * vec4(v_pos,1.0);
    f_color = v_color;
    s_pos = u_LightMVP * vec4(v_pos,1.0);

}
#END


#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 FragColor;

in vec4 f_color;
in vec4 s_pos;


uniform sampler2DShadow ShadowMap;


float Shadow_Factor(){
    vec3 proj_coords = s_pos.xyz/s_pos.w;
    vec3 UV_Coords;
    UV_Coords.x = 0.5*proj_coords.x + 0.5;
    UV_Coords.y = 0.5*proj_coords.y + 0.5;
    UV_Coords.z = 0.5*proj_coords.z + 0.5;
    //float Depth = texture(ShadowMap, UV_Coords).x;
    return 0.5*texture(ShadowMap, UV_Coords) +0.5;
    //float bias = 0.025;
    //if (Depth + bias < z){return 0.5;}
    //else{return 1.0;}
}

void main()
{
    FragColor = Shadow_Factor()*vec4(1.0, 0.0,0.0,1.0); //f_color;
}

#END


