//
//  vertexbuffer.hpp
//  OpenGL
//
//  Created by Andrew Diggs on 7/14/22.
//

#ifndef vertexbuffer_hpp
#define vertexbuffer_hpp

#include <stdio.h>

class VertexBuffer{
private:
    unsigned int render_ID;
    
public:
    VertexBuffer();
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();
    
    void Gen_Buffer(const void* data, unsigned int size);
    void bind();
    void unbind();
    unsigned int get_ID();
    
};


class IndexBuffer{
private:
    unsigned int render_ID;
    unsigned int m_count;
    
public:
    IndexBuffer();
    IndexBuffer(const unsigned int* data, unsigned int count);
    ~IndexBuffer();
    
    void Gen_Buffer(const void* data, unsigned int count);
    void bind();
    void unbind();
    unsigned int get_num();
    
};
    

#endif /* vertexbuffer_hpp */
