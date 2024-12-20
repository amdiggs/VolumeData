#SHADER CIRCLE
#version 330 core
layout (location = 0) out vec4 color;
in vec4 f_color;
in vec4 f_light;
in vec2 t_coords;
in float o_type;
in vec4 gl_FragCoord;
in vec3 f_pos;

uniform sampler2D u_Texture;
uniform sampler2D Axis_Texture;
uniform vec2 u_res;
vec4 t_color;
float fade = 0.005;
float sc;
vec4 temp_color;



void main()
{
 
    if(int(o_type) ==1){
        sc = 0.4;
        temp_color = vec4(0.8,0.3,0.0,1.0);
    }
    else{
        sc = 0.0;
        temp_color = f_color;
    }
    vec2 test = vec2(f_pos.x, f_pos.y);
    float dist = 1.0 - length(test);
    dist = smoothstep(sc , 0.6 + sc,dist);
    vec4 n_color = vec4(dist);
    if(n_color.a > 0.4){
        color = n_color;
        color*=temp_color;
    }
    else{discard;}
}
#END

