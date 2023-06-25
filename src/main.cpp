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

void clean_up_objects(unsigned int VAO, unsigned int VBO, unsigned int EBO, unsigned int shader_program);

int main()
{
    GLFWwindow* my_window = window_start_up(800, 600, "window_name_input");
    
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    float square_vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
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

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glDrawElements(GL_TR)

    unsigned int vertex_buffer_object, vertex_array_object;
    glGenVertexArrays(1, &vertex_array_object);
    glGenBuffers(1, &vertex_buffer_object);

    // First bind the vertex array object then bind the vertex buffers, and the configure vertex attributes.
    glBindVertexArray(vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);

    unsigned int EBO; // Element Buffer Objects
    glGenBuffers(1, &EBO);

    // Bind EBO and copy indicies into the buffer.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // glUseProgram(shader_program);


    run_render_loop(my_window, shader_program, vertex_array_object);
    clean_up_objects(vertex_array_object, vertex_buffer_object, EBO, shader_program);
    return 0;
}

void clean_up_objects(unsigned int VAO, unsigned int VBO, unsigned int EBO, unsigned int shader_program)
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);
    glfwTerminate();
}