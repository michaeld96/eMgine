#include "libraries.hpp"


void init_all_buffers( unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, 
                       const int number_of_VAOs, const int number_of_VBOs, const int number_of_EBOs,
                       float *vertices, size_t vertices_size, unsigned int *indicies, 
                       size_t indicies_size, int attrib_starting_index,  int size_of_attrib);