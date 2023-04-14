//
//  Atomic.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 8/31/22.
//

#include "Atomic.hpp"
#include "FileIO.hpp"
#include "OGL/Meshes.hpp"
Simulation* Sim = Simulation::Get();

float Boundary_Wrapped_Dist(AMD::Vec3 A, AMD::Vec3 B){
    AMD::Vec3 box = Sim->Sim_Box();
    float dist_sq = 0.0;
    for (int i = 0; i< 3; i++){
        float delta = abs(A[i]-B[i]);
        if(delta > 0.5*box[i]){
            float x1 = (A[i] < B[i]) ? A[i] : B[i];
            float x2 = (A[i] < B[i]) ? B[i] : A[i];
            dist_sq += ((x1 + box[i]) - x2) * ((x1 + box[i]) - x2);
        }
        else{dist_sq += delta * delta;}
    }
    
    return sqrt(dist_sq);
}



Atom::Atom()
: m_id(10000000), m_type(0), m_coords(0.0,0.0,0.0), m_num_neighbors(0)
{
    m_neighbors = (Atom**)malloc(MAX_NEIGHBORS*sizeof(Atom*));
}

Atom::Atom(std::string line)
:m_num_neighbors(0)
{
    // LAMMPS dump looks like At.num At.type xs ys zs
    std::stringstream ss;
    float x, y, z;
    ss << line;
    ss >> m_id >> m_type >> x >> y >> z;
    m_coords = AMD::Vec3(x,y,z);
    m_neighbors = (Atom**)malloc(MAX_NEIGHBORS*sizeof(Atom*));
    
}

Atom::Atom(int _id, int _type, float x, float y, float z)
:m_id(_id), m_type(_type),m_coords(x, y, z), m_num_neighbors(0)
{
    m_neighbors = (Atom**)malloc(MAX_NEIGHBORS*sizeof(Atom*));
}

Atom& Atom::operator=(const Atom& other){
    if(this == &other){return *this;}
    
    this->m_id = other.m_id;
    this->m_type = other.m_type;
    this->m_coords = other.m_coords;
    this->m_num_neighbors = other.m_num_neighbors;
    
    this->m_neighbors[0] = other.m_neighbors[0];
    this->m_neighbors[1] = other.m_neighbors[1];
    this->m_neighbors[2] = other.m_neighbors[2];
    this->m_neighbors[3] = other.m_neighbors[3];
    this->m_neighbors[4] = other.m_neighbors[4];
    
    return *this;
}


Atom::~Atom() {
    free(m_neighbors);
}


AMD::Vec3& Atom::Get_Coords(){
    return this -> m_coords;
}
unsigned int Atom::Get_Type() const{
    return m_type;
}

unsigned int Atom::Get_ID() const{
    return m_id;
}


unsigned int Atom::Get_Num_Neighbors() const{
    return m_num_neighbors;
}

Atom** Atom::Get_Neighbors(){
    return this->m_neighbors;
}


void Atom::Set_Coords(float x, float y, float z){
    this->m_coords.x = x;
    this->m_coords.y = y;
    this->m_coords.z = z;
}


void Atom::Set_Vals(Atom_Line& line){
    AMD::Vec3 Box = Sim->Sim_Box();
    m_id = line.id;
    m_type = line.type;
    m_coords.x = line.coords.x * Box.x;
    m_coords.y = line.coords.y * Box.y;
    m_coords.z = line.coords.z * Box.z;
    Clear_Neighbors();
}

void Atom::Clear_Neighbors(){
    for (int i = 0; i< m_num_neighbors; i++){
        m_neighbors[i] = 0;
    }
    m_num_neighbors = 0;
}

void Atom::Push_Neighbor(Atom& neb){
    if (m_num_neighbors == MAX_NEIGHBORS){
        std::cout << "Exceeded Max Number of Neighbors!!!\n";
        Print_Neighbors();
        exit(-5);
    }
    this->m_neighbors[m_num_neighbors] = &neb;
    m_num_neighbors ++;
}


void Atom::Pop_Neighbor(Atom& neb){
    
}


void Atom::Rescale(){
    m_coords.x *= Sim->Sim_Box().x;
    m_coords.y *= Sim->Sim_Box().y;
    m_coords.z *= Sim->Sim_Box().z;
}

void Atom::Print(){
    std::cout << m_id << " " << m_type
    << " " << m_coords.x << " " << m_coords.y << " " << m_coords.z << std::endl;
}


void Atom::Print_Neighbors(){
    Print();
    for(int i = 0; i<m_num_neighbors; i++){
        m_neighbors[i] -> Print();
        std::cout << Boundary_Wrapped_Dist(this->Get_Coords(), m_neighbors[i]->Get_Coords()) << std::endl;
    }
    
}





//###################Bond class#####################

Bond::Bond(){}

Bond::Bond(Atom& A,Atom& B)
{
    m_start = A.Get_Coords();
    m_end = B.Get_Coords();
    m_vec = m_end - m_start;
    m_types.x = A.Get_Type();
    m_types.y = B.Get_Type();
    Set_Len();
    Set_Theta();
    Set_Phi();
    
    
}


Bond::~Bond() {}




void Bond::Set_Theta(){
    AMD::Vec3 z_hat = AMD::Vec3(0.0, 0.0, 1.0);
    m_ang.z = AMD::Get_angle(m_vec, z_hat);
    
}

void Bond::Set_Phi(){
    AMD::Vec3 y_hat = AMD::Vec3(0.0, -1.0, 0.0);
    AMD::Vec3 xy = AMD::Vec3(this->m_vec.x, this->m_vec.y, 0.0);
    float phi = AMD::Get_angle(xy, y_hat);
    if(this->m_vec.x < 0){
        m_ang.x = 6.283185307 - phi;
    }
    else{m_ang.x = phi;}
    m_ang.y = 0.0;
    return;
}

AMD::Vec3 Bond::get_off_set() {
    AMD::Vec3 temp = m_vec*0.5;
    return m_start + temp;
}

AMD::Vec3 &Bond::get_angles() {
    return this->m_ang;
}

AMD::Vec2 &Bond::get_types() {
    return this->m_types;
}

float Bond::get_len() { 
    return m_len;
}

void Bond::Set_Len() { 
    if((int)m_types.x == 2 && (int)m_types.y == 2){
        m_len = m_vec.len() - 1.5;
    }
    else{m_len = m_vec.len() - 0.8;}
    
    
}


Atom_Line::Atom_Line(std::string line)
{
    // LAMMPS dump looks like At.num At.type xs ys zs
    std::stringstream ss;
    float x, y, z;
    ss << line;
    ss >> id >> type >> x >> y >> z;
    coords = AMD::Vec3(x,y,z);
    
}





Dump::Dump()
:init(false), dump_num_atoms(0)
{}

Dump::~Dump(){
    if(init){
        free(Atom_Lines);
    }
}


void Dump::Init(std::ifstream& file_stream, size_t& pos){
    std::string line;
    std::stringstream ss;
    file_stream.seekg(pos);
    std::getline(file_stream, line);
    
    
    float lo = 0.0;float hi = 0.0;
    
    if(!match_TS(line.c_str())){
        printf("First Line did not match TIMESTEP:\n");
        exit(3);
    }
    else{
        std::getline(file_stream, line);
        timestep = get_int(line);
    }
    while(std::getline(file_stream, line)){
        if(match_TS(line.c_str())){
            pos = file_stream.tellg();
            pos -= line.length() + 1;
            file_stream.seekg(pos);
            break;
        }
        
        else if(match_Num_Atoms(line.c_str())){
            std::getline(file_stream, line);
            dump_num_atoms = get_int(line);
        }
        
        else if(match_Box_Bounds(line.c_str())){
            for(int i = 0; i< 3; i ++){
                std::getline(file_stream, line);
                ss << line;
                ss >> lo >> hi;
                sim_box[i][0] = lo;
                sim_box[i][1] = hi;
                ss.str("");
                ss.clear();
                }
            
        }// end of box else if
        
        else if(match_Atom_Line(line.c_str())){
            Atom_Lines = (Atom_Line*)malloc(dump_num_atoms*sizeof(Atom_Line));
            for(int i = 0; i< dump_num_atoms; i ++){
                std::getline(file_stream, line);
                Atom_Lines[i] = Atom_Line(line);
                }
            
        }// end of atom line else if
        
    }// end of while loop
    
    init = true;
    
}




Simulation::Simulation()
:m_num_blocks(0), m_curr_block(0),m_num_atoms(0), m_init(false) {}




Simulation::~Simulation()
{
    free(m_data);
}


Simulation Simulation::inst;

Simulation* Simulation::Get(){
    return &inst;
}






void Simulation::Init(const char* file){
    
    size_t pos = 0;
    std::ifstream infile(file, std::ios_base::in);
    if (!infile.is_open()){
        std::cout << "File did not open!!" << std::endl;
        exit(9);
    }
    
    int count = 0;
    while (!infile.eof()) {
        m_num_blocks ++;
        m_data = (Dump*)realloc(m_data, m_num_blocks*sizeof(Dump));
        m_data[count].Init(infile, pos);
        count++;
    }
    Set_Block(0);
    m_init = true;
    
}
void Simulation::Compute_Neighbors() { 
    float cut;
    int num_nebs;
    int count = 0;
    float dist;
    for (int i = 0; i< m_num_atoms; i++){
        AMD::Vec3 A = atoms[i].Get_Coords();
        num_nebs =0;
        if(A.x > 10000.0){continue;}
        for (int j = i+1; j< m_num_atoms; j++){
            AMD::Vec3 B = atoms[j].Get_Coords();
            if(B.x > 10000.0){continue;}
            dist = Boundary_Wrapped_Dist(A, B);
            switch (atoms[i].Get_Type() + atoms[j].Get_Type()) {
                case 2:
                    cut = cutoffs[0];
                    break;
                    
                case 3:
                    cut = cutoffs[1];
                    break;
                    
                case 4:
                    cut = cutoffs[2];
                    break;
                    
                default:
                    cut = 0.0;
                    break;
            }
            
            if(dist <= cut){
                neighbor_IDs[count][0] = i; neighbor_IDs[count][1] = j;
                atoms[i].Push_Neighbor(atoms[j]);
                atoms[j].Push_Neighbor(atoms[i]);
                count++;
                num_nebs++;
            }
        
        }
    }
    this -> m_num_bonds = count;
}



void Simulation::Set_Block(int block){
    if(block > m_num_blocks || block < 0){return;}
    
    m_timestep = m_data[block].timestep;
    m_num_atoms = m_data[block].dump_num_atoms;
    
    for (int i = 0; i< 3; i++){
        m_sim_box[i][0] = m_data[block].sim_box[i][0];
        m_sim_box[i][1] = m_data[block].sim_box[i][1];
    }
    
    
    
    for (int i = 0; i< m_num_atoms; i++){
        atoms[i]. Set_Vals(m_data[block].Atom_Lines[i]);
    }
    
    m_curr_block = block;
}


void Simulation::Update_Sim(char dir){
    switch (dir) {
        case 'f':
            Set_Block(m_curr_block + 1);
            break;
            
        case 'r':
            Set_Block(m_curr_block - 1);
            
        default:
            break;
    }
    //Compute_Neighbors();
    m_need_update = true;
}



bool Simulation::Is_Init(){
    return m_init;
}

int Simulation::Timestep(){
    return m_timestep;
}


int Simulation::Num_Atoms(){
    return this->m_num_atoms;
}

AMD::Vec3 Simulation::Sim_Box(){
    float x = m_sim_box[0][1] - m_sim_box[0][0];
    float y = m_sim_box[1][1] - m_sim_box[1][0];
    float z = m_sim_box[2][1] - m_sim_box[2][0];
    return AMD::Vec3(x, y, z);
}


Atom* Simulation::Atoms(){
    return atoms;
}

void Simulation::print(Atom_Attrib attrib){
    switch (attrib) {
        case COORDS:
            for (int i = 0; i< m_num_atoms; i++){
                atoms[i].Get_Coords().print();
                std::cout << "=======================================" << std::endl;
            }
            break;
        case TYPE:
            for (int i = 0; i< m_num_atoms; i++){
                std::cout << atoms[i].Get_Type() << std::endl;
                std::cout << "=======================================" << std::endl;
            }
            break;
            
        case AT_NEIGHBORS:
            for (int i = 0; i< m_num_atoms; i++){
                atoms[i].Print_Neighbors();
                std::cout << "=======================================" << std::endl;
            }
            break;
            
        case NEIGHBORS:
            for (int i = 0; i< m_num_bonds; i++){
                std::cout << neighbor_IDs[i][0] <<" -- " << neighbor_IDs[i][1] << std::endl;
                std::cout << "=======================================" << std::endl;
            }
            break;
        default:
            break;
    }
    
    
}










