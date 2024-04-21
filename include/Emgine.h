#ifndef EMGINE_H
#define EMGINE_H
#include "Common.h"
#include <string>
#include <unordered_map>
#include "ConfigManager.h"
#include "Helper.h"
#include "ImageManager.h"
#include "SceneManager.h"
#include "TextManager.h"
#include "WindowRenderer.h"
#include "AudioManager.h"
#include "InputManager.h"
#include "MapHelper.h"

enum class GameState
{
    RUNNING,
    INTRO,
    PLAYER_DEATH,
    NEW_SCENE,
    QUIT,
    WIN
};

class Renderer;

class Emgine
{ 
friend class Renderer;
private:
    bool quit;
    bool            emgine_game_over_bad;
    GameState       emgine_game_state;
    size_t          emgine_intro_index = 0;
    uint32_t        emgine_frame_count = -1;
    bool            emgine_played_intro = false;
    bool            emgine_played_gameplay_audio = false;
    bool            emgine_is_player = false;
    uint32_t        emgine_player_score = 0;
    double          emgine_game_camera_x_offset = 0;
    double          emgine_game_camera_y_offset = 0;
    float           emgine_player_x_pos = 0.0f;
    float          emgine_player_y_pos = 0.0f;
    glm::vec2        emgine_camera_pos = glm::vec2(0, 0);
    int             emgine_players_uuid = -999;
    bool            emgine_player_is_at_center = false;
    int             emgine_player_health = 3;
    int             emgine_cooldown_frame_score = -180;
    int             emgine_cooldown_frame_health = -180;
    bool            emgine_played_bad_ending_audio = false;
    bool            emgine_played_good_ending_audio = false;
    bool            emgine_new_scene_loaded = false;
    double          emgine_player_movement_speed = 0.02;
    double          emgine_camera_ease_factor = 1.0;
    bool            emgine_new_scene = true;
    bool            emgine_x_flipping_on_movement = false;
    bool            emgine_player_is_moving = false;
    std::unordered_set<Actor*> emgine_actors_triggered_with_player;
    int             emgine_frame_player_took_damager = -1;
    bool            played_nearby_dialogue_sfx = false;

public:
    Emgine() : 
        quit(false), 
        emgine_game_state(GameState::INTRO)
        // emgine_game_camera_x_offset(in_game_cam_x_offset),
        // emgine_game_camera_y_offset(in_game_cam_y_offset),
        // emgine_camera_ease_factor(in_camera_ease_factor),
        // emgine_player_x_pos(in_player_x_pos),
        // emgine_player_y_pos(in_player_y_pos),
        // emgine_players_uuid(in_players_uuid),
        // emgine_player_movement_speed(in_player_movement_speed),
        // emgine_x_flipping_on_movement(in_x_flipping_on_movement)
        {};

    GameState get_game_state();
    static void read_json_file(const std::string& path, rapidjson::Document & out_document);
    void set_game_state(GameState in_game_state);
    void poll_events(SDL_Renderer* r, WindowRenderer* window);
    void update_all(glm::vec2 proposed_pos);

    void process_input(SDL_Renderer* r);
    
    void load_bad_ending_to_buffer(SDL_Renderer* r);
    void load_good_ending_to_buffer(SDL_Renderer* r);
    void move_player(glm::vec2 proposed_pos);
    void make_player_state_moving();
    void make_player_state_not_moving();

    void run_onstart_for_all_actors();
    void onupdate_for_all_actors();
    void onlateupdate_for_all_actors();
    void process_added_components();
    void process_removed_components();
    void actors_that_need_to_be_destroyed();
    void run_onstart_for_new_actors();
    void add_new_actors();

    ~Emgine();
};

#endif // EMGINE_H