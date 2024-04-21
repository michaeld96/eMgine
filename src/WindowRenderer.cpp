#include "WindowRenderer.h"

void WindowRenderer::init_window()
{
    // create the sdl window.
    SDL_Window* window = Helper::SDL_CreateWindow498(window_name.c_str(), 
                                          SDL_WINDOWPOS_CENTERED, 
                                          SDL_WINDOWPOS_CENTERED, 
                                          width, 
                                          height, 
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(0);
    }
    this->window = window;
}

void WindowRenderer::init_editor_window()
{
    // create the sdl window.
    SDL_Window* window = Helper::SDL_CreateWindow498(window_name.c_str(), 
                                          SDL_WINDOWPOS_CENTERED, 
                                          SDL_WINDOWPOS_CENTERED, 
                                          width, 
                                          height, 
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(0);
    }
    this->window = window;
}

void WindowRenderer::init_renderer()
{
    SDL_Renderer* renderer = Helper::SDL_CreateRenderer498(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(this->window);
        SDL_Quit();
        exit(0);
    }
    this->renderer = renderer;
}

void WindowRenderer::set_clear_color()
{
    // set the clear color.
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void WindowRenderer::render_scene()
{
    SDL_RenderClear(renderer);
    Helper::SDL_RenderPresent498(renderer);
}

void WindowRenderer::clear_renderer()
{
    SDL_RenderClear(renderer);
}

SDL_Renderer* WindowRenderer::get_renderer()
{
    return renderer;
}

SDL_Window* WindowRenderer::get_window()
{
    return window;
}

int WindowRenderer::get_view_width() const
{
    return width;
}

int WindowRenderer::get_view_height() const
{
    return height;
}

void WindowRenderer::set_zoom_factor(double in_zoom_factor)
{
	SDL_RenderSetScale(renderer, in_zoom_factor, in_zoom_factor);
}

double WindowRenderer::get_zoom_factor() const
{
    return _zoom_factor;
}


// int WindowRenderer::get_camera_x() const
// {
//     return camera.x;
// }

// int WindowRenderer::get_camera_y() const
// {
//     return camera.y;
// }