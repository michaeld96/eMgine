#include "libraries.hpp"
#include "window.hpp"

std::string load_shader(const std:: string& filename)
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

int main()
{
    GLFWwindow* my_window = window_start_up(800, 600, "window_name_input");
    
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

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

    // Fragment shader compilation.
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    source = load_shader("../src/fragment-shader.frag");
    shader_source = source.c_str();
    glShaderSource(fragment_shader, 1, &shader_source, NULL);
    glCompileShader(fragment_shader);

    // Check to see if fragment shader was compiled OK.
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION::FAILED\n" << info << "\n";
    }

    // Creating a shader program.
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



    // // Drawing steps:
    // // 1. copy vertices array in a buffer for OpenGL to use.
    // // Making an instance of a vertex buffer object.
    // unsigned int VBO;
    // // Creating one buffer.
    // glGenBuffers(1, &VBO);
    // // Binding the buffer. GL_ARRAY_BUFFER is used for vertices.
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // // Copies vertex data into buffers memory.
    // // ARGS:    1. the typ eof buffer we want to copy data into.
    // //          2. the size we want to pass to the buffer.
    // //          3. the actual data.
    // //          4. tell the graphics card how we want it to manage the data.
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // // 2. set the vertex attributes pointers:
    // // Telling OpenGL how to interpret our vertices.
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // // Enable attributes. 
    // glEnableVertexAttribArray(0);
    // // 3. use shader program when we want to render a object.
    // // Active the program by calling the following function.
    // glUseProgram(shader_program);

    unsigned int vertex_buffer_object, vertex_array_object;
    glGenVertexArrays(1, &vertex_array_object);
    glGenBuffers(1, &vertex_buffer_object);
    
    // First bind the vertex array object then bind the vertex buffers, and the configure vertex attributes.
    glBindVertexArray(vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

    // glUseProgram(shader_program);


    run_render_loop(my_window, shader_program, vertex_array_object);
    return 0;
}