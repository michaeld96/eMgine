#ifndef SHADER_SET_UP
#define SHADER_SET_UP

#include "libraries.hpp"

std::string load_shader(const std::string& filename);

unsigned int init_vertex_shader(const std::string& _source);

unsigned int init_fragment_shader(const std::string& _source);

unsigned int init_shader_program(unsigned int vertex_shader, unsigned int fragment_shader);


#endif // SHADER_SET_UP