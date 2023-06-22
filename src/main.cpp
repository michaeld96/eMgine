#include "libraries.hpp"
#include "window.hpp"

std::string load_shader(const std:: string& filename)
{
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main()
{
    GLFWwindow* my_window = window_start_up(800, 600, "hello");
    // run_render_loop(my_window);
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

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
    std::string s =  load_shader("vertex-shader.vert");
    return 0;
}