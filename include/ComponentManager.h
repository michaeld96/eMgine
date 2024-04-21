#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H
#include "Common.h"
#include "MapHelper.h"
#include "Helper.h"
#include "InputManager.h"
#include "TextManager.h"
#include "AudioManager.h"
#include "ImageManager.h"
#include "Camera.h"

class ComponentManager
{
public:
static inline lua_State* _lua_state; // underscore is a convention for private members.
static inline std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> _component_tables;
static inline int global_component_counter = 0;


static void init_lua_state()
{
    // we are creating the state.
    _lua_state = luaL_newstate();
    // now we are getting the standard libraries.
    luaL_openlibs(_lua_state);
}

static void init_components()
{
    std::string directory_path_str = "resources/component_types/";
    if (std::filesystem::exists(directory_path_str) && std::filesystem::is_directory(directory_path_str))
    {
        for (const auto & entry : std::filesystem::directory_iterator(directory_path_str))
        {
            std::string file_path_str = entry.path().string();
            std::string component_name = entry.path().stem().string();  // Get the filename without the extension
            const char* file_path = file_path_str.c_str();
            // we are loading the lua file.
            if (luaL_dofile(_lua_state, file_path) != LUA_OK)
            {
                std::cout << "problem with lua file " << component_name;
                exit(0);
            }
            // if the file is OK, we are going to get the table and load it into the lua state.
            _component_tables.insert({component_name, 
                std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(_lua_state, component_name.c_str()))});
        }
    }
}

static void init_functions();

static void establish_inheritance(luabridge::LuaRef& derived, luabridge::LuaRef& base)
{
    luabridge::LuaRef new_metatable = luabridge::newTable(_lua_state);
    new_metatable["__index"] = base;

    // pushing derived and new_metatable onto the Lua stack.
    derived.push(_lua_state);
    new_metatable.push(_lua_state);
    lua_setmetatable(_lua_state, -2);
    lua_pop(_lua_state, 1);
}

/******** FUNCTIONS FOR LUA TO CPP********/
// For Debug.
static void DebugPrint(const std::string& message)
{
    std::cout << message << std::endl;
}

static void DebugPrintError(const std::string& message)
{
    std::cerr << message << std::endl;
}



// For Application
static void ApplicationQuit()
{
    exit(0);
}

static void ApplicationSleep(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

static int ApplicationGetFrame()
{
    return Helper::GetFrameNumber();
}

static void ApplicationOpenUrl(const std::string& url)
{
    #ifdef _WIN32
    std::string command = "start " + url;
    #elif __APPLE__
    std::string command = "open " + url;
    #else
    std::string command = "xdg-open " + url;
    #endif
    system(command.c_str());
}

static std::string generate_component_key()
{
    return "r" + std::to_string(global_component_counter++);
}

static void cleanup()
{
    global_component_counter = 0;
    _component_tables.clear();
}

static void close_lua_state()
{
    lua_close(_lua_state);
}

};
#endif // COMPONENT_MANAGER_H
