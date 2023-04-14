//
//  Logging.h
//  VolumeData
//
//  Created by Andrew Diggs on 3/16/23.
//

#ifndef Logging_h
#define Logging_h

#include <stdio.h>
#include <iostream>

#define OUT_OF_RANGE -10

template <typename T>
void Print_Value_T(T x){
    std::cout << x << std::endl;
    return;
}

template <typename T>
void Print_Vals_T(T vals, int num_vals){
    for (int i = 0; i< num_vals; i++){
        std::cout << vals[i] << std::endl;
    }
    return;
}

void print(bool x){Print_Value_T<bool>(x);}
void print(char x){Print_Value_T<char>(x);}
void print(char* x){Print_Value_T<char*>(x);}
void print(int x){Print_Value_T<int>(x);}
void print(float x){Print_Value_T<float>(x);}
void print(double x){Print_Value_T<double>(x);}

void print(bool* x, int num){Print_Vals_T<bool*>(x, num);}
void print(char* x, int num){Print_Vals_T<char*>(x, num);}
void print(char** x, int num){Print_Vals_T<char**>(x, num);}
void print(int* x, int num){Print_Vals_T<int*>(x, num);}
void print(float* x, int num){Print_Vals_T<float*>(x, num);}
void print(double* x, int num){Print_Vals_T<double*>(x, num);}
void print(float** x, int rows, int cols){

    for (int i = 0; i<rows; i++){
        for (int j = 0; j< cols; j++){
            std::cout << "---";
        }
        std::cout << "\n|";
        for (int j = 0; j< cols; j++){
            int count = x[i][j];
            if (count>9){std::cout << x[i][j] << "|";}
            else{std::cout << x[i][j] << " |";}
        }
        std::cout << "\n";
    }
    for (int j = 0; j< cols; j++){
        std::cout << "---";
    }
    std::cout << std::endl;
}


void ERROR_LOG(int err_value){
    switch (err_value) {
        case -10:
            print("INDEX OUT OF RANGE!!");
            exit(-10);
            break;
            
        default:
            break;
    }
}
/*
void ERROR(const char* message){
    std::cout << "Error!!! " << message << "\n";
    exit(10);
}
*/

#endif /* Logging_h */
