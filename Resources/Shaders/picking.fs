#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_norm;
layout (location = 3) in vec2 v_texCoords;
layout (location = 4) in float v_index;

out vec2 f_texCoords;
 
void main()
{

    
    gl_Position = vec4(v_pos,1.0);
    f_texCoords = v_texCoords;

}
#END


#SHADER FRAGMENT
#version 330 core

out vec4 FragColor;

in vec2 f_texCoords;

uniform sampler2D screenTexture;

void main()
{
    FragColor = vec4(1.0) - texture(screenTexture, f_texCoords);
    //FragColor = vec4(1.0,0.0,0.0,1.0);
}

#END

