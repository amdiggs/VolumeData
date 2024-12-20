#pragma OPENCL EXTENSION cl_khr_fp64 : enable

struct atom{
    int m_id;
    int m_type;
    float3 m_coords;
};

float BW_Distance(float3 A, float3 B, float3 box){
    float dist_sq = 0.0;
    for (int i = 0; i< 3; i++){
        float delta = sqrt((A[i]-B[i])*(A[i]-B[i]));
        if(delta > 0.5*box[i]){
            float x1 = (A[i] < B[i]) ? A[i] : B[i];
            float x2 = (A[i] < B[i]) ? B[i] : A[i];
            dist_sq += ((x1 + box[i]) - x2) * ((x1 + box[i]) - x2);
        }
        else{dist_sq += delta * delta;}
    }
    
    return sqrt(dist_sq);
}


float3 Get_Bin_Widths(float3 b, int3 d){
    float dx = b.x / (float)d.x;
    float dy = b.y / (float)d.y;
    float dz = b.z / (float)d.z;
    
    return (float3)(dx,dy,dy);
}

__kernel void cl_Hist(__global struct atom* input,__global float* output, const int num_atoms, const int3 dims, const float3 box)
{
    int idx = get_global_id(0);
    int idy = get_global_id(1);
    int idz = get_global_id(2);
    float3 bw = Get_Bin_Widths(box, dims);
    float dx = (0.5 + idx)*bw.x;
    float dy = (0.5 + idy)*bw.y;
    float dz = (0.5 + idz)*bw.z;
    float3 center = (float3)(dx,dy,dz);
    
    float3 m_box = (float3)(box.x,box.y,10000.0);
    
    float num_si = 0;
    float num_ox = 0;
    float ratio = 0.0;
    float mod = 1.0;
    for(int i = 0; i < num_atoms; i++){
        float3 coords = input[i].m_coords;
        float dist = BW_Distance(center,coords,m_box);
        if(dist < 5.43){
            if(dist > 2.715){
                mod = 1.0 / dist;
                }
            else{mod = 1.0;}
            if(input[i].m_type == 1){num_si += mod;}
            else if(input[i].m_type == 2){num_ox += mod;}
            else if(input[i].m_type == 3){continue;}
            else{num_si+=10000.0;}
        }
    }
    
    if(num_si > 0){
        ratio = (float)num_ox / (float)num_si;
    }
    else{ratio = 2.0;}
    int out_id = idx + idy*dims.x + idz*(dims.x*dims.y);
    output[out_id] = ratio;
}

__kernel void Hist_Test(__global struct atom* input,__global float3* output)
{
    int g_id = get_global_id(0);
    
    output[g_id] = input[g_id].m_coords;
}

