#include "libraries.hpp"
#include "window.hpp"

GLFWwindow* window_start_up(int width, int height, const char *window_name)
{
    // 1. Set up GLFW.
    glfwInit();
    // 2. Configure GLFW. 3.3, first digit is major, second is minor.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // 3. Setting the profile of OpenGL. 
    // First is the hint, and second is the value.
    // GLFW_OPENGL_CORE_PROFILE means only use the most recent functions.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 4. Set this to say we are only using OpenGL functions from 3.3 and above.
    //    Important for Apple since only 3.2 and afterward are supported.
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // 5. Creating a window object.
    GLFWwindow* window = glfwCreateWindow(width, height, window_name, NULL, NULL);
    // 6. Check for error.
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return NULL; 
    }

    // 7. Make this window current on the call thread.
    glfwMakeContextCurrent(window);

    // 8. If the window is resized it will redraw the window and make
    //    the rendering window the same size as the new window.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 9. Init GLAD loaders
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD.\n";
        return NULL;
    }
    return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Tell OpenGL the size of the rendering window.
    glViewport(0, 0, width, height);
}

void run_render_loop(GLFWwindow* window)
{
    while(!glfwWindowShouldClose(window))
    {
        process_input(window);
        
        glfwSwapBuffers(window);
        // Checks to see any events have been triggered, and if so
        // the window state will be updated.
        glfwPollEvents();
    }
    glfwTerminate();
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (((glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS) || glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}