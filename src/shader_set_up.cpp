#include "shader_set_up.hpp"

std::string load_shader(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Cannot open shader file.\n" << std::strerror(errno) << "\n";
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
unsigned int init_vertex_shader(const std::string& _source)
{
    // Load in shader.
    std::string source =  load_shader(_source);
    const char* shader_source = source.c_str();

    // Creating the shader.
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    // Attaching the code for the shader and compiling.
    glShaderSource(vertex_shader, 1, &shader_source, NULL);
    glCompileShader(vertex_shader);

    // Checking to see if shader compiled.
    int success;
    char info[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION::FAILED\n" << info << "\n";
    }
    return vertex_shader;
}

unsigned int init_fragment_shader(const std::string& _source)
{
    int success;
    char info[512];
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Load in the source and compile the fragment shader source.
    std::string source = load_shader(_source);
    const char* shader_source = source.c_str();
    glShaderSource(fragment_shader, 1, &shader_source, NULL);
    glCompileShader(fragment_shader);

    // Check to see if fragment shader was compiled OK.
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION::FAILED\n" << info << "\n";
    }
    // Return the fragment shader.
    return fragment_shader;
}

unsigned int init_shader_program(unsigned int vertex_shader, unsigned int fragment_shader)
{
    // Creating a shader program.   
    int success;
    char info[512];
    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Check to see if the shader_program linked successfully.
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, info);
        std::cerr << "ERROR::SHADER_PROGRAM::LINKER::FAILED\n" << info << "\n";
    }

    // Delete the shader objects because we have linked!
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}
