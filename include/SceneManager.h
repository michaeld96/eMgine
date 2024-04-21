#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H
#include "Common.h"
#include "ConfigManager.h"
#include "MapHelper.h"
#include <unordered_map>
#include "TemplateManager.h"
#include <cctype>
#include "ImageManager.h"
#include <optional>
#include "AudioManager.h"
#include "Component.h"
#include "PhysicsManager.h"

class SceneManager
{
public:
    // static inline std::vector<Actor*> imported_actors;
    static inline std::vector<std::shared_ptr<Actor>> imported_actors;
    static inline std::string next_scene = "";
    static inline std::unordered_set<int> actors_that_scored;
    static inline glm::vec2 initial_player_pos = glm::vec2(0.0, 0.0);
    static inline int players_uuid = -999;
    static inline std::string initial_scene = "";

    // static inline std::unordered_map<std::string, SDL_Texture*> _cached_textures;
    static inline std::vector<Actor*> actors_that_have_collisions;
    static inline std::vector<Actor*> actors_that_have_triggers;

    // hw7
    static inline std::vector<int> actors_that_need_to_be_destroyed;
    static inline std::vector<std::shared_ptr<Actor>> actors_that_need_to_be_added;
    static inline std::vector<std::shared_ptr<Actor>> actors_that_need_to_have_components_added;
    static inline std::vector<std::shared_ptr<Actor>> actors_that_need_on_start_ran;
    static inline bool new_scene_requested = false;
    static inline std::string new_scene_name = "";
    static inline std::string current_scene = "";
    static inline std::unordered_map<std::string, Actor*> actor_templates;


    static void load_initial_scene()
    {
        rapidjson::Document document;
        ConfigManager::read_json_file("resources/game.config", document);
        if (document.HasMember("initial_scene") 
            && document["initial_scene"].IsString() 
            && document["initial_scene"].GetStringLength() > 0)
        {
            initial_scene = document["initial_scene"].GetString();
            load_scene(initial_scene);
        }
        else
        {
            std::cout << "error: initial_scene unspecified";
            exit(0);
        }
    }

    // static void load_scene(const std::string& scene_name);
    static void load_scene(const std::string&scene_name);

    static void getting_scene_from_dialogue(std::string& dialogue)
    {
        std::string keyword = "proceed to ";
        size_t pos = dialogue.find(keyword);
        if (pos != std::string::npos) {
            next_scene = obtain_word_after_phrase(dialogue, keyword);
        }
    }

    static std::string obtain_word_after_phrase(const std::string& input, const std::string& phrase) 
    {
        // Find the position of the phrase in the string
        size_t pos = input.find(phrase);

        // If phrase is not found, return an empty string
        if (pos == std::string::npos) return "";

        // Find the starting position of the next word (skip spaces after the phrase)
        pos += phrase.length();
        while (pos < input.size() && std::isspace(input[pos])) {
            ++pos;
        }

        // If we're at the end of the string, return an empty string
        if (pos == input.size()) return "";

        // Find the end position of the word (until a space or the end of the string)
        size_t endPos = pos;
        while (endPos < input.size() && !std::isspace(input[endPos])) {
            ++endPos;
        }

        // Extract and return the word
        return input.substr(pos, endPos - pos);
    }
    static uint64_t gen_key(int x, int y)
    {
        uint32_t ux = static_cast<uint32_t>(x);
        uint32_t uy = static_cast<uint32_t>(y);
        uint64_t result = static_cast<uint64_t>(ux);
        result = (result << 32);
        result = result | static_cast<uint64_t>(uy);
        return result;
    }

    static void check_if_lua_file_exists(const std::string& file_name)
    {
        std::string file_path_str = "resources/component_types/" + file_name + ".lua";
        if (!std::filesystem::exists(file_path_str))
        {
            std::cout << "error: failed to locate component " << file_name;
            exit(0);
        }
    }

// static Actor* load_in_template(const std::string& template_name);

static std::shared_ptr<Actor> load_in_template(const std::string& template_name);

static void Load(std::string scene_name);

static std::string GetCurrent();

static void DontDestroy(Actor* actor);

static void clean_up()
{
    imported_actors.clear();
    actors_that_scored.clear();
    actors_that_have_collisions.clear();
    actors_that_have_triggers.clear();
    actors_that_need_to_be_destroyed.clear();
    actors_that_need_to_be_added.clear();
    actors_that_need_to_have_components_added.clear();
    actors_that_need_on_start_ran.clear();
    new_scene_requested = false;
    new_scene_name = "";
    current_scene = "";
    actor_templates.clear();

}

};
#endif // SCENEMANAGER_H
