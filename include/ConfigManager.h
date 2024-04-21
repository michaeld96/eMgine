#ifndef EMGINEUTILITY_H
#define EMGINEUTILITY_H
#include "Common.h"
#include <filesystem>
#include <cstdlib>
#include "rapidjson/filewritestream.h" 
#include "rapidjson/writer.h" 
#include "rapidjson/prettywriter.h"

class ConfigManager
{
private:
static std::string game_start_message;
static std::string game_over_bad_message;
static std::string game_over_good_message;
// static inline int x_resolution = 640;
// static inline int y_resolution = 360;
static int x_resolution;
static int y_resolution;
static const std::string decision_string;
static const std::string make_decision_string;
static inline std::string window_name = "";
static inline int window_width = 640;
static inline int window_height = 360;
static inline Uint8 clear_color_r = 255;
static inline Uint8 clear_color_g = 255;
static inline Uint8 clear_color_b = 255;
static inline float _zoom_factor = 1.0f;
static inline float _player_movement_speed = 0.02f; // default val



public:

static inline double camera_offset_x = 0;
static inline double camera_offset_y = 0;
static inline float _cam_ease_factor = 1.0f; // default val
static inline bool _x_scale_actor_flipping_on_movement = false;

static void check_resources_folder()
{
    const std::filesystem::path resource_path = "resources/";
    if (!std::filesystem::exists(resource_path)) {
        std::cout << "error: resources/ missing";
        exit(0);
    }
}

static void check_game_config_file()
{
    const std::filesystem::path game_config_path = "resources/game.config";
    if (!std::filesystem::exists(game_config_path)) {
        std::cout << "error: resources/game.config missing";
        exit(0);
    }
}

static bool exists_render_file()
{
    const std::filesystem::path game_config_path = "resources/rendering.config";
    if (std::filesystem::exists(game_config_path)) {
		return true;
    }
	return false;
}

static void get_game_start_message()
{
	rapidjson::Document document;
	read_json_file("resources/game.config", document);
	if (document.HasMember("game_start_message") && document["game_start_message"].IsString()) 
	{
		game_start_message = document["game_start_message"].GetString();
	}
}

static void get_game_over_bad_message()
{
	rapidjson::Document document;
	read_json_file("resources/game.config", document);
	if (document.HasMember("game_over_bad_message") && document["game_over_bad_message"].IsString()) 
	{
		game_over_bad_message = document["game_over_bad_message"].GetString();
	}
}

static void get_game_over_good_message()
{
	rapidjson::Document document;
	read_json_file("resources/game.config", document);
	if (document.HasMember("game_over_good_message") && document["game_over_good_message"].IsString()) 
	{
		game_over_good_message = document["game_over_good_message"].GetString();
	}
}

static void set_player_movement_speed()
{
	rapidjson::Document document;
	read_json_file("resources/game.config", document);
	if (document.HasMember("player_movement_speed"))
	{
		_player_movement_speed = document["player_movement_speed"].GetFloat();
	}
}	

static void set_up_rendering_config()
{
	
	// check to see if the file exists.
	const std::filesystem::path game_config_path = "resources/rendering.config";
	rapidjson::Document document;
	if (exists_render_file())
	{
		read_json_file("resources/rendering.config", document);
		if (document.HasMember("x_resolution"))
		{
			x_resolution = document["x_resolution"].GetInt();
		}
		if (document.HasMember("y_resolution"))
		{
			y_resolution = document["y_resolution"].GetInt();
		}
		if (document.HasMember("window_width"))
		{
			window_width = document["window_width"].GetInt();
		}
		if (document.HasMember("window_height"))
		{
			window_height = document["window_height"].GetInt();
		}
		if (document.HasMember("clear_color_r"))
		{
			clear_color_r = document["clear_color_r"].GetInt();
		}
		if (document.HasMember("clear_color_g"))
		{
			clear_color_g = document["clear_color_g"].GetInt();
		}
		if (document.HasMember("clear_color_b"))
		{
			clear_color_b = document["clear_color_b"].GetInt();
		}
		if (document.HasMember("cam_offset_x"))
		{
			camera_offset_x = document["cam_offset_x"].GetDouble();
		}
		if (document.HasMember("cam_offset_y"))
		{
			camera_offset_y = document["cam_offset_y"].GetDouble();
		}
		if (document.HasMember("zoom_factor"))
		{
			 _zoom_factor = document["zoom_factor"].GetFloat();
		}
		if (document.HasMember("cam_ease_factor"))
		{
			 _cam_ease_factor = document["cam_ease_factor"].GetDouble();
		}
		if (document.HasMember("x_scale_actor_flipping_on_movement"))
		{
			_x_scale_actor_flipping_on_movement = document["x_scale_actor_flipping_on_movement"].GetBool();
		}

	}
}

static void set_window_name()
{
	rapidjson::Document document;
	read_json_file("resources/game.config", document);
	if (document.HasMember("game_title") && document["game_title"].IsString()) 
	{
		window_name = document["game_title"].GetString();
	}
}

static void read_json_file(const std::string& path, rapidjson::Document & out_document)
{
	FILE* file_pointer = nullptr;
#ifdef _WIN32
	fopen_s(&file_pointer, path.c_str(), "rb");
#else
	file_pointer = fopen(path.c_str(), "rb");
#endif
	char buffer[65536];
	rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
	out_document.ParseStream(stream);
	std::fclose(file_pointer);

	if (out_document.HasParseError()) {
		rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
		std::cout << "error parsing json at [" << path << "]" << std::endl;
		exit(0);
	}
}

static void print_game_start_message()
{
	if (!game_start_message.empty())
	{
		std::cout << game_start_message << "\n";
	}
}

static void print_game_over_bad_message()
{
	std::cout << game_over_bad_message;
}

static void print_game_over_good_message()
{
	std::cout << game_over_good_message;
}

static int get_x_resolution()
{
	return x_resolution;
}

static int get_y_resolution()
{
	return y_resolution;
}

static void print_decision_string()
{
	std::cout << decision_string;
	std::cout << "\n";
}	

static void print_make_decision()
{
	std::cout << make_decision_string;
	std::cout << "\n";
}

static std::string get_window_name()
{
	return window_name;
}

static int get_window_width()
{
	return window_width;
}

static int get_window_height()
{
	return window_height;
}

static Uint8 get_clear_color_r()
{
	return clear_color_r;
}

static Uint8 get_clear_color_g()
{
	return clear_color_g;
}

static Uint8 get_clear_color_b()
{
	return clear_color_b;
}

static float get_zoom_factor()
{
	return _zoom_factor;
}

static float get_player_movement_speed()
{
	return _player_movement_speed;
}

static void write_json_file(const std::string& path, rapidjson::Document& document)
{
    // Open the file for writing
    FILE* file_pointer = nullptr;
#ifdef _WIN32
    fopen_s(&file_pointer, path.c_str(), "wb");
#else
    file_pointer = fopen(path.c_str(), "wb");
#endif
    if (!file_pointer) {
        std::cerr << "Failed to open " << path << " for writing.\n";
        return;
    }

    // Create a file write stream
    char writeBuffer[65536];
    rapidjson::FileWriteStream os(file_pointer, writeBuffer, sizeof(writeBuffer));

    // Create a writer and serialize the document
    // rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
    document.Accept(writer);
    
    // Close the file
    fclose(file_pointer);
}


};
#endif // EMGINEUTILITY_H