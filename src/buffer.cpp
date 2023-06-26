#include "buffer.hpp"

void init_all_buffers( unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, 
                       const int number_of_VAOs, const int number_of_VBOs, const int number_of_EBOs,
                       float *vertices, size_t vertices_size, unsigned int *indicies, 
                       size_t indicies_size, int attrib_starting_index,  int size_of_attrib)
{
    bool is_null_EBO = true;
    // Generate VAO and Buffers.
    glGenVertexArrays(number_of_VAOs, VAO);
    glGenBuffers(number_of_VBOs, VBO);
    if (EBO != nullptr)
    {
        glGenBuffers(number_of_EBOs, EBO);
        is_null_EBO = false;
    }

    // First bind VAO, then bind and set vertex buffers, and then configure 
    // vertex attributes.
    glBindVertexArray(*VAO);

    // Bind VBO and copy data.
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
    if (!is_null_EBO)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies_size, indicies, GL_STATIC_DRAW);
    }

    // Define where the attributes start.
    glVertexAttribPointer(attrib_starting_index, size_of_attrib, GL_FLOAT, GL_FALSE, size_of_attrib * sizeof(float), (void*)0);
    glEnableVertexAttribArray(attrib_starting_index);
    
    // Unbinds the VBO because we don't need to modify it anymore.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // unbind VAO because we are done modifying it.
    glBindVertexArray(0);
}