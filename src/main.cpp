#include "libraries.hpp"
#include "window.hpp"

int main()
{
    GLFWwindow* my_window = window_start_up(800, 600, "hello");
    run_render_loop(my_window);
    float verticies[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    // Making an instance of a vertex buffer object.
    unsigned int VBO;
    // Creating one buffer.
    glGenBuffers(1, &VBO);

    // Binding the buffer.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    return 0;
}