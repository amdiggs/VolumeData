//
//  vertexarray.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 7/27/22.
//

#include "vertexarray.hpp"
#include "Render.hpp"
#include "AMDmath.hpp"
#include "vertexbuffer.hpp"

VertexArray::VertexArray()
:count(0){
    
    glGenVertexArrays(1, &m_ID);
}

VertexArray::~VertexArray(){
    
    glDeleteVertexArrays(1, &m_ID);
}


void VertexArray::Add_Static_Instance_Buffer( const void* data,unsigned int size, int num ){
    bind();
    unsigned int testVBO;
    int num_comp = size/sizeof(float);
    glGenBuffers(1, &testVBO);
    glBindBuffer(GL_ARRAY_BUFFER, testVBO);
    glBufferData(GL_ARRAY_BUFFER, num*size, (void*)data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(count);
    glVertexAttribPointer(count, num_comp, GL_FLOAT, GL_FALSE, size, (void*)0);
    glVertexAttribDivisor(count,1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    count++;
    
}


void VertexArray::Add_Static_Buffer( const void* data,unsigned int size, int num ){
    bind();
    unsigned int testVBO;
    int num_comp = size/sizeof(float);
    glGenBuffers(1, &testVBO);
    glBindBuffer(GL_ARRAY_BUFFER, testVBO);
    glBufferData(GL_ARRAY_BUFFER, num*size, (void*)data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(count);
    glVertexAttribPointer(count, num_comp, GL_FLOAT, GL_FALSE, size, (void*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    count++;
    
}

unsigned int VertexArray::Add_Dynamic_Buffer(unsigned int size){
    bind();
    unsigned int testVBO;
    int num_comp = size/sizeof(float);
    glGenBuffers(1, &testVBO);
    glBindBuffer(GL_ARRAY_BUFFER, testVBO);
    std::cout << count << std::endl;
    glEnableVertexAttribArray(count);
    glVertexAttribPointer(count, num_comp, GL_FLOAT, GL_FALSE, size, (void*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    count++;
    
    return testVBO;
}


unsigned int VertexArray::Add_Dynamic_Instance_Buffer(unsigned int size){
    bind();
    unsigned int testVBO;
    int num_comp = size/sizeof(float);
    glGenBuffers(1, &testVBO);
    glBindBuffer(GL_ARRAY_BUFFER, testVBO);
    glEnableVertexAttribArray(count);
    glVertexAttribPointer(count, num_comp, GL_FLOAT, GL_FALSE, size, (void*)0);
    glVertexAttribDivisor(count,1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    count++;
    
    return testVBO;
}



void VertexArray::Add_Vertex_Buffer(VertexBuffer& vb){
    bind();
    vb.bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AMD::Vertex_TX), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(AMD::Vertex_TX), (void*)offsetof(AMD::Vertex_TX, clr));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(AMD::Vertex_TX), (void*)offsetof(AMD::Vertex_TX, norm));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(AMD::Vertex_TX), (void*)offsetof(AMD::Vertex_TX,texture));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(AMD::Vertex_TX), (void*)offsetof(AMD::Vertex_TX,tex_indx));
    glBindVertexArray(0);
    count +=5;
}


void VertexArray::Add_Basic_Vertex_Buffer(VertexBuffer& vb){
    bind();
    vb.bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AMD::Vertex_Basic), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(AMD::Vertex_Basic), (void*)offsetof(AMD::Vertex_Basic, clr));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(AMD::Vertex_Basic), (void*)offsetof(AMD::Vertex_Basic, norm));
    glBindVertexArray(0);
    count +=3;
}



void VertexArray::bind(){
    
    glBindVertexArray(m_ID);
    
}

void VertexArray::unbind(){
    glBindVertexArray(0);
}
