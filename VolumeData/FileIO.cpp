//
//  FileIO.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 8/21/22.
//

#include "FileIO.hpp"

namespace fs = std::filesystem;

std::string m_dig = "[[:digit:]]+";
std::string m_float = "[[:digit:]]+\\.[[:digit:]]+";
std::string m_space = "[[:blank:]]";
std::string m_exp_float = "[[:digit:]]+\\.[[:digit:]]+e\\+[[:digit:]]+";
std::string m_comment = "^#.*";
std::string dump_file = ".dump";
std::string dat_file = ".dat";

bool match(std::string input, std::string m_type){
    std::regex reg (m_type);
    return std::regex_match(input,reg);
}

bool search(std::string input, std::string m_type){
    std::regex reg (m_type);
    return std::regex_search(input,reg);
}

bool match_int(std::string input){
    return match(input, m_dig);
}
bool match_float(std::string input){
    return match(input, m_float) || match(input, m_exp_float);
    
}
bool match_space(std::string input){
    return match(input, m_space);
}


bool match_comment(std::string input){
    return  match(input, m_comment);
}

bool match_atom_file(std::string input){
    return search(input, dump_file) || search(input, dat_file);
}



bool match_TS(const char* line){return std::regex_match(line,re_TS);}
bool match_Num_Atoms(const char* line){return std::regex_match(line,re_NA);}
bool match_Box_Bounds(const char* line){return std::regex_match(line,re_BB);}
bool match_Atom_Line(const char* line){return std::regex_match(line,re_AL);}






int get_int(std::string int_str)
{
    if(match_int(int_str)){
        return std::atoi(int_str.c_str());
    }
    else{std::cout << int_str << " is not compatable with type int"  << std::endl; exit(2);}
    
}

float get_float(std::string fl_str)
{
    
 if(match_float(fl_str)){
     return std::atoi(fl_str.c_str());
 }
 else{std::cout << fl_str << " is not compatable with type float" << std::endl; exit(2);}
}



unsigned int Hash(const char* word){
    unsigned int hash_int = 0;
    for (int i = 0; i<std::strlen(word); i++){
        hash_int+=word[i];
    }
    return hash_int;
}


bool ITEM(std::string str, std::string reg_ex)
{
    std::regex reg(reg_ex);
    std::smatch m;
    std::regex_search(str, m, reg);
    return !m.empty();
}


char* Read_Line(std::ifstream& in_file){
    char* line = nullptr;
    char temp[100];
    in_file.getline(temp, 100);
    int num_c = (int)in_file.gcount();
    line = (char*)malloc(num_c*sizeof(char));
    for(int i = 0; i< num_c; i++){
        line[i] = temp[i];
    }
    return line;
}


char** Read_File(const char* file,int& num_blocks, int* block_idx, int& num_lines){
    char** contents = nullptr;
    std::ifstream infile(file, std::ios_base::in);
    int count = 0;
    if (!infile.is_open()){
        std::cout << "File did not open!!" << std::endl;
        exit(9);
    }
    
    int ts_count = 0;
    std::regex ts(TIMESTEP);
    while (!infile.eof()){
        count ++;
        contents = (char**)realloc(contents, count*sizeof(char*));
        contents[count - 1] = Read_Line(infile);
        if(std::regex_match(contents[count - 1], ts)){
            count ++;
            contents = (char**)realloc(contents, count*sizeof(char*));
            contents[count - 1] = Read_Line(infile);
            block_idx[ts_count] = get_int(contents[count - 1]);
            ts_count++;
        }
    }// while
    num_lines = count;
    num_blocks = ts_count;
    infile.close();
    return contents;
}





