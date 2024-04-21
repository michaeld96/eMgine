#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H
#include "Common.h"
#include <unordered_map>
enum INPUT_STATE { INPUT_STATE_UP, INPUT_STATE_JUST_BECAME_DOWN, INPUT_STATE_DOWN, INPUT_STATE_JUST_BECAME_UP };

const std::unordered_map<std::string, SDL_Scancode> __keycode_to_scancode = {
	// Directional (arrow) Keys
	{"up", SDL_SCANCODE_UP},
	{"down", SDL_SCANCODE_DOWN},
	{"right", SDL_SCANCODE_RIGHT},
	{"left", SDL_SCANCODE_LEFT},

	// Misc Keys
	{"escape", SDL_SCANCODE_ESCAPE},

	// Modifier Keys
	{"lshift", SDL_SCANCODE_LSHIFT},
	{"rshift", SDL_SCANCODE_RSHIFT},
	{"lctrl", SDL_SCANCODE_LCTRL},
	{"rctrl", SDL_SCANCODE_RCTRL},
	{"lalt", SDL_SCANCODE_LALT},
	{"ralt", SDL_SCANCODE_RALT},

	// Editing Keys
	{"tab", SDL_SCANCODE_TAB},
	{"return", SDL_SCANCODE_RETURN},
	{"enter", SDL_SCANCODE_RETURN},
	{"backspace", SDL_SCANCODE_BACKSPACE},
	{"delete", SDL_SCANCODE_DELETE},
	{"insert", SDL_SCANCODE_INSERT},

	// Character Keys
	{"space", SDL_SCANCODE_SPACE},
	{"a", SDL_SCANCODE_A},
	{"b", SDL_SCANCODE_B},
	{"c", SDL_SCANCODE_C},
	{"d", SDL_SCANCODE_D},
	{"e", SDL_SCANCODE_E},
	{"f", SDL_SCANCODE_F},
	{"g", SDL_SCANCODE_G},
	{"h", SDL_SCANCODE_H},
	{"i", SDL_SCANCODE_I},
	{"j", SDL_SCANCODE_J},
	{"k", SDL_SCANCODE_K},
	{"l", SDL_SCANCODE_L},
	{"m", SDL_SCANCODE_M},
	{"n", SDL_SCANCODE_N},
	{"o", SDL_SCANCODE_O},
	{"p", SDL_SCANCODE_P},
	{"q", SDL_SCANCODE_Q},
	{"r", SDL_SCANCODE_R},
	{"s", SDL_SCANCODE_S},
	{"t", SDL_SCANCODE_T},
	{"u", SDL_SCANCODE_U},
	{"v", SDL_SCANCODE_V},
	{"w", SDL_SCANCODE_W},
	{"x", SDL_SCANCODE_X},
	{"y", SDL_SCANCODE_Y},
	{"z", SDL_SCANCODE_Z},
	{"0", SDL_SCANCODE_0},
	{"1", SDL_SCANCODE_1},
	{"2", SDL_SCANCODE_2},
	{"3", SDL_SCANCODE_3},
	{"4", SDL_SCANCODE_4},
	{"5", SDL_SCANCODE_5},
	{"6", SDL_SCANCODE_6},
	{"7", SDL_SCANCODE_7},
	{"8", SDL_SCANCODE_8},
	{"9", SDL_SCANCODE_9},
	{"/", SDL_SCANCODE_SLASH},
	{";", SDL_SCANCODE_SEMICOLON},
	{"=", SDL_SCANCODE_EQUALS},
	{"-", SDL_SCANCODE_MINUS},
	{".", SDL_SCANCODE_PERIOD},
	{",", SDL_SCANCODE_COMMA},
	{"[", SDL_SCANCODE_LEFTBRACKET},
	{"]", SDL_SCANCODE_RIGHTBRACKET},
	{"\\", SDL_SCANCODE_BACKSLASH},
	{"'", SDL_SCANCODE_APOSTROPHE}
};

class InputManager
{
private:
	static inline std::unordered_map<SDL_Scancode, INPUT_STATE> _keyboard_states;
	static inline std::vector<SDL_Scancode> _just_became_down_scancodes;
	static inline std::vector<SDL_Scancode> _just_became_up_scancodes;
	static inline glm::vec2 _mouse_position = {0.0f, 0.0f};
	static inline float _mouse_scroll_this_frame = 0.0f;
	static inline std::unordered_map<int, INPUT_STATE> _mouse_button_states;
	static inline std::vector<int> _just_became_down_mouse_buttons;
	static inline std::vector<int> _just_became_up_mouse_buttons;

public:
	// Call before main loop begins.
	static void init();
	// Call every frame at start of event loop.
	static void process_event(const SDL_Event& e);
	static void last_update();
	static bool get_key(SDL_Scancode keycode);
	static bool get_key_down(SDL_Scancode keycode);
	static bool get_key_up(SDL_Scancode keycode);
	static bool string_get_key(const std::string& key);
	static bool string_get_key_down(const std::string& key);
	static bool string_get_key_up(const std::string& key);
	static glm::vec2 GetMousePosition();
	static bool GetMouseButton(int button);
	static bool GetMouseButtonDown(int button);
	static bool GetMouseButtonUp(int button);
	static float GetMouseScrollDelta();
};
#endif // INPUT_MANAGER_H
