#ifndef WINDOW_HPP
#define WINDOW_HPP

/*****
 * @brief This is a function that will let the user create a window using GLFW.
 * @param width Width of the window.
 * @param height Height of the window.
 * @param window_name Name of the window.
 * 
 */
GLFWwindow* window_start_up(int width, int height, const char *window_name);

/*****
 * @brief 
 * This function will readjust the viewport when the window gets resized.
 * @param window GLFW window pointer.
 * @param width Window width.
 * @param height Window height.
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/*****
 * @brief 
 * Forever loop until user hits close on the window.
 * @param window A GLFW window pointer.
 */
void run_render_loop(GLFWwindow* window, unsigned int shader_program, unsigned int VAO);

/*****
 * @brief 
 * Processes input while window is open.
 * @param window A GLFW window.
 */
void process_input(GLFWwindow* window);
#endif // WINDOW_HPP