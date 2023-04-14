//
//  vertexbuffer.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 7/14/22.
//

#include "vertexbuffer.hpp"
#include "Render.hpp"

VertexBuffer::VertexBuffer(){}

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    glGenBuffers(1, &render_ID);
    glBindBuffer(GL_ARRAY_BUFFER, render_ID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}


VertexBuffer::~VertexBuffer(){
    glDeleteBuffers(1, &render_ID);
}

void VertexBuffer::Gen_Buffer(const void* data, unsigned int size){
    glGenBuffers(1, &render_ID);
    glBindBuffer(GL_ARRAY_BUFFER, render_ID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    
}

void VertexBuffer::bind(){
    glBindBuffer(GL_ARRAY_BUFFER, render_ID);
    
}

void VertexBuffer::unbind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



unsigned int VertexBuffer::get_ID(){
    return render_ID;
}


IndexBuffer::IndexBuffer(){}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
:m_count(count)
{
    glGenBuffers(1, &render_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}


IndexBuffer::~IndexBuffer(){
    glDeleteBuffers(1, &render_ID);
}

void IndexBuffer::Gen_Buffer(const void* data, unsigned int count){
    this->m_count = count;
    glGenBuffers(1, &render_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
    unbind();
    
}

void IndexBuffer::bind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_ID);
    
}

void IndexBuffer::unbind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

unsigned int IndexBuffer::get_num(){
    return m_count;
}



















