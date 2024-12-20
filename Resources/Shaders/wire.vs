#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;


out vec3 f_pos;
out float sc;
uniform mat4 u_MVP;
uniform float clr_scale;
vec4 new_pos;

void main()
{
    if(clr_scale < 0.9){
        new_pos = vec4(v_pos.x,v_pos.y,v_pos.z + 0.01,1.0);
    }
    else{
        new_pos = vec4(v_pos,1.0);
    }
    gl_Position = u_MVP * new_pos;
    f_pos = v_pos;
    sc = clr_scale;
    
}
#END


#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

in vec3 f_pos;
in float sc;


float Comp_Red(){
    float fr = f_pos.z/30.0;
    fr *= 10.0;
    float r2 = floor(fr);
    return r2 /10.0;
}

float WHITE(float r){
    float x0 = r - 0.4;
    float arg = -50.0*x0*x0;
    return exp(arg);
}

void main()
{
    float mr = Comp_Red();
    float mb = 1.0 - mr;
    float mg = 0.0; // smoothstep(0.0,0.4,mr);
    float w = WHITE(mr);
    vec4 tmp_clr;
    if(sc > 0.5){
        tmp_clr = vec4(mr , mb, w, 1.0);
    }
    else{
        tmp_clr = vec4(0.1, 0.1, 0.1, 1.0);
    }
    color = tmp_clr;
}

#END

