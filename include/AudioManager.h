#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H
#include "Common.h"
#include "ConfigManager.h"
#include "AudioHelper.h"
class AudioManager
{
public:
static inline std::unordered_map<std::string, Mix_Chunk*> loaded_audio;
static inline std::string intro_bgm;
static inline std::string gameplay_audio_name;
static inline std::string bad_ending_audio_name;
static inline std::string good_ending_audio_name;
static inline std::string score_sfx_name;
static inline std::string player_damager_sfx;
static inline std::string player_step_sfx;
static inline std::string nearby_dialogue_sfx;
static inline bool is_in_editor_mode = false;
static void load_intro_audio()
{
    rapidjson::Document document;
    ConfigManager::read_json_file("resources/game.config", document);
    if (document.HasMember("intro_bgm") && document["intro_bgm"].IsString())
    {
        std::string bgm = document["intro_bgm"].GetString();
        std::string bgm_wav = bgm + ".wav";
        std::string bgm_ogg = bgm + ".ogg";
        const std::filesystem::path game_config_path = "resources/audio";
        Mix_Chunk* sound = nullptr; 

        if (std::filesystem::exists(game_config_path / bgm_wav))
        {
            sound = AudioHelper::Mix_LoadWAV498((game_config_path / bgm_wav).string().c_str());
        }
        else if (std::filesystem::exists(game_config_path / bgm_ogg))
        {
            sound = AudioHelper::Mix_LoadWAV498((game_config_path / bgm_ogg).string().c_str());
        }
        else
        {
            std::cout << "error: missing audio " << bgm_wav << " or " << bgm_ogg;
            exit(0);
        }
        intro_bgm = bgm;
        loaded_audio[bgm] = sound;
    }
    if (document.HasMember("gameplay_audio") && document["gameplay_audio"].IsString())
    {
        std::string gameplay_audio = document["gameplay_audio"].GetString();
        std::string gameplay_audio_wav = gameplay_audio + ".wav";
        std::string gameplay_audio_ogg = gameplay_audio + ".ogg";
        const std::filesystem::path game_config_path = "resources/audio";
        Mix_Chunk* sound = nullptr; 

        if (std::filesystem::exists(game_config_path / gameplay_audio_wav))
        {
            sound = AudioHelper::Mix_LoadWAV498((game_config_path / gameplay_audio_wav).string().c_str());
        }
        else if (std::filesystem::exists(game_config_path / gameplay_audio_ogg))
        {
            sound = AudioHelper::Mix_LoadWAV498((game_config_path / gameplay_audio_ogg).string().c_str());
        }
        else
        {
            std::cout << "error: missing audio " << gameplay_audio_wav << " or " << gameplay_audio_ogg;
            exit(0);
        }
        loaded_audio[gameplay_audio] = sound;
        gameplay_audio_name = gameplay_audio;
    }
    if (document.HasMember("game_over_bad_audio") && document["game_over_bad_audio"].IsString())
    {
        std::string bad_audio = document["game_over_bad_audio"].GetString();
        std::string bad_audio_wav = bad_audio + ".wav";
        std::string bad_audio_ogg = bad_audio + ".ogg";
        const std::filesystem::path game_config_path = "resources/audio";
        Mix_Chunk* sound = nullptr; 

        if (std::filesystem::exists(game_config_path / bad_audio_wav))
        {
            sound = AudioHelper::Mix_LoadWAV498((game_config_path / bad_audio_wav).string().c_str());
        }
        else if (std::filesystem::exists(game_config_path / bad_audio_ogg))
        {
            sound = AudioHelper::Mix_LoadWAV498((game_config_path / bad_audio_ogg).string().c_str());
        }
        else
        {
            std::cout << "error: missing audio " << bad_audio_wav << " or " << bad_audio_ogg;
            exit(0);
        }
        loaded_audio[bad_audio] = sound;
        bad_ending_audio_name = bad_audio;
    }
    if (document.HasMember("game_over_good_audio") && document["game_over_good_audio"].IsString())
    {
        std::string good_audio = document["game_over_good_audio"].GetString();
        std::string good_audio_wav = good_audio + ".wav";
        std::string good_audio_ogg = good_audio + ".ogg";
        const std::filesystem::path game_config_path = "resources/audio";
        Mix_Chunk* sound = nullptr; 

        if (std::filesystem::exists(game_config_path / good_audio_wav))
        {
            sound = AudioHelper::Mix_LoadWAV498((game_config_path / good_audio_wav).string().c_str());
        }
        else if (std::filesystem::exists(game_config_path / good_audio_ogg))
        {
            sound = AudioHelper::Mix_LoadWAV498((game_config_path / good_audio_ogg).string().c_str());
        }
        else
        {
            std::cout << "error: missing audio " << good_audio_wav << " or " << good_audio_ogg;
            exit(0);
        }
        loaded_audio[good_audio] = sound;
        good_ending_audio_name = good_audio;
    }
    if (document.HasMember("score_sfx"))
    {
        std::string audio = document["score_sfx"].GetString();
        std::string audio_wav = audio + ".wav";
        std::string audio_ogg = audio + ".ogg";
        const std::filesystem::path game_config_path = "resources/audio";
        Mix_Chunk* sound = nullptr; 

        if (std::filesystem::exists(game_config_path / audio_wav))
        {
            sound = AudioHelper::Mix_LoadWAV498((game_config_path / audio_wav).string().c_str());
        }
        else if (std::filesystem::exists(game_config_path / audio_ogg))
        {
            sound = AudioHelper::Mix_LoadWAV498((game_config_path / audio_ogg).string().c_str());
        }
        else
        {
            std::cout << "error: missing audio " << audio_wav << " or " << audio_ogg;
            exit(0);
        }
        loaded_audio[audio] = sound;
        score_sfx_name = audio;
    }
}

static void load_player_damage_sfx(const std::string& audio_file_name)
{
    rapidjson::Document document;
    ConfigManager::read_json_file("resources/game.config", document);
    std::string audio_wav = audio_file_name + ".wav";
    std::string audio_ogg = audio_file_name + ".ogg";
    const std::filesystem::path game_config_path = "resources/audio";
    Mix_Chunk* sound = nullptr; 

    if (std::filesystem::exists(game_config_path / audio_wav))
    {
        sound = AudioHelper::Mix_LoadWAV498((game_config_path / audio_wav).string().c_str());
    }
    else if (std::filesystem::exists(game_config_path / audio_ogg))
    {
        sound = AudioHelper::Mix_LoadWAV498((game_config_path / audio_ogg).string().c_str());
    }
    else
    {
        std::cout << "error: missing audio " << audio_wav << " or " << audio_ogg;
        exit(0);
    }
    loaded_audio[audio_file_name] = sound;
    player_damager_sfx = audio_file_name;
}

static void load_player_step_sfx(const std::string& audio_file_name)
{
    rapidjson::Document document;
    ConfigManager::read_json_file("resources/game.config", document);
    std::string audio_wav = audio_file_name + ".wav";
    std::string audio_ogg = audio_file_name + ".ogg";
    const std::filesystem::path game_config_path = "resources/audio";
    Mix_Chunk* sound = nullptr; 

    if (std::filesystem::exists(game_config_path / audio_wav))
    {
        sound = AudioHelper::Mix_LoadWAV498((game_config_path / audio_wav).string().c_str());
    }
    else if (std::filesystem::exists(game_config_path / audio_ogg))
    {
        sound = AudioHelper::Mix_LoadWAV498((game_config_path / audio_ogg).string().c_str());
    }
    else
    {
        std::cout << "error: missing audio " << audio_wav << " or " << audio_ogg;
        exit(0);
    }
    loaded_audio[audio_file_name] = sound;
    player_step_sfx = audio_file_name;
}

static void load_nearby_dialogue_sfx(const std::string& audio_file_name)
{
    rapidjson::Document document;
    ConfigManager::read_json_file("resources/game.config", document);
    std::string audio_wav = audio_file_name + ".wav";
    std::string audio_ogg = audio_file_name + ".ogg";
    const std::filesystem::path game_config_path = "resources/audio";
    Mix_Chunk* sound = nullptr; 

    if (std::filesystem::exists(game_config_path / audio_wav))
    {
        sound = AudioHelper::Mix_LoadWAV498((game_config_path / audio_wav).string().c_str());
    }
    else if (std::filesystem::exists(game_config_path / audio_ogg))
    {
        sound = AudioHelper::Mix_LoadWAV498((game_config_path / audio_ogg).string().c_str());
    }
    else
    {
        std::cout << "error: missing audio " << audio_wav << " or " << audio_ogg;
        exit(0);
    }
    loaded_audio[audio_file_name] = sound;
}

static void init()
{
    if (AudioHelper::Mix_OpenAudio498(44100, MIX_DEFAULT_FORMAT, 1, 2048) < 0) 
    {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        exit(0);
    }
    AudioHelper::Mix_AllocateChannels498(50);
}

static void play_audio(std::string audio_name, int loops, int channel)
{
    Mix_Chunk* sound = loaded_audio[audio_name];
    AudioHelper::Mix_PlayChannel498(channel, sound, loops);
}


static void halt_audio()
{
    AudioHelper::Mix_HaltChannel498(0);
}

static void play_intro_music()
{
    play_audio(intro_bgm, -1, 0);
}

static void play_gameplay_audio()
{
    play_audio(gameplay_audio_name, -1, 0);
}

static bool found_intro_music()
{
    // look for "intro_bgm" in loaded_audio
    if (loaded_audio.find(intro_bgm) != loaded_audio.end())
    {
        return true;
    }
    return false;
}

static bool found_gamplay_audio()
{
    if (loaded_audio.find(gameplay_audio_name) != loaded_audio.end())
    {
        return true;
    }
    return false;
}

static void clean_up()
{
    for (auto& audio : loaded_audio)
    {
        Mix_FreeChunk(audio.second);
    }
    loaded_audio.clear();
    AudioHelper::Mix_AllocateChannels498(0);
    Mix_Quit();
}

static void play_score_sfx()
{
    play_audio(score_sfx_name, 0, 1);
}

static void play_damage_sfx()
{
    int channel = Helper::GetFrameNumber() % 48 + 2;
    play_audio(player_damager_sfx, 0, channel);
}

static void play_step_sfx()
{
    int channel = Helper::GetFrameNumber() % 48 + 2;
    play_audio(player_step_sfx, 0, channel);
}

static void play_dialogue_sfx(const std::string& audio_file_name)
{
    int channel = Helper::GetFrameNumber() % 48 + 2;
    play_audio(audio_file_name, 0, channel);
}

static Mix_Chunk* load_audio_clip(const std::string& clip_name)
{
    std::string wav_file = "resources/audio/" + clip_name + ".wav";
    std::string ogg_file = "resources/audio/" + clip_name + ".ogg";
    std::string use_this_file = "";
    if (std::filesystem::exists(wav_file))
    {
        use_this_file = wav_file;
    }
    else if (std::filesystem::exists(ogg_file))
    {
        use_this_file = ogg_file;
    }
    else
    {
        std::cout << "error: missing audio " << clip_name;
        exit(0);
    }
    // see if this audio clip is already loaded
    if (loaded_audio.find(clip_name) != loaded_audio.end())
    {
        return loaded_audio[clip_name];
    }
    Mix_Chunk* sound = AudioHelper::Mix_LoadWAV498(use_this_file.c_str());
    if (sound == nullptr)
    {
        std::cout << "error: missing audio " << use_this_file;
        exit(0);
    }
    // cache for future use
    loaded_audio[clip_name] = sound;
    return sound;
}

static void Play(int channel, const std::string& clip_name, bool does_loop)
{
    if (is_in_editor_mode)
    {
        return;
    }
    // load the clip
    Mix_Chunk* sound = load_audio_clip(clip_name);
    // play the clip
    AudioHelper::Mix_PlayChannel498(channel, sound, does_loop ? -1 : 0);
}

static void Halt(int channel)
{
    AudioHelper::Mix_HaltChannel498(channel);
}

static void SetVolume(int channel, int volume)
{
    AudioHelper::Mix_Volume498(channel, volume);
}

};
#endif // AUDIO_MANAGER_H
