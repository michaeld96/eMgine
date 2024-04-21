#include "InputManager.h"
#include "ComponentManager.h"

void InputManager::init()
{
    for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; code++)
    {
        _keyboard_states[static_cast<SDL_Scancode>(code)] = INPUT_STATE_UP;
    }
}

void InputManager::process_event(const SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN)
    {
        _keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_DOWN;
        _just_became_down_scancodes.push_back(e.key.keysym.scancode);
    }
    else if (e.type == SDL_KEYUP)
    {
        _keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_UP;
        _just_became_up_scancodes.push_back(e.key.keysym.scancode);
    }
    else if (e.type == SDL_MOUSEMOTION)
    {
        _mouse_position.x = static_cast<float>(e.motion.x);
        _mouse_position.y = static_cast<float>(e.motion.y);
    }
    else if (e.type == SDL_MOUSEWHEEL)
    {
        _mouse_scroll_this_frame = static_cast<float>(e.wheel.preciseY);
    }
    else if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        _mouse_button_states[e.button.button] = INPUT_STATE_JUST_BECAME_DOWN;
        _just_became_down_mouse_buttons.push_back(e.button.button);
    }
    else if (e.type == SDL_MOUSEBUTTONUP)
    {
        _mouse_button_states[e.button.button] = INPUT_STATE_JUST_BECAME_UP;
        _just_became_up_mouse_buttons.push_back(e.button.button);
    }
}

void InputManager::last_update()
{
    for (const auto& scancode : _just_became_down_scancodes)
    {
        _keyboard_states[scancode] = INPUT_STATE_DOWN;
    }
    _just_became_down_scancodes.clear();

    for (const auto& scancode : _just_became_up_scancodes)
    {
        _keyboard_states[scancode] = INPUT_STATE_UP;
    }
    _just_became_up_scancodes.clear();

    for (const auto& button : _just_became_down_mouse_buttons)
    {
        _mouse_button_states[button] = INPUT_STATE_DOWN;
    }
    _just_became_down_mouse_buttons.clear();

    for (const auto& button : _just_became_up_mouse_buttons)
    {
        _mouse_button_states[button] = INPUT_STATE_UP;
    }
    _just_became_up_mouse_buttons.clear();

    _mouse_scroll_this_frame = 0.0f;
}

bool InputManager::get_key(SDL_Scancode keycode)
{
    auto itr = _keyboard_states.find(keycode);
    if (itr != _keyboard_states.end())
    {
        return itr->second == INPUT_STATE_DOWN || itr->second == INPUT_STATE_JUST_BECAME_DOWN;
    }
    return false;
}

bool InputManager::get_key_down(SDL_Scancode keycode)
{
    auto itr = _keyboard_states.find(keycode);
    if (itr != _keyboard_states.end())
    {
        return itr->second == INPUT_STATE_JUST_BECAME_DOWN;
    }
    return false;
}

bool InputManager::get_key_up(SDL_Scancode keycode)
{
    auto itr = _keyboard_states.find(keycode);
    if (itr != _keyboard_states.end())
    {
        return itr->second == INPUT_STATE_JUST_BECAME_UP;
    }
    return false;
}

bool InputManager::string_get_key(const std::string& key)
{
    auto itr = __keycode_to_scancode.find(key);
    if (itr != __keycode_to_scancode.end())
    {
        return get_key(itr->second);
    }
    return false;
}

bool InputManager::string_get_key_down(const std::string& key)
{
    auto itr = __keycode_to_scancode.find(key);
    if (itr != __keycode_to_scancode.end())
    {
        return get_key_down(itr->second);
    }
    return false;
}

bool InputManager::string_get_key_up(const std::string& key)
{
    auto itr = __keycode_to_scancode.find(key);
    if (itr != __keycode_to_scancode.end())
    {
        return get_key_up(itr->second);
    }
    return false;
}

glm::vec2 InputManager::GetMousePosition()
{
    return _mouse_position;
}

bool InputManager::GetMouseButton(int button)
{
    auto itr = _mouse_button_states.find(button);
    if (itr != _mouse_button_states.end())
    {
        return itr->second == INPUT_STATE_DOWN || itr->second == INPUT_STATE_JUST_BECAME_DOWN;
    }
    return false;
}

bool InputManager::GetMouseButtonDown(int button)
{
    auto itr = _mouse_button_states.find(button);
    if (itr != _mouse_button_states.end())
    {
        return itr->second == INPUT_STATE_JUST_BECAME_DOWN;
    }
    return false;
}

bool InputManager::GetMouseButtonUp(int button)
{
    auto itr = _mouse_button_states.find(button);
    if (itr != _mouse_button_states.end())
    {
        return itr->second == INPUT_STATE_JUST_BECAME_UP;
    }
    return false;
}

float InputManager::GetMouseScrollDelta()
{
    return _mouse_scroll_this_frame;
}