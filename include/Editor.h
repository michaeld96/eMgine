#ifndef Editor_H
#define Editor_H
#include "Common.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "Helper.h"
#include "WindowRenderer.h"
#include "ConfigManager.h"
#include "../libs/ImGuiFileDialog/ImGuiFileDialog.h"
#include <variant>

struct GameConfig
{
    std::vector<char> game_title;
    std::vector<char> initial_scene;

    GameConfig()
    {
        game_title.resize(256);
        initial_scene.resize(256);
    }
};

namespace EditorNameSpace // these structs will be serialized to json.
{
    struct Component
    {
        std::string type;
        std::vector<std::pair<std::string, std::string>> overrides;
        char new_override_key[256] = "";
        char new_override_value[256] = "";
        // constructor for component
        Component(std::string type)
        {
            this->type = type;
        }
    };
    struct Actor
    {
        std::string name;
        std::vector<Component> components;
        // constructor for actor 
        Actor(std::string name)
        {
            this->name = name;
        }
    };
    struct Scene
    {
        std::string name;
        std::vector<Actor> actors;
        // constructor for scene
        Scene(std::string name, std::vector<Actor> actors)
        {
            this->name = name;
            this->actors = actors;
        }
    };
}

struct RenderingConfig
{
    Uint8 clear_color_r;
    Uint8 clear_color_g;
    Uint8 clear_color_b;
    int x_resolution;
    int y_resolution;

    RenderingConfig()
    {
        clear_color_r = 255;
        clear_color_g = 255;
        clear_color_b = 255;
        x_resolution = 640;
        y_resolution = 360;
    }
};

class Editor
{
private:
    static inline GameConfig game_config;
    static inline RenderingConfig rendering_config;

    static inline std::vector<EditorNameSpace::Scene> scenes;
    static inline std::vector<EditorNameSpace::Actor> templates;

    static inline bool shown_initial_scene_warning = false;
    static inline bool valid_initial_scene = false;
    static inline SDL_Texture* scene_preview = nullptr;
    static inline bool _lua_editor_open_ = false;
    // static inline std::string lua_code_buffer = "";
    static inline std::vector<char> lua_code_buffer;
    static inline bool lua_code_buffer_full = false;
    
    static inline bool _pop_up_warning_empty_scene_name_ = false;
    static inline bool _pop_up_warning_empty_actor_name_ = false;
    static inline bool _pop_up_warning_empty_component_name_ = false;
    static inline bool _pop_up_warning_empty_component_value_ = false;
    static inline bool _pop_up_warning_empty_override_key_ = false;

    static inline bool _template_pop_up_empty_template_name_ = false;
    static inline bool _template_pop_up_empty_actor_name_ = false;
    static inline bool _template_pop_up_empty_component_name_ = false;
    static inline bool _template_pop_up_empty_override_key_ = false;
    static inline bool _template_pop_up_empty_override_value_ = false;
public:
    // helpers
    static std::string get_initial_scene_from_game_config();
    static std::string char_arr_to_string(const char* char_arr);

    static inline bool _build_scene_ = false;
    static inline std::string _current_scene_rendered_ = "";
    static bool is_valid_initial_scene();
    // init imgui
    static void init_imgui(SDL_Window* window, SDL_Renderer* renderer);
    // input loop through the editor interface.
    static int im_gui_loop(WindowRenderer* editor_window);
    static void shutdown_imgui();
    static void destroy_window(WindowRenderer* editor_window);

    // for game.config
    static void load_game_config();
    static void save_game_config();

    // for rendering.config
    static void load_rendering_config();
    static void save_rendering_config();
    
    static void make_pop_up_warning(const char* popup_window_name, const char* popup_message, bool& show_warning);
    static void make_pop_up_warning(const char* popup_window_name, const char* popup_message);
    
    // file explorer (loading in files and what not.)
    static void draw_load_file();
    static void draw_remove_file();
    static void copy_image_to_resources(const char* file_path);
    static void copy_audio_to_resources(const char* file_path);
    static void copy_font_to_resources(const char* file_path);
    static void copy_lua_file_to_resources(const char* file_path);
    static void copy_scene_to_resources(const char* file_path);
    
    static void draw_asset_explorer();

    // scene editor
    static void scene_editor();
    static void load_scenes();
    static void save_scene(const EditorNameSpace::Scene& scene);

    // template editor (super similar to scene editor)
    static void template_editor();
    static void save_templates();
    static void load_templates();
    static void copy_template_to_resources(const char* file_path);

    // scene preview
    static void init_scene_preview(WindowRenderer* editor_window);
    static void show_scene_drop_down();

    // lua editor
    // static int open_lua_editor(const std::string& lua_file_path);
    static void open_lua_editor(const std::string& lua_file_path);

    // cleaning
    static void clean_up();
};

#endif // Editor_H