#include "libraries.hpp"
#include "window.hpp"
#include "shader_set_up.hpp"
#include "buffer.hpp"

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

    unsigned int vertex_shader = init_vertex_shader("../src/shaders/vertex-shader.vert");

    unsigned int fragment_shader = init_fragment_shader("../src/shaders/fragment-shader.frag");

    unsigned int shader_program = init_shader_program(vertex_shader, fragment_shader);

    unsigned int VAO, VBO, EBO;
    
    // For rectangle.
    init_all_buffers(&VAO, &VBO, &EBO, 1, 1, 1, square_vertices, sizeof(square_vertices), indices, sizeof(indices), 0, 3);

    // For triangle.
    // init_all_buffers(&VAO, &VBO, nullptr, 1, 1, 0, vertices, sizeof(vertices), nullptr, 0, 0, 3);

    run_render_loop(my_window, shader_program, VAO, &EBO);
    clean_up_objects(VAO, VBO, 0, shader_program);
    return 0;
}

void clean_up_objects(unsigned int VAO, unsigned int VBO, unsigned int EBO, unsigned int shader_program)
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    if (EBO != 0)
    {
        glDeleteBuffers(1, &EBO);
    }
    glDeleteProgram(shader_program);
    glfwTerminate();
}