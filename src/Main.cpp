#include "Common.h"
#include "MapHelper.h"
#include "Emgine.h"
#include "ConfigManager.h"
#include "SceneManager.h"
#include "WindowRenderer.h"
#include "ImageManager.h"
#include "TextManager.h"
#include "AudioManager.h"
#include <SDL2/include/SDL.h>
#include <rapidjson/document.h>
#include "InputManager.h"
#include "ComponentManager.h"
#include "Camera.h"
#include "PhysicsManager.h"
#include "EventBus.h"
#include "Editor.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

void init_sdl();

int main(int argc, char* argv[])
{
    /**************** START: GLOBAL INITS ****************/
    init_sdl();
    InputManager::init();
    TextManager::load_text(); // misleading name, inits TTF
    ComponentManager::init_lua_state();
    ComponentManager::init_functions();
    /**************** END:   GLOBAL INITS ****************/
    /**************** START: EDITOR ****************/
editor:
    WindowRenderer* editor_window = new WindowRenderer("eMgine Editor", 1920, 1080, 0, 0, 0, 1.0);
    editor_window->init_editor_window();
    editor_window->init_renderer();
    Editor::load_game_config();
    Editor::load_rendering_config();
    Editor::load_scenes();
    Editor::load_templates();
    Editor::_build_scene_ = false;
    Editor::init_imgui(editor_window->get_window(), editor_window->get_renderer());
    int keep_running = Editor::im_gui_loop(editor_window);
    // quit the main loop if we exit the editor.
    if (keep_running == 0)
    {
        Editor::shutdown_imgui();
        Editor::destroy_window(editor_window);
        SDL_Quit();
        return 0;
    }
    // Need to close down window to have the context of new window to be smooth.
    Editor::clean_up();
    Editor::shutdown_imgui();
    Editor::destroy_window(editor_window);
    /**************** END:   EDITOR ****************/

    /**************** START: GAME CONFIG ****************/
    // InputManager::init();
    ConfigManager::check_resources_folder(); // makes sure that resources exists.
    ConfigManager::check_game_config_file(); // makes sure that game_config exists.
    ConfigManager::set_up_rendering_config(); // grabs settings from json file.
    ConfigManager::set_window_name(); // grabs windows name from json file.

    AudioManager::init();

    WindowRenderer* window_class = new WindowRenderer(ConfigManager::get_window_name(), 
                                                ConfigManager::get_x_resolution(), 
                                                ConfigManager::get_y_resolution(),
                                                ConfigManager::get_clear_color_r(),
                                                ConfigManager::get_clear_color_g(),
                                                ConfigManager::get_clear_color_b(),
                                                ConfigManager::get_zoom_factor());

    // init window and renderer for game.
    window_class->init_window();
    window_class->init_renderer();

    window_class->set_zoom_factor(ConfigManager::get_zoom_factor());

    window_class->set_clear_color();
    
    ComponentManager::init_components();
    /**************** END: GAME CONFIG ****************/
    
    /**************** START: GAME INIT ****************/
    PhysicsManager::Init();
    
    SceneManager::load_initial_scene(); // load in actor data here and the first scene.

    // init engine.
    Emgine game;

    /**************** END: GAME INIT ****************/

    /**************** START: GAME LOOP ****************/
    // clear renderer before game.
    window_class->clear_renderer();
    // TODO: COULD I REMOVE THESE SET UPS?
    if (!AudioManager::intro_bgm.empty())
    {
        AudioManager::play_intro_music();
    }
    if (ImageManager::intro_images.empty())
    {
        game.set_game_state(GameState::RUNNING);
    }

    // load only when new scene starts.
    game.run_onstart_for_all_actors();
    while(game.get_game_state() != GameState::QUIT)
    {
        game.process_input(window_class->get_renderer());
        {
            SDL_SetRenderDrawColor(window_class->get_renderer(), 
                                      ConfigManager::get_clear_color_r(), 
                                      ConfigManager::get_clear_color_g(), 
                                      ConfigManager::get_clear_color_b(), 
                                      255);
            SDL_RenderClear(window_class->get_renderer());
            // add new actors
            if (Helper::GetFrameNumber() > 0)
            {
                game.add_new_actors();
            }            
            // add new components to the actor.
            game.process_added_components();
            // calls the update component on all actors.
            game.onupdate_for_all_actors();
            // calls the late update component on all actors.
            game.onlateupdate_for_all_actors();
            // process the subscriptions for the event bus.
            EventBus::UpdateSubscriptions();
            // remove components from the actor.
            game.process_removed_components();
            // destroy actors that need to be destroyed.
            game.actors_that_need_to_be_destroyed();
            // now we will run the physics step.
            // physics_manager.WorldStep();
            PhysicsManager::WorldStep();
            // ************ START RENDERING ************
            SDL_RenderSetScale(window_class->get_renderer(), Camera::GetZoom(), Camera::GetZoom());
            // draw scene images
            ImageManager::RenderImageRequests(window_class->get_renderer(), window_class, window_class->get_view_width(), window_class->get_view_height());
            // reset render scale for UI
            SDL_RenderSetScale(window_class->get_renderer(), 1.0f, 1.0f);
            // render UI
            ImageManager::RenderUIRequest(window_class->get_renderer());
            // render text
            TextManager::RenderTextRequest(window_class->get_renderer(), window_class);
            // render the pixel
            ImageManager::RenderPixelRequests(window_class->get_renderer(), window_class);
            if (SceneManager::new_scene_requested)
            {
                // destroy all actors from old scene that dont have dont_destroy set to true.
                game.actors_that_need_to_be_destroyed();
                // load new scene.
                SceneManager::load_scene(SceneManager::new_scene_name);
                SceneManager::new_scene_requested = false;
                // run onstart for all loaded actors
                game.run_onstart_for_all_actors();
            }
        }
        Helper::SDL_RenderPresent498(window_class->get_renderer());
        // last update for input manager.
        InputManager::last_update();
    }
    /**************** END: GAME LOOP ****************/
    // clean up
    // delete renderer
    AudioManager::halt_audio();
    AudioManager::clean_up();
    SceneManager::clean_up();
    ImageManager::clean_up();
    
    delete window_class;
    EventBus::clean_up();
    ComponentManager::cleanup(); // clears the component tables.
    goto editor;
    ComponentManager::close_lua_state(); // closes the lua state.
    SDL_Quit();
    return 0;
}

void init_sdl()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(0);
    }
}