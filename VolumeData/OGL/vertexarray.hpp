//
//  vertexarray.hpp
//  OpenGL
//
//  Created by Andrew Diggs on 7/27/22.
//

#ifndef vertexarray_hpp
#define vertexarray_hpp

#include <stdio.h>

class VertexBuffer;
class VertexArray{
private:
    unsigned int m_ID;
    unsigned int count = 0;
public:
    VertexArray();
    ~VertexArray();
    
    void bind();
    void unbind();
    void Add_Static_Buffer(const void* data, unsigned int size, int num);
    void Add_Static_Instance_Buffer(const void* data, unsigned int size, int num);
    void Add_Vertex_Buffer(VertexBuffer& vb);
    void Add_Basic_Vertex_Buffer(VertexBuffer& vb);
    unsigned int Add_Dynamic_Buffer(unsigned int size);
    unsigned int Add_Dynamic_Instance_Buffer(unsigned int size);
    
};
#endif /* vertexarray_hpp */
