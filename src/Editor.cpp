#include "Editor.h"
#include "InputManager.h"
#include "Emgine.h"
#include "SceneManager.h"
#include "ComponentManager.h"
#include "EventBus.h"
#include "Camera.h"
#include "TextManager.h"
#include "AudioManager.h"
#include "PhysicsManager.h"
#include <typeinfo>

// helper 
bool is_bool(const std::string& str);



void Editor::init_imgui(SDL_Window* window, SDL_Renderer* renderer)
{
    // init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // set up imgui style.
    ImGui::StyleColorsDark();
    // set up rendering bindings
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

int Editor::im_gui_loop(WindowRenderer* editor_window)
{
    bool game_engine_off = true;
    static bool need_to_show_warning_popup_initial_scene = false;
    static bool shown_empty_game_title_warning = false;
    static bool shown_empty_initial_scene_warning = false;
    static bool shown_how_to = false;
    while (game_engine_off)
    {
        SDL_Event event;
        while (Helper::SDL_PollEvent498(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
            {
                return 0; // this will tell the editor to shut down and exit.
            }
        }
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // editor interface.
        ImGui::Begin("Game Loop");
            // add editor wigit.
            if (ImGui::Button("Start Game"))
            {
                valid_initial_scene = is_valid_initial_scene();
                if (valid_initial_scene)
                {
                    game_engine_off = false;
                    ComponentManager::cleanup();
                    SceneManager::clean_up();
                    ImageManager::clean_up();
                    // if the file search is open, close it
                    // if (ImGuiFileDialog::Instance()->IsOpened("LoadFileKey"))
                    // {
                    //     ImGuiFileDialog::Instance()->Close();
                    //     return 1;
                    // }
                    // if (ImGuiFileDialog::Instance()->IsOpened("RemoveFileKey"))
                    // {
                    //     ImGuiFileDialog::Instance()->Close();
                    //     return 1;
                    // }
                }
                else
                {
                    ImGui::OpenPopup("Not Valid Initial Scene");
                    need_to_show_warning_popup_initial_scene = true;
                }
            }
            if (need_to_show_warning_popup_initial_scene)
            {
                ImGui::SetNextWindowSize(ImVec2(500, 100));
                if (ImGui::BeginPopupModal("Not Valid Initial Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Please specify an initial scene in the game configuration.");
                    if (ImGui::Button("OK"))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
        ImGui::End();

        ImGui::Begin("Game Configuration");
            ImGui::InputText("Game Title", game_config.game_title.data(), game_config.game_title.size());
            ImGui::InputText("Initial Scene", game_config.initial_scene.data(), game_config.initial_scene.size());
            if (ImGui::Button("Save"))
            {
                // check to see if the game title and initial scene are not empty.
                if (game_config.game_title[0] == '\0')
                {
                    // ImGui::OpenPopup("Empty Game Title");
                    shown_empty_game_title_warning = true;
                }
                else if (game_config.initial_scene[0] == '\0')
                {
                    // ImGui::OpenPopup("Empty Game Title");
                    shown_empty_initial_scene_warning = true;
                }
                else 
                {
                    // reshape the vector to remove any extra characters.
                    game_config.game_title.resize(strlen(game_config.game_title.data()));
                    game_config.initial_scene.resize(strlen(game_config.initial_scene.data()));
                    save_game_config();
                    // resize again so that the vector is back to its original size.
                    game_config.game_title.resize(256);
                    game_config.initial_scene.resize(256);
                }
            }
            make_pop_up_warning("Empty Game Title", "Please specify a game title.", shown_empty_game_title_warning);
            make_pop_up_warning("Empty Initial Scene", "Please specify an initial scene.", shown_empty_initial_scene_warning);
        ImGui::End(); // Game Configuration

        ImGui::Begin("Rendering Configuration");
            ImGui::InputInt("X Resolution (Must be a value greater than 0)", &rendering_config.x_resolution);
            ImGui::InputInt("Y Resolution (Must be a value greater than 0)", &rendering_config.y_resolution);
            ImGui::InputScalar("Clear Color R (Must be a value of [0-255])", ImGuiDataType_U8, &rendering_config.clear_color_r);
            ImGui::InputScalar("Clear Color G (Must be a value of [0-255])", ImGuiDataType_U8, &rendering_config.clear_color_g);
            ImGui::InputScalar("Clear Color B (Must be a value of [0-255])", ImGuiDataType_U8, &rendering_config.clear_color_b);
            if (rendering_config.x_resolution <= 0)
            {
                rendering_config.x_resolution = 1;
            }
            if (rendering_config.y_resolution <= 0)
            {
                rendering_config.y_resolution = 1;
            }
            if (ImGui::Button("Save"))
            {
                save_rendering_config();
            }
        ImGui::End(); // Rendering Configuration

        ImGui::Begin("File Explorer");
            if (ImGui::Button("Load File"))
            {
                ImGuiFileDialog::Instance()->OpenDialog("LoadFileKey", "Choose File", ".png,.lua,.wav,.ttf,.scene, .template", {"resources/"});
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove File"))
            {
                ImGuiFileDialog::Instance()->OpenDialog("RemoveFileKey", "Choose File", ".png,.lua,.wav,.ttf,.scene, .template", {"resources/"});
            }
            ImGui::SameLine();
            if (ImGui::Button("How To"))
            {
                ImGui::OpenPopup("How To");
                shown_how_to = true;
            }
            make_pop_up_warning("How To", 
            "To add images to your game, click on the 'Open File Explorer' button and select a .png file. For Lua files, click on any file that ends in .Lua. Text fonts, .ttf extensions. Audio files, .wav extension.", shown_how_to);
        ImGui::End();

        draw_load_file();

        draw_remove_file();

        draw_asset_explorer();

        scene_editor();

        template_editor();

        /********** START: PREVIEW WINDOW **********/
        ImGui::Begin("Editor Tab");

        
            ImVec2 window_size = {
                static_cast<float>(rendering_config.x_resolution), 
                static_cast<float>(rendering_config.y_resolution)
            };

            if (is_valid_initial_scene())
            {
                init_scene_preview(editor_window); 
                ImGui::Image((void*)scene_preview, window_size);    
            }
            else
            {
                SDL_Texture* temp = SDL_CreateTexture(editor_window->get_renderer(), 
                                      SDL_PIXELFORMAT_RGBA8888, 
                                      SDL_TEXTUREACCESS_TARGET, 
                                      rendering_config.x_resolution, 
                                      rendering_config.y_resolution);
                SDL_SetRenderTarget(editor_window->get_renderer(), temp);
                // render the clear color to the window.
                SDL_SetRenderDrawColor(editor_window->get_renderer(), rendering_config.clear_color_r, rendering_config.clear_color_g, rendering_config.clear_color_b, 255);
                SDL_RenderClear(editor_window->get_renderer());
                // set render target back to NULL
                SDL_SetRenderTarget(editor_window->get_renderer(), NULL);
                ImGui::Image((void*)temp, window_size);
            }
        ImGui::End();
        /********** END: PREVIEW WINDOW **********/

        // render the editor's interface.
        ImGui::Render();
        SDL_SetRenderDrawColor(editor_window->get_renderer(), 0, 0, 0, 255);
        SDL_RenderClear(editor_window->get_renderer());
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        Helper::SDL_RenderPresent498(editor_window->get_renderer());
    }
    return 1; // return 1 to indicate that the game engine is on.
}

void Editor::shutdown_imgui()
{
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui::DestroyContext();
}

void Editor::destroy_window(WindowRenderer* editor_window)
{
    SDL_DestroyRenderer(editor_window->get_renderer());
    SDL_DestroyWindow(editor_window->get_window());
    delete editor_window;
}

void Editor::load_game_config()
{
    // first see if resources folder exists. If not, create it.
    if (!std::filesystem::exists("resources"))
    {
        std::filesystem::create_directory("resources");
    }
    // now see if game.config exists, if it does, populate the game_config struct.
    if (std::filesystem::exists("resources/game.config"))
    {
        rapidjson::Document document;
        ConfigManager::read_json_file("resources/game.config", document);
        if (document.HasMember("game_title") && document["game_title"].IsString() && document["game_title"].GetStringLength() > 0)
        {
            std::string game_title = document["game_title"].GetString();
            std::copy(game_title.begin(), game_title.end(), game_config.game_title.begin());
        }
        if (document.HasMember("initial_scene") && document["initial_scene"].IsString() && document["initial_scene"].GetStringLength() > 0)
        {
            std::string initial_scene = document["initial_scene"].GetString();
            std::copy(initial_scene.begin(), initial_scene.end(), game_config.initial_scene.begin());
        }
    }
}

void Editor::save_game_config()
{
    // creating json document.
    rapidjson::Document document;
    document.SetObject();

    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value game_title;
    game_title.SetString(game_config.game_title.data(), game_config.game_title.size(), allocator);
    document.AddMember("game_title", game_title, allocator);
    rapidjson::Value initial_scene;
    initial_scene.SetString(game_config.initial_scene.data(), game_config.initial_scene.size(), allocator);
    document.AddMember("initial_scene", initial_scene, allocator);
    ConfigManager::write_json_file("resources/game.config", document);
}

void Editor::load_rendering_config()
{
    // check to see if the file exists.
    const std::filesystem::path game_config_path = "resources/rendering.config";
    rapidjson::Document document;
    if (std::filesystem::exists(game_config_path))
    {
        ConfigManager::read_json_file("resources/rendering.config", document);
        if (document.HasMember("clear_color_r"))
        {
            rendering_config.clear_color_r = document["clear_color_r"].GetUint();
        }
        if (document.HasMember("clear_color_g"))
        {
            rendering_config.clear_color_g = document["clear_color_g"].GetUint();
        }
        if (document.HasMember("clear_color_b"))
        {
            rendering_config.clear_color_b = document["clear_color_b"].GetUint();
        }
        if (document.HasMember("x_resolution"))
        {
            rendering_config.x_resolution = document["x_resolution"].GetInt();
        }
        if (document.HasMember("y_resolution"))
        {
            rendering_config.y_resolution = document["y_resolution"].GetInt();
        }
    }
}

void Editor::save_rendering_config()
{
    // creating json document.
    rapidjson::Document document;
    document.SetObject();

    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("clear_color_r", rendering_config.clear_color_r, allocator);
    document.AddMember("clear_color_g", rendering_config.clear_color_g, allocator);
    document.AddMember("clear_color_b", rendering_config.clear_color_b, allocator);
    document.AddMember("x_resolution", rendering_config.x_resolution, allocator);
    document.AddMember("y_resolution", rendering_config.y_resolution, allocator);
    ConfigManager::write_json_file("resources/rendering.config", document);
}

void Editor::make_pop_up_warning(const char* popup_window_name, const char* popup_message, bool& show_warning)
{
    if (show_warning)
    {
        ImGui::OpenPopup(popup_window_name);
        if (ImGui::BeginPopupModal(popup_window_name, NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", popup_message);
            if (ImGui::Button("OK"))
            {
                show_warning = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void Editor::make_pop_up_warning(const char* popup_window_name, const char* popup_message)
{
    ImGui::OpenPopup(popup_window_name);
    if (ImGui::BeginPopupModal(popup_window_name, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("%s", popup_message);
        if (ImGui::Button("OK"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Editor::draw_load_file()
{
    if(ImGuiFileDialog::Instance()->Display("LoadFileKey"))
    {
        if(ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // get extension of file
            std::string extension = filePathName.substr(filePathName.find_last_of(".") + 1);
            // going to copy the image to "resources/images"
            if (extension == "png")
            {
                copy_image_to_resources(filePathName.c_str());
            }
            else if (extension == "wav")
            {
                copy_audio_to_resources(filePathName.c_str());
            }
            else if (extension == "ttf")
            {
                copy_font_to_resources(filePathName.c_str());
            }
            else if (extension == "lua")
            {
                copy_lua_file_to_resources(filePathName.c_str());
            }
            else if (extension == "scene")
            {
                copy_scene_to_resources(filePathName.c_str());
            }
            else if (extension == "template")
            {
                copy_template_to_resources(filePathName.c_str());
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void Editor::copy_image_to_resources(const char* file_path)
{
    // get the file name.
    std::string file_name = std::filesystem::path(file_path).filename().string();

    if (!std::filesystem::exists("resources/images"))
    {
        std::filesystem::create_directory("resources/images");
    }
    // copy the file to resources/images 
    std::filesystem::copy(file_path, "resources/images/" + file_name, std::filesystem::copy_options::overwrite_existing);
}

void Editor::copy_audio_to_resources(const char* file_path)
{
    // get the file name.
    std::string file_name = std::filesystem::path(file_path).filename().string();

    if (!std::filesystem::exists("resources/audio"))
    {
        std::filesystem::create_directory("resources/audio");
    }
    // copy the file to resources/audio 
    std::filesystem::copy(file_path, "resources/audio/" + file_name, std::filesystem::copy_options::overwrite_existing);
}

void Editor::copy_font_to_resources(const char* file_path)
{
    // get the file name.
    std::string file_name = std::filesystem::path(file_path).filename().string();

    if (!std::filesystem::exists("resources/fonts"))
    {
        std::filesystem::create_directory("resources/fonts");
    }
    // copy the file to resources/fonts 
    std::filesystem::copy(file_path, "resources/fonts/" + file_name, std::filesystem::copy_options::overwrite_existing);
}

void Editor::copy_lua_file_to_resources(const char* file_path)
{
    // get the file name.
    std::string file_name = std::filesystem::path(file_path).filename().string();

    if (!std::filesystem::exists("resources/component_types"))
    {
        std::filesystem::create_directory("resources/component_types");
    }
    // copy the file to resources/lua 
    std::filesystem::copy(file_path, "resources/component_types/" + file_name, std::filesystem::copy_options::overwrite_existing);
}

void Editor::copy_scene_to_resources(const char* file_path)
{
    // get the file name.
    std::string file_name = std::filesystem::path(file_path).filename().string();

    if (!std::filesystem::exists("resources/scenes"))
    {
        std::filesystem::create_directory("resources/scenes");
    }
    // copy the file to resources/scenes 
    std::filesystem::copy(file_path, "resources/scenes/" + file_name, std::filesystem::copy_options::overwrite_existing);
    // reload the scenes
    load_scenes();
}

void Editor::copy_template_to_resources(const char* file_path)
{
    // get the file name.
    std::string file_name = std::filesystem::path(file_path).filename().string();

    if (!std::filesystem::exists("resources/actor_templates"))
    {
        std::filesystem::create_directory("resources/actor_templates");
    }
    // copy the file to resources/actor_templates 
    std::filesystem::copy(file_path, "resources/actor_templates/" + file_name, std::filesystem::copy_options::overwrite_existing);
    load_templates();
}

void Editor::draw_asset_explorer()
{
    ImGui::Begin("Asset Explorer");
        // get all the files in the resources folder.
        for (const auto& entry : std::filesystem::directory_iterator("resources"))
        {
            std::string file_name = entry.path().filename().string();
            if (file_name == "game.config" || file_name == "rendering.config")
            {
                ImGui::Text("%s", file_name.c_str());
            }
            else if (file_name == "images" || file_name == "audio" || file_name == "fonts" || file_name == "component_types" || file_name == "scenes" || file_name == "actor_templates")
            {
                ImGui::Text("%s", file_name.c_str());
                ImGui::Indent();
                for (const auto& sub_entry : std::filesystem::directory_iterator(entry.path()))
                {
                    std::string sub_file_name = sub_entry.path().filename().string();
                    ImGui::Text("%s", sub_file_name.c_str());
                }
                ImGui::Unindent();
            }
        }
    ImGui::End();
}

void Editor::draw_remove_file()
{
    if(ImGuiFileDialog::Instance()->Display("RemoveFileKey"))
    {
        if(ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // get extension of file
            std::string extension = filePathName.substr(filePathName.find_last_of(".") + 1);
            // going to copy the image to "resources/images"
            if (extension == "png")
            {
                // get the file name.
                std::string file_name = std::filesystem::path(filePathName).filename().string();
                std::filesystem::remove("resources/images/" + file_name);
            }
            else if (extension == "wav")
            {
                // get the file name.
                std::string file_name = std::filesystem::path(filePathName).filename().string();
                std::filesystem::remove("resources/audio/" + file_name);
            }
            else if (extension == "ttf")
            {
                // get the file name.
                std::string file_name = std::filesystem::path(filePathName).filename().string();
                std::filesystem::remove("resources/fonts/" + file_name);
            }
            else if (extension == "lua")
            {
                // get the file name.
                std::string file_name = std::filesystem::path(filePathName).filename().string();
                std::filesystem::remove("resources/component_types/" + file_name);
            }
            else if (extension == "scene")
            {
                // get the file name.
                std::string file_name = std::filesystem::path(filePathName).filename().string();
                std::filesystem::remove("resources/scenes/" + file_name);

                // get itr to the scene in the scenes vector.
                // remove .scene from the file name.
                file_name = file_name.substr(0, file_name.find_last_of("."));
                auto itr = std::find_if(scenes.begin(), scenes.end(), [file_name](const EditorNameSpace::Scene& scene) {
                    return scene.name == file_name;
                });

                if (itr != scenes.end())
                {
                    scenes.erase(itr);
                }
            }
            else if (extension == "template")
            {
                // get the file name.
                std::string file_name = std::filesystem::path(filePathName).filename().string();
                std::filesystem::remove("resources/actor_templates/" + file_name);

                // remove template from the templates vector.
                // get itr to the template in the templates vector.
                // remove .template from the file name.
                file_name = file_name.substr(0, file_name.find_last_of("."));
                auto itr = std::find_if(templates.begin(), templates.end(), 
                    [file_name](const EditorNameSpace::Actor& _template) {
                        return _template.name == file_name;
                });
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void Editor::scene_editor() 
{
    
    static char scene_name[256] = "New Scene";
    ImGui::Begin("Scene Editor");
    ImGui::InputText("Scene Name", scene_name, sizeof(scene_name));
    if (ImGui::Button("Add Scene")) 
    {
        // convert char into string
        std::string scene_name_string = char_arr_to_string(scene_name);
        if (!scene_name_string.empty())
        {
            scenes.push_back(EditorNameSpace::Scene({scene_name, {}}));
            strcpy(scene_name, "New Scene"); // Reset the scene name input
        }
        else
        {
            _pop_up_warning_empty_scene_name_ = true;
        }
    }
    make_pop_up_warning("Empty Scene Name", "Please specify a scene name.", _pop_up_warning_empty_scene_name_);

    // if (_pop_up_warning_empty_scene_name_)
    // {
    //     make_pop_up_warning("Empty Scene Name", "Please specify a scene name.");
    // }
    // delete scene
    static int scene_to_delete = -1; // -1 means no scene selected for deletion
    std::vector<const char*> scene_names;
    for (const auto& scene : scenes)
    {
        scene_names.push_back(scene.name.c_str());
    }
    if (scene_names.size() > 0)
    {
        ImGui::SameLine();
        if (ImGui::Button("Delete Scene"))
        {
            ImGui::OpenPopup("Delete Scene");
        }
        // show the popup
        if (ImGui::BeginPopup("Delete Scene"))
        {
            ImGui::Text("Select a scene to delete:");
            for (int i = 0; i < scene_names.size(); i++)
            {
                if (ImGui::Selectable(scene_names[i]))
                {
                    // set index for deletion and close current popup to open confirmation
                    scene_to_delete = i;
                    ImGui::CloseCurrentPopup(); // Close this popup to open the next one outside this scope
                }
            }
            ImGui::EndPopup();
        }
    }
    // Handle the confirmation popup outside the scope of the "Delete Scene" popup
    if (scene_to_delete != -1)
    {
        ImGui::OpenPopup("Confirm Deletion"); // Open the confirmation popup
    }
    // confirmation modal
    if (ImGui::BeginPopupModal("Confirm Deletion", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to delete this scene?");
        if (ImGui::Button("Yes"))
        {
            // erase the scene from the resource file.
            std::string file_name = scenes[scene_to_delete].name + ".scene";
            std::filesystem::remove("resources/scenes/" + file_name);
            scenes.erase(scenes.begin() + scene_to_delete);
            scene_to_delete = -1; // Reset the index after deletion
            ImGui::CloseCurrentPopup(); // Close the confirmation popup
        }
        ImGui::SameLine();
        if (ImGui::Button("No"))
        {
            scene_to_delete = -1; // Reset the index if not confirmed
            ImGui::CloseCurrentPopup(); // Close the confirmation popup
        }
        ImGui::EndPopup();
    }

    for (auto& scene : scenes) 
    {
        std::string scene_label = "Scene: " + scene.name;
        if (ImGui::TreeNode(scene_label.c_str())) 
        {
            static char actor_name[256] = "New Actor";
            // char actor_name[256];
            ImGui::InputText("Actor Name", actor_name, sizeof(actor_name));
            if (ImGui::Button("Add Actor")) 
            {
                // convert char into string
                std::string actor_name_string = actor_name;
                if (!actor_name_string.empty())
                {
                    scene.actors.push_back(EditorNameSpace::Actor({actor_name_string, {}}));
                    strcpy(actor_name, "New Actor"); // Reset the actor name input
                }
                else
                {
                    _pop_up_warning_empty_actor_name_ = true;
                }
            }
            make_pop_up_warning("Empty Actor Name", "Please specify an actor name.", _pop_up_warning_empty_actor_name_);
            // delete actor
            static int actor_to_delete = -1; // -1 means no actor selected for deletion
            std::vector<const char*> actor_names;
            for (const auto& actor : scene.actors)
            {
                actor_names.push_back(actor.name.c_str());
            }
            if (actor_names.size() > 0)
            {
                ImGui::SameLine();
                if (ImGui::Button("Delete Actor"))
                {
                    ImGui::OpenPopup("Delete Actor");
                }
                // show the popup
                if (ImGui::BeginPopup("Delete Actor"))
                {
                    ImGui::Text("Select an actor to delete:");
                    for (int i = 0; i < actor_names.size(); i++)
                    {
                        if (ImGui::Selectable(actor_names[i]))
                        {
                            // set index for deletion and close current popup to open confirmation
                            actor_to_delete = i;
                            ImGui::CloseCurrentPopup(); // Close this popup to open the next one outside this scope
                        }
                    }
                    ImGui::EndPopup();
                }
            }

            // Handle the confirmation popup outside the scope of the "Delete Actor" popup
            if (actor_to_delete != -1)
            {
                ImGui::OpenPopup("Confirm Deletion"); // Open the confirmation popup
            }

            // confirmation modal
            if (ImGui::BeginPopupModal("Confirm Deletion", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Are you sure you want to delete this actor?");
                if (ImGui::Button("Yes"))
                {
                    scene.actors.erase(scene.actors.begin() + actor_to_delete);
                    actor_to_delete = -1; // Reset the index after deletion
                    ImGui::CloseCurrentPopup(); // Close the confirmation popup
                }
                ImGui::SameLine();
                if (ImGui::Button("No"))
                {
                    actor_to_delete = -1; // Reset the index if not confirmed
                    ImGui::CloseCurrentPopup(); // Close the confirmation popup
                }
                ImGui::EndPopup();
            }

            for (int i = 0; i < scene.actors.size(); i++) 
            {
                auto& actor = scene.actors[i];
                std::string actor_label = "Actor: " + actor.name + "##" + std::to_string(i);
                if (ImGui::TreeNode(actor_label.c_str())) 
                {
                    // char actor_buffer[256];
                    // strcpy(actor_buffer, actor.name.c_str());
                    // if (ImGui::InputText("Actor Name##Actor", actor_buffer, sizeof(actor_buffer))) 
                    // {
                    //     actor.name = actor_buffer;
                    // }
                
                    static char new_component_name[256] = "";
                    // char new_component_name[256];
                    ImGui::InputText("New Component Name##Component", new_component_name, sizeof(new_component_name));
                    if (ImGui::Button("Add Component##Component")) 
                    {
                        // convert the char array to a string
                        std::string new_component_name_string = new_component_name;
                        if (!new_component_name_string.empty())
                        {
                            actor.components.push_back(EditorNameSpace::Component({new_component_name_string, {}}));
                            strcpy(new_component_name, ""); // Reset the new component name input
                        }
                        else
                        {
                            _pop_up_warning_empty_component_name_ = true;
                        }
                    }
                    make_pop_up_warning("Empty Component Name", "Please specify a component name.", _pop_up_warning_empty_component_name_);
                    ///////////////// todo
                    static int component_to_delete = -1; // -1 means no component selected for deletion

                    // delete component
                    std::vector<const char*> component_names;
                    for (const auto& component : actor.components)
                    {
                        component_names.push_back(component.type.c_str());
                    }
                    if (component_names.size() > 0)
                    {
                        ImGui::SameLine();
                        if (ImGui::Button("Delete Component"))
                        {
                            ImGui::OpenPopup("Delete Component");
                        }
                        // show the popup
                        if (ImGui::BeginPopup("Delete Component"))
                        {
                            ImGui::Text("Select a component to delete:");
                            for (int i = 0; i < component_names.size(); i++)
                            {
                                if (ImGui::Selectable(component_names[i]))
                                {
                                    // set index for deletion and close current popup to open confirmation
                                    component_to_delete = i;
                                    ImGui::CloseCurrentPopup(); // Close this popup to open the next one outside this scope
                                }
                            }
                            ImGui::EndPopup();
                        }
                    }

                    // Handle the confirmation popup outside the scope of the "Delete Component" popup
                    if (component_to_delete != -1)
                    {
                        ImGui::OpenPopup("Confirm Deletion"); // Open the confirmation popup
                    }

                    // Confirmation modal
                    if (ImGui::BeginPopupModal("Confirm Deletion", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                    {
                        ImGui::Text("Are you sure you want to delete this component?");
                        if (ImGui::Button("Yes"))
                        {
                            // delete the lua file 
                            std::string file_name = actor.components[component_to_delete].type + ".lua";
                            std::filesystem::remove("resources/component_types/" + file_name);
                            actor.components.erase(actor.components.begin() + component_to_delete);
                            component_to_delete = -1; // Reset the index after deletion
                            ImGui::CloseCurrentPopup(); // Close the confirmation popup
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("No"))
                        {
                            component_to_delete = -1; // Reset the index if not confirmed
                            ImGui::CloseCurrentPopup(); // Close the confirmation popup
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::SameLine();
                    static int component_to_edit = -1; // -1 means no component selected for editing
                    if (ImGui::Button("Edit Component"))
                    {
                        ImGui::OpenPopup("Edit Component");
                    }
                    // show the popup
                    if (ImGui::BeginPopup("Edit Component"))
                    {
                        ImGui::Text("Select a component to edit:");
                        for (int i = 0; i < component_names.size(); i++)
                        {
                            if (ImGui::Selectable(component_names[i]))
                            {
                                // set index for deletion and close current popup to open confirmation
                                component_to_edit = i;
                                ImGui::CloseCurrentPopup(); // Close this popup to open the next one outside this scope
                                _lua_editor_open_ = true;
                            }
                        }
                        ImGui::EndPopup();
                    }
                    // if (component_to_edit != -1)
                    if (_lua_editor_open_)
                    {
                        std::string component_name = actor.components[component_to_edit].type;
                        std::string component_file_name = component_name + ".lua";
                        std::string component_file_path = "resources/component_types/" + component_file_name;
                        open_lua_editor(component_file_path);
                        // component_to_edit = -1; // Reset the index after deletion
                    }

                    for (int j = 0; j < actor.components.size(); j++) 
                    {
                        auto& component = actor.components[j];
                        std::string component_label = "Component: " + component.type + "##" + std::to_string(j);
                        if (ImGui::TreeNode(component_label.c_str())) 
                        {
                            // char component_buffer[256];
                            // strcpy(component_buffer, component.type.c_str());
                            // if (ImGui::InputText("Component Name##Component", component_buffer, sizeof(component_buffer))) 
                            // {
                            //     component.type = component_buffer;
                            // }
                            

                            // Handle adding a new override
                            // static char new_override_key[256] = "";
                            // static char new_override_value[256] = "";
                            char new_override_key[256];
                            char new_override_value[256];
                            std::string override_key_label = "New Override Key##Override" + std::to_string(j);
                            std::string override_value_label = "New Override Value##Override" + std::to_string(j);
                            ImGui::InputText(override_key_label.c_str(), component.new_override_key, sizeof(new_override_key));
                            ImGui::InputText(override_value_label.c_str(), component.new_override_value, sizeof(new_override_value));
                            std::string button_label = "Add Override##Override" + std::to_string(j);
                            if (ImGui::Button(button_label.c_str())) 
                            {
                                std::string new_override_key_string = component.new_override_key;
                                std::string new_override_value_string = component.new_override_value;
                                if (new_override_key_string.empty())
                                {
                                    _pop_up_warning_empty_override_key_ = true;
                                }
                                else if (new_override_value_string.empty())
                                {
                                    _pop_up_warning_empty_component_value_ = true;
                                }
                                else
                                {
                                    component.overrides.push_back(std::make_pair(new_override_key_string, new_override_value_string));
                                    strcpy(component.new_override_key, ""); // Reset the new override key input
                                    strcpy(component.new_override_value, ""); // Reset the new override value input
                                }
                            }
                            make_pop_up_warning("Empty Override Key", "Please specify an override key.", _pop_up_warning_empty_override_key_);
                            make_pop_up_warning("Empty Override Value", "Please specify an override value.", _pop_up_warning_empty_component_value_);
                            // select override to remove 
                            static int override_to_delete = -1; // -1 means no override selected for deletion
                            std::vector<const char*> override_keys;
                            for (const auto& override : component.overrides)
                            {
                                override_keys.push_back(override.first.c_str());
                            }
                            if (override_keys.size() > 0)
                            {
                                ImGui::SameLine();
                                if (ImGui::Button("Delete Override"))
                                {
                                    ImGui::OpenPopup("Delete Override");
                                }
                                // show the popup
                                if (ImGui::BeginPopup("Delete Override"))
                                {
                                    ImGui::Text("Select an override to delete:");
                                    for (int i = 0; i < override_keys.size(); i++)
                                    {
                                        if (ImGui::Selectable(override_keys[i]))
                                        {
                                            // set index for deletion and close current popup to open confirmation
                                            override_to_delete = i;
                                            ImGui::CloseCurrentPopup(); // Close this popup to open the next one outside this scope
                                        }
                                    }
                                    ImGui::EndPopup();
                                }
                            }
                            // Handle the confirmation popup outside the scope of the "Delete Override" popup
                            if (override_to_delete != -1)
                            {
                                ImGui::OpenPopup("Confirm Deletion"); // Open the confirmation popup
                            }
                            // Confirmation modal
                            if (ImGui::BeginPopupModal("Confirm Deletion", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                            {
                                ImGui::Text("Are you sure you want to delete this override?");
                                if (ImGui::Button("Yes"))
                                {
                                    component.overrides.erase(component.overrides.begin() + override_to_delete);
                                    override_to_delete = -1; // Reset the index after deletion
                                    ImGui::CloseCurrentPopup(); // Close the confirmation popup
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("No"))
                                {
                                    override_to_delete = -1; // Reset the index if not confirmed
                                    ImGui::CloseCurrentPopup(); // Close the confirmation popup
                                }
                                ImGui::EndPopup();
                            }


                            // Display existing overrides
                            for (int k = 0; k < component.overrides.size(); k++) 
                            {
                                auto& override = component.overrides[k];
                                std::string override_label = "Override " + std::to_string(k) + "##" + std::to_string(j);
                                if (ImGui::TreeNode(override_label.c_str())) {
                                    char override_key_buffer[256];
                                    char override_value_buffer[256];
                                    strcpy(override_key_buffer, override.first.c_str());
                                    strcpy(override_value_buffer, override.second.c_str());
                                    if (ImGui::InputText("Key##Override", override_key_buffer, sizeof(override_key_buffer))) 
                                    {
                                        override.first = override_key_buffer;
                                    }
                                    if (ImGui::InputText("Value##Override", override_value_buffer, sizeof(override_value_buffer)))
                                    {
                                        override.second = override_value_buffer;
                                    }
                                    ImGui::TreePop(); // Close the override tree node
                                }
                            }
                            // add some padding to the bottom
                            ImGui::Dummy(ImVec2(0.0f, 10.0f));

                            ImGui::TreePop(); // Close the component tree node
                        }
                    }

                    ImGui::TreePop(); // Close the actor tree node
                }
            }

            ImGui::TreePop(); // Close the scene tree node
        }
    }

    if (ImGui::Button("Save Scene(s)")) 
    {
        for (const auto& scene : scenes) 
        {
            save_scene(scene);
        }
    }

    ImGui::End();
}

// Serialize the scenes to a json file.
void Editor::save_scene(const EditorNameSpace::Scene& scene)
{

    if (!std::filesystem::exists("resources/scenes"))
    {
        std::filesystem::create_directory("resources/scenes");
    }
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    rapidjson::Value actors_array(rapidjson::kArrayType);
    for (const auto& actor : scene.actors)
    {
        rapidjson::Value actor_object(rapidjson::kObjectType);
        actor_object.AddMember("name", rapidjson::Value(actor.name.c_str(), allocator), allocator);

        rapidjson::Value components_object(rapidjson::kObjectType);
        int component_index = 1;
        for (const auto& component : actor.components)
        {
            rapidjson::Value component_object(rapidjson::kObjectType);
            component_object.AddMember("type", rapidjson::Value(component.type.c_str(), allocator), allocator);

            for (const auto& override : component.overrides)
            {
                rapidjson::Value value;
                if (override.second.find('.') != std::string::npos || (override.second[0] == '-' && override.second.size() > 1)) // Check if it's a float
                {
                    value.SetFloat(std::stod(override.second));
                }
                // else if (std::all_of(override.second.begin(), override.second.end(), ::isdigit)) // Check if it's an int
                else if (std::all_of(override.second.begin(), override.second.end(), ::isdigit) || (override.second[0] == '-' && override.second.size() > 1 && std::all_of(override.second.begin() + 1, override.second.end(), ::isdigit))) // Check if it's an int
                {
                    value.SetInt(std::stoi(override.second));
                }
                else if (is_bool(override.second)) // Check if it's a bool
                {
                    value.SetBool(override.second == "true");
                }
                else // It's a string
                {
                    value.SetString(override.second.c_str(), allocator);
                }
                component_object.AddMember(rapidjson::Value(override.first.c_str(), allocator), value, allocator);
            }

            components_object.AddMember(rapidjson::Value(std::to_string(component_index++).c_str(), allocator), component_object, allocator);
        }

        actor_object.AddMember("components", components_object, allocator);
        actors_array.PushBack(actor_object, allocator);
    }

    document.AddMember("actors", actors_array, allocator);

    // Convert the document to a string
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string json_output = buffer.GetString();

    // Write the string to a file
    std::string filename = "resources/scenes/" + scene.name + ".scene";
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << json_output;
        file.close();
    }
}


void Editor::load_scenes()
{
    if (!std::filesystem::exists("resources/scenes"))
    {
        return; // nothing to load.
    }
    for (const auto& entry : std::filesystem::directory_iterator("resources/scenes"))
    {
        std::string file_name = entry.path().filename().string();
        std::string scene_name = file_name.substr(0, file_name.find_last_of("."));
        // see if scene already exists
        auto itr = std::find_if(scenes.begin(), scenes.end(), [scene_name](const EditorNameSpace::Scene& scene) {
            return scene.name == scene_name;
        });
        if (itr == scenes.end()) // scene does not exist
        {
            rapidjson::Document document;
            ConfigManager::read_json_file(entry.path().string(), document);
            if (document.HasMember("actors") && document["actors"].IsArray())
            {
                std::vector<EditorNameSpace::Actor> actors;
                for (const auto& actor : document["actors"].GetArray())
                {
                    if (actor.HasMember("name") && actor["name"].IsString())
                    {
                        EditorNameSpace::Actor new_actor(actor["name"].GetString());
                        if (actor.HasMember("components") && actor["components"].IsObject())
                        {
                            for (const auto& component_member : actor["components"].GetObject())
                            {
                                const auto& component = component_member.value;
                                if (component.HasMember("type") && component["type"].IsString())
                                {
                                    EditorNameSpace::Component new_component(component["type"].GetString());
                                    for (const auto& override_member : component.GetObject())
                                    {
                                        if (override_member.name != "type") // Skip the type member
                                        {
                                            std::string override_key = override_member.name.GetString();
                                            std::string override_value;
                                            if (override_member.value.IsString())
                                            {
                                                override_value = override_member.value.GetString();
                                            }
                                            else if (override_member.value.IsInt())
                                            {
                                                override_value = std::to_string(override_member.value.GetInt());
                                            }
                                            else if (override_member.value.IsFloat())
                                            {
                                                override_value = std::to_string(override_member.value.GetFloat());
                                            }
                                            else if (override_member.value.IsBool())
                                            {
                                                override_value = override_member.value.GetBool() ? "true" : "false";
                                            }
                                            new_component.overrides.push_back(std::make_pair(override_key, override_value));
                                        }
                                    }
                                    new_actor.components.push_back(new_component);
                                }
                            }
                        }
                        actors.push_back(new_actor);
                    }
                }
                scenes.push_back(EditorNameSpace::Scene({scene_name, actors}));
            }
        }
    }
}

bool Editor::is_valid_initial_scene()
{
    std::string initial_scene_casting = "";
    // loop through the chars in the game_config.initial_scene and cast them to a string.
    for (const auto& c : game_config.initial_scene)
    {
        if (c == '\0')
        {
            break;
        }
        initial_scene_casting += c;
    }

    
    for (const auto& scene : scenes)
    {
        if (scene.name == initial_scene_casting)
        {
            return true;
        }
    }
    return false;
}

void Editor::clean_up()
{
    scenes.clear();
    templates.clear();
}

void Editor::template_editor()
{
    static char template_name[256] = "New Template";
    ImGui::Begin("Template Editor");
        ImGui::InputText("Template Name", template_name, sizeof(template_name));
        if (ImGui::Button("Add Template"))
        {
            // check to see if template exists
            for (const auto& temp : templates)
            {
                if (temp.name == template_name)
                {
                    return; // template already exists.
                }
            }
            if (std::string(template_name).empty())
            {
                _template_pop_up_empty_template_name_ = true;
            }
            else
            {
                std::string template_name_string = template_name;
                templates.push_back(EditorNameSpace::Actor({template_name_string, {}}));
            }
        }
        make_pop_up_warning("Empty Template Name", "Please specify a template name.", _template_pop_up_empty_template_name_);

        // delete template
        static int template_to_delete = -1; // -1 means no template selected for deletion
        std::vector<const char*> template_names;
        for (const auto& temp : templates)
        {
            template_names.push_back(temp.name.c_str());
        }
        if (template_names.size() > 0)
        {
            ImGui::SameLine();
            if (ImGui::Button("Delete Template"))
            {
                ImGui::OpenPopup("Delete Template");
            }
            // show the popup
            if (ImGui::BeginPopup("Delete Template"))
            {
                ImGui::Text("Select a template to delete:");
                for (int i = 0; i < template_names.size(); i++)
                {
                    if (ImGui::Selectable(template_names[i]))
                    {
                        // set index for deletion and close current popup to open confirmation
                        template_to_delete = i;
                        ImGui::CloseCurrentPopup(); // Close this popup to open the next one outside this scope
                    }
                }
                ImGui::EndPopup();
            }
        }

        // Handle the confirmation popup outside the scope of the "Delete Template" popup
        if (template_to_delete != -1)
        {
            ImGui::OpenPopup("Confirm Deletion"); // Open the confirmation popup
        }
        // confirmation modal
        if (ImGui::BeginPopupModal("Confirm Deletion", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you want to delete this template?");
            if (ImGui::Button("Yes"))
            {
                // remove the file
                std::string file_name = templates[template_to_delete].name + ".template";
                std::filesystem::remove("resources/actor_templates/" + file_name);
                templates.erase(templates.begin() + template_to_delete);
                template_to_delete = -1; // Reset the index after deletion
                ImGui::CloseCurrentPopup(); // Close the confirmation popup
            }
            ImGui::SameLine();
            if (ImGui::Button("No"))
            {
                template_to_delete = -1; // Reset the index if not confirmed
                ImGui::CloseCurrentPopup(); // Close the confirmation popup
            }
            ImGui::EndPopup();
        }
        
        for (auto& temp: templates)
        {
            std::string temp_label = "Template: " + temp.name;
            if (ImGui::TreeNode(temp_label.c_str()))
            {
                // char actor_name[256];
                // strcpy(actor_name, temp.name.c_str());
                // if (ImGui::InputText("Actor Name", actor_name, sizeof(actor_name)))
                // {
                //     temp.name = actor_name;
                // }

                static char new_component_name[256] = "";
                ImGui::InputText("New Component Name##Component", new_component_name, sizeof(new_component_name));
                if (ImGui::Button("Add Component##Component")) 
                {
                    // convert the char array to a string
                    std::string new_component_name_string = new_component_name;
                    if (new_component_name_string.empty())
                    {
                        _pop_up_warning_empty_component_name_ = true;
                    }
                    else
                    {
                        temp.components.push_back(EditorNameSpace::Component({new_component_name_string, {}}));
                        strcpy(new_component_name, ""); // Reset the new component name input
                    }
                }

                static int component_to_delete = -1; // -1 means no component selected for deletion
                std::vector<const char*> component_names;
                for (const auto& component : temp.components)
                {
                    component_names.push_back(component.type.c_str());
                }
                if (component_names.size() > 0)
                {
                    ImGui::SameLine();
                    if (ImGui::Button("Delete Component"))
                    {
                        ImGui::OpenPopup("Delete Component");
                    }
                    // show the popup
                    if (ImGui::BeginPopup("Delete Component"))
                    {
                        ImGui::Text("Select a component to delete:");
                        for (int i = 0; i < component_names.size(); i++)
                        {
                            if (ImGui::Selectable(component_names[i]))
                            {
                                // set index for deletion and close current popup to open confirmation
                                component_to_delete = i;
                                ImGui::CloseCurrentPopup(); // Close this popup to open the next one outside this scope
                            }
                        }
                        ImGui::EndPopup();
                    }
                }
                if (component_to_delete != -1)
                {
                    ImGui::OpenPopup("Confirm Deletion"); // Open the confirmation popup
                }
                if (ImGui::BeginPopupModal("Confirm Deletion", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Are you sure you want to delete this component?");
                    if (ImGui::Button("Yes"))
                    {
                        // delete lua file.
                        std::string file_name = temp.components[component_to_delete].type + ".lua";
                        std::filesystem::remove("resources/component_types/" + file_name);
                        temp.components.erase(temp.components.begin() + component_to_delete);
                        component_to_delete = -1; // Reset the index after deletion
                        ImGui::CloseCurrentPopup(); // Close the confirmation popup
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("No"))
                    {
                        component_to_delete = -1; // Reset the index if not confirmed
                        ImGui::CloseCurrentPopup(); // Close the confirmation popup
                    }
                    ImGui::EndPopup();
                }

                make_pop_up_warning("Empty Component Name", "Please specify a component name.", _pop_up_warning_empty_component_name_);
                for (int i = 0; i < temp.components.size(); i++)
                {
                    auto& component = temp.components[i];
                    std::string component_label = "Component: " + component.type + "##" + std::to_string(i);
                    if (ImGui::TreeNode(component_label.c_str()))
                    {
                        // char component_name[256];
                        // strcpy(component_name, component.type.c_str());
                        // if (ImGui::InputText("Component Name", component_name, sizeof(component_name)))
                        // {
                        //     component.type = component_name;
                        // }

                            // Handle adding a new override
                            static char new_override_key[256] = "";
                            static char new_override_value[256] = "";
                            ImGui::InputText("New Override Key##Override", component.new_override_key, sizeof(new_override_key));
                            ImGui::InputText("New Override Value##Override", component.new_override_value, sizeof(new_override_value));
                            if (ImGui::Button("Add Override##Override")) 
                            {
                                if (std::string(component.new_override_key).empty())
                                {
                                    _pop_up_warning_empty_override_key_ = true;
                                }
                                else if (std::string(component.new_override_value).empty())
                                {
                                    _pop_up_warning_empty_component_value_ = true;
                                }
                                else
                                {
                                    component.overrides.push_back(std::make_pair(std::string(component.new_override_key), std::string(component.new_override_value)));
                                    strcpy(component.new_override_key, ""); // Reset the new override key input
                                    strcpy(component.new_override_value, ""); // Reset the new override value input
                                }
                            }

                            make_pop_up_warning("Empty Override Key", "Please specify an override key.", _pop_up_warning_empty_override_key_);
                            make_pop_up_warning("Empty Override Value", "Please specify an override value.", _pop_up_warning_empty_component_value_);

                            static int override_to_delete = -1; // -1 means no override selected for deletion
                            std::vector<const char*> override_keys;
                            for (const auto& override : component.overrides)
                            {
                                override_keys.push_back(override.first.c_str());
                            }
                            if (override_keys.size() > 0)
                            {
                                ImGui::SameLine();
                                if (ImGui::Button("Delete Override"))
                                {
                                    ImGui::OpenPopup("Delete Override");
                                }
                                // show the popup
                                if (ImGui::BeginPopup("Delete Override"))
                                {
                                    ImGui::Text("Select an override to delete:");
                                    for (int i = 0; i < override_keys.size(); i++)
                                    {
                                        if (ImGui::Selectable(override_keys[i]))
                                        {
                                            // set index for deletion and close current popup to open confirmation
                                            override_to_delete = i;
                                            ImGui::CloseCurrentPopup(); // Close this popup to open the next one outside this scope
                                        }
                                    }
                                    ImGui::EndPopup();
                                }
                            }

                            // Handle the confirmation popup outside the scope of the "Delete Override" popup
                            if (override_to_delete != -1)
                            {
                                ImGui::OpenPopup("Confirm Deletion"); // Open the confirmation popup
                            }
                            // Confirmation modal
                            if (ImGui::BeginPopupModal("Confirm Deletion", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                            {
                                ImGui::Text("Are you sure you want to delete this override?");
                                if (ImGui::Button("Yes"))
                                {
                                    component.overrides.erase(component.overrides.begin() + override_to_delete);
                                    override_to_delete = -1; // Reset the index after deletion
                                    ImGui::CloseCurrentPopup(); // Close the confirmation popup
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("No"))
                                {
                                    override_to_delete = -1; // Reset the index if not confirmed
                                    ImGui::CloseCurrentPopup(); // Close the confirmation popup
                                }
                                ImGui::EndPopup();
                            }
                        
                        // lua editor
                        ImGui::SameLine();
                        static int temp_component_to_edit = -1; // -1 means no component selected for editing
                        if (ImGui::Button("Edit Component"))
                        {
                            ImGui::OpenPopup("Edit Component");
                        }
                        // show the popup
                        if (ImGui::BeginPopup("Edit Component"))
                        {
                            ImGui::Text("Select a component to edit:");
                            for (int i = 0; i < component_names.size(); i++)
                            {
                                if (ImGui::Selectable(component_names[i]))
                                {
                                    // set index for deletion and close current popup to open confirmation
                                    temp_component_to_edit = i;
                                    ImGui::CloseCurrentPopup(); // Close this popup to open the next one outside this scope
                                    _lua_editor_open_ = true;
                                }
                            }
                            ImGui::EndPopup();
                        }
                        if (_lua_editor_open_)
                        {
                            std::string component_name = temp.components[temp_component_to_edit].type;
                            std::string component_file_name = component_name + ".lua";
                            std::string component_file_path = "resources/component_types/" + component_file_name;
                            open_lua_editor(component_file_path);
                        }
                        for (int j = 0; j < component.overrides.size(); j++)
                        {
                            auto& override = component.overrides[j];
                            std::string override_label = "Override: " + override.first + "##" + std::to_string(j);
                            if (ImGui::TreeNode(override_label.c_str()))
                            {
                                char override_key[256];
                                char override_value[256];
                                strcpy(override_key, override.first.c_str());
                                strcpy(override_value, override.second.c_str());
                                if (ImGui::InputText("Override Key", override_key, sizeof(override_key)))
                                {
                                    override.first = override_key;
                                }
                                if (ImGui::InputText("Override Value", override_value, sizeof(override_value)))
                                {
                                    override.second = override_value;
                                }
                                ImGui::TreePop();
                            }
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
    if (ImGui::Button("Save Templates"))
    {
        save_templates();
    }
    ImGui::End();
}

void Editor::save_templates()
{
    if (!std::filesystem::exists("resources/actor_templates"))
    {
        std::filesystem::create_directory("resources/actor_templates");
    }
    for (auto& temp : templates)
    {
        rapidjson::Document document;
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        rapidjson::Value name(temp.name.c_str(), allocator);
        document.AddMember("name", name, allocator);

        rapidjson::Value components_object(rapidjson::kObjectType);
        int component_index = 1;
        for (const auto& component : temp.components)
        {
            rapidjson::Value component_object(rapidjson::kObjectType);
            component_object.AddMember("type", rapidjson::Value(component.type.c_str(), allocator), allocator);

            for (const auto& override : component.overrides)
            {
                rapidjson::Value value;
                if (override.second.find('.') != std::string::npos) // Check if it's a float
                {
                    value.SetFloat(std::stod(override.second));
                }
                else if (std::all_of(override.second.begin(), override.second.end(), ::isdigit)) // Check if it's an int
                {
                    value.SetInt(std::stoi(override.second));
                }
                else if (is_bool(override.second)) // Check if it's a bool
                {
                    value.SetBool(override.second == "true");
                }
                else // It's a string
                {
                    value.SetString(override.second.c_str(), allocator);
                }
                component_object.AddMember(rapidjson::Value(override.first.c_str(), allocator), value, allocator);
            }

            // components_array.PushBack(component_object, allocator);
            components_object.AddMember(rapidjson::Value(std::to_string(component_index++).c_str(), allocator), component_object, allocator);
        }

        // document.AddMember("components", components_array, allocator);
        document.AddMember("components", components_object, allocator);

        // Convert the document to a string
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        std::string json_output = buffer.GetString();

        // Write the string to a file
        std::string filename = "resources/actor_templates/" + temp.name + ".template";
        std::ofstream file(filename);
        if (file.is_open())
        {
            file << json_output;
            file.close();
        }
    }
}

void Editor::load_templates() 
{
    // Make sure the templates directory exists
    std::filesystem::path templates_dir("resources/actor_templates");
    if (!std::filesystem::exists(templates_dir)) 
    {
        // std::cerr << "Templates directory does not exist.\n";
        return; // Nothing to load.
    }

    // Iterate through the template files in the directory
    for (const auto& entry : std::filesystem::directory_iterator(templates_dir)) 
    {
        std::string file_path = entry.path().string();
        
        // Skip if not a file or if it's not a .template file
        if (!entry.is_regular_file() || entry.path().extension() != ".template") 
        {
            continue;
        }

        // Load and parse the JSON file
        rapidjson::Document document;
        ConfigManager::read_json_file(file_path, document);
        
        if (document.HasParseError()) {
            std::cerr << "JSON parse error in template: " << file_path << "\n";
            continue;
        }
        // Extract the name and components of the template
        if (document.IsObject() && document.HasMember("name") 
            && document["name"].IsString()) 
        {
            bool template_exists = false;
            // see if the template already exists in templates
            // if it does, skip it.
            for (const auto& temp : templates)
            {
                if (temp.name == document["name"].GetString())
                {
                    template_exists = true;
                    break;
                }
            }

            if (!template_exists) 
            {
                std::string template_name = document["name"].GetString();
                std::vector<EditorNameSpace::Component> components;

                if (document.HasMember("components") && document["components"].IsObject()) 
                {
                    for (auto& m : document["components"].GetObject()) 
                    {
                        std::string component_type = m.value["type"].GetString();
                        // Construct the component, parse overrides if any
                        EditorNameSpace::Component component(component_type);

                        for (auto& override_member : m.value.GetObject()) 
                        {
                            if (override_member.name != "type") 
                            { 
                                // Skip the 'type' as it's already used
                                std::string key = override_member.name.GetString();
                                std::string value;
                                // The value could be a number, bool or string
                                if (override_member.value.IsString()) 
                                {
                                    value = override_member.value.GetString();
                                } 
                                else if (override_member.value.IsBool()) 
                                {
                                    value = override_member.value.GetBool() ? "true" : "false";
                                } 
                                else if (override_member.value.IsInt()) 
                                {
                                    value = std::to_string(override_member.value.GetInt());
                                } 
                                else if (override_member.value.IsFloat()) 
                                {
                                    value = std::to_string(override_member.value.GetFloat());
                                }

                                component.overrides.emplace_back(key, value);
                            }
                        }

                        components.push_back(std::move(component));
                    }
                }

                // Add the loaded template to the vector
                templates.push_back(EditorNameSpace::Actor({template_name, {}}));
                for (auto& component : components)
                {
                    templates.back().components.push_back(component);
                }
            }
            // templates.push_back(EditorNameSpace::Actor(template_name, std::move(components)));
        }
    }
}

bool is_bool(const std::string& str) 
{
    return str == "true" || str == "false";
}

void Editor::init_scene_preview(WindowRenderer* editor_window)
{

    if (_current_scene_rendered_ == "")
    {
        _current_scene_rendered_ = get_initial_scene_from_game_config();
    }
    if (ImGui::Button("Build Scene"))
    {   
        scene_preview = SDL_CreateTexture(editor_window->get_renderer(), 
                                            SDL_PIXELFORMAT_RGBA8888, 
                                            SDL_TEXTUREACCESS_TARGET, 
                                            rendering_config.x_resolution, 
                                            rendering_config.y_resolution);
        _build_scene_ = true;
        // print out scne_preview size
        SDL_SetRenderTarget(editor_window->get_renderer(), scene_preview);
        SDL_SetRenderDrawColor(editor_window->get_renderer(), 
                            rendering_config.clear_color_r, 
                            rendering_config.clear_color_g, 
                            rendering_config.clear_color_b, 
                            255);
        SDL_RenderClear(editor_window->get_renderer());
        AudioManager::is_in_editor_mode = true;
        ComponentManager::cleanup(); // clears the component tables.
        ComponentManager::init_components(); // re-initializes the component tables.

        SceneManager::clean_up(); // clears the imported_actors vector.
        // SceneManager::load_initial_scene();
        SceneManager::load_scene(_current_scene_rendered_);
        ImageManager::clean_up(); // ? need ?
        PhysicsManager::Init(); // hmmmmmmm
        
        Emgine game;
        game.run_onstart_for_all_actors();
        game.add_new_actors(); // this was in an if block before.
        game.process_added_components();
        game.onupdate_for_all_actors();
        game.onlateupdate_for_all_actors();
        // EventBus::UpdateSubscriptions(); need???
        game.process_removed_components();
        game.actors_that_need_to_be_destroyed();
        SDL_RenderSetScale(editor_window->get_renderer(), Camera::GetZoom(), Camera::GetZoom());
        ImageManager::RenderImageRequests(editor_window->get_renderer(), 
                                          editor_window, 
                                          rendering_config.x_resolution, 
                                          rendering_config.y_resolution);
        SDL_RenderSetScale(editor_window->get_renderer(), 1.0f, 1.0f);
        ImageManager::RenderUIRequest(editor_window->get_renderer());
        TextManager::RenderTextRequest(editor_window->get_renderer(), editor_window);
        ImageManager::RenderPixelRequests(editor_window->get_renderer(), editor_window);
        AudioManager::is_in_editor_mode = false;
    }
    else if (!_build_scene_)
    {
        scene_preview = SDL_CreateTexture(editor_window->get_renderer(), 
                                            SDL_PIXELFORMAT_RGBA8888, 
                                            SDL_TEXTUREACCESS_TARGET, 
                                            rendering_config.x_resolution, 
                                            rendering_config.y_resolution);
        // render clear color.
        SDL_SetRenderTarget(editor_window->get_renderer(), scene_preview);
        SDL_SetRenderDrawColor(editor_window->get_renderer(), 
                            rendering_config.clear_color_r, 
                            rendering_config.clear_color_g, 
                            rendering_config.clear_color_b, 
                            255);
        SDL_RenderClear(editor_window->get_renderer());
    }
    ImGui::SameLine();
    // if (ImGui::Button("Load Scene to Render"))
    // {
    //     std::cout << "button pressed\n";
    // }
    show_scene_drop_down();

    // set the render target back to the window.
    SDL_SetRenderTarget(editor_window->get_renderer(), NULL);
}

std::string Editor::get_initial_scene_from_game_config()
{
    std::string initial_scene_casting = "";
    // loop through the chars in the game_config.initial_scene and cast them to a string.
    for (const auto& c : game_config.initial_scene)
    {
        if (c == '\0')
        {
            break;
        }
        initial_scene_casting += c;
    }
    return initial_scene_casting;
}

void Editor::show_scene_drop_down()
{
    // static that will hold the current index.
    static int current_index = 0; 

    // retrieve scene files from directory
    std::vector<std::string> scene_files;
    for (const auto& entry : std::filesystem::directory_iterator("resources/scenes"))
    {
        if (entry.path().extension() == ".scene")
        {
            scene_files.push_back(entry.path().filename().string());
        }
    }

    if (!_build_scene_)
    {
        // check to see if the current scene is in the scene_files vector.
        auto itr = std::find(scene_files.begin(), scene_files.end(), _current_scene_rendered_ + ".scene");
        if (itr != scene_files.end())
        {
            current_index = std::distance(scene_files.begin(), itr);
        }
        else
        {
            current_index = 0;
        }
    }

    // convert the vector to a char array for ImGui.
    std::vector<const char*> scene_files_c_str;
    for (const auto& scene_file : scene_files)
    {
        scene_files_c_str.push_back(scene_file.c_str());
    }

    // ensure that there is one scene file.
    if (!scene_files_c_str.empty())
    {
        // display the drop down.
        if (ImGui::BeginCombo("Load Scene To Render", scene_files_c_str[current_index]))
        {
            for (int i = 0; i < scene_files_c_str.size(); i++)
            {
                bool is_selected = (current_index == i);
                if (ImGui::Selectable(scene_files_c_str[i], is_selected))
                {
                    current_index = i;
                    std::string temp = scene_files[i];
                    // remove .scene from the file name.
                    temp = temp.substr(0, temp.find_last_of("."));
                    _current_scene_rendered_ = temp;

                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
    else
    {
        ImGui::Text("No Scene Files Found");
    }
}

// Method to open the Lua editor
void Editor::open_lua_editor(const std::string& file_path) 
{
    if (!lua_code_buffer_full) 
    {
        lua_code_buffer.clear();
        std::ifstream file(file_path); 
        if (!file) // if the file doesn't exist, create it
        {
            std::ofstream new_file(file_path);
            new_file.close();
            file.open(file_path);
        }
        if (file.is_open()) 
        {
            std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            if (contents.empty()) 
            {
                // get the component type from the file name
                std::string component_type = file_path.substr(file_path.find_last_of('/') + 1);
                // remove the .lua extension
                component_type = component_type.substr(0, component_type.find_last_of('.'));
                std::ofstream out_file(file_path);
                out_file << component_type << " = {\n\n}\n";
                out_file.close();
                contents = component_type + " = {\n\n}\n";
            }
            lua_code_buffer.resize(contents.size() + 1024); // Extra space for new characters
            std::copy(contents.begin(), contents.end(), lua_code_buffer.begin());
            file.close();
        }
        ImGui::OpenPopup("Lua Editor");
        lua_code_buffer_full = true;
    }

    if (ImGui::BeginPopupModal("Lua Editor", NULL, ImGuiWindowFlags_MenuBar)) 
    {
        if (ImGui::BeginMenuBar()) 
        {
            if (ImGui::BeginMenu("File")) 
            {
                if (ImGui::MenuItem("Save")) 
                {
                    // std::ofstream out_file(file_path);
                    std::ofstream out_file(file_path, std::ios::binary); // use binary mode to prevent text transformations
                    if (out_file.is_open()) 
                    {
                        size_t content_length = std::strlen(lua_code_buffer.data());
                        out_file.write(lua_code_buffer.data(), content_length);
                        out_file.close();
                    }
                }
                if (ImGui::MenuItem("Close")) 
                {
                    _lua_editor_open_ = false;
                    ImGui::CloseCurrentPopup();
                    lua_code_buffer_full = false;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::InputTextMultiline("##CodeEditor", &lua_code_buffer[0], lua_code_buffer.size() + 1, ImVec2(-FLT_MIN, -FLT_MIN), ImGuiInputTextFlags_AllowTabInput);
        ImGui::EndPopup();
    }
}

std::string Editor::char_arr_to_string(const char* arr)
{
    std::string str;
    for (int i = 0; i < std::strlen(arr); i++)
    {
        if (arr[i] == '\0')
        {
            break;
        }
        str += arr[i];
    }
    return str;
}