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

void set_up_verticies(std::string file_path)
{
        // Making an instance of a vertex buffer object.
    unsigned int VBO;
    // Creating one buffer.
    glGenBuffers(1, &VBO);

    // Binding the buffer. GL_ARRAY_BUFFER is used for vertices.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copies vertex data into buffers memory.
    // ARGS:    1. the typ eof buffer we want to copy data into.
    //          2. the size we want to pass to the buffer.
    //          3. the actual data.
    //          4. tell the graphics card how we want it to manage the data.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Load in shader.
    std::string source =  load_shader("../src/vertex-shader.vert");
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
}