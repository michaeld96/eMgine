#ifndef WINDOW_RENDERER_H
#define WINDOW_RENDERER_H
#include "Common.h"
#include "Helper.h"

class WindowRenderer
{
private:
    std::string window_name;
    int width;
    int height;
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint8 r, g, b, a = 255;
    glm::ivec2 camera;
    double _zoom_factor = 1.0;

public:
    WindowRenderer() : window_name(""), width(0), height(0) {}
    WindowRenderer(const std::string& in_window_name, 
                   int in_width, 
                   int in_height, 
                   Uint8 in_r, 
                   Uint8 in_g, 
                   Uint8 in_b, 
                   double in_zoom_factor) 
        : window_name(in_window_name), 
          width(in_width), 
          height(in_height),
          window(nullptr),
          renderer(nullptr),
          r(in_r),
          g(in_g),
          b(in_b),
          _zoom_factor(in_zoom_factor)
        //camera(0, 0) // TODO: need to change.
    {}
    ~WindowRenderer()
    {
        if (renderer != nullptr) 
        {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr; 
        }
        if (window != nullptr) 
        {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    }
    /**
     * @brief Initializes the window for the game. 
    */
    void init_window();
    void init_renderer();
    void clear_renderer();
    void set_zoom_factor(double in_zoom_factor);
    SDL_Renderer* get_renderer();
    void set_clear_color();
    void render_scene();
    SDL_Window* get_window();
    int get_view_width() const;
    int get_view_height() const;
    int get_camera_x() const;
    int get_camera_y() const;
    double get_zoom_factor() const;
    void init_editor_window();
    
};

#endif // WINDOW_RENDERER_H