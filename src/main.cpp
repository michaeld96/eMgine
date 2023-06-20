#include "libraries.hpp"
#include "window.hpp"

int main()
{
    GLFWwindow* my_window = window_start_up(800, 600, "hello");
    run_render_loop(my_window);
    return 0;
}