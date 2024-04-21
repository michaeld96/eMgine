#ifndef COMPONENT_H
#define COMPONENT_H
#include "Common.h"

class Component
{
public:
    std::string type;
    std::shared_ptr<luabridge::LuaRef> lua_table;
    bool has_start;
    bool has_update;
    bool has_last_update;
    bool enabled;

    Component() : enabled(true), has_start(false), has_update(false), has_last_update(false) {}
    
    bool IsEnabled() const { return enabled; }
    void SetEnabled(bool value) { enabled = value; }

    // destructor 
    ~Component() 
    { 
        lua_table.reset();
        // std::cout << "lua table count " << lua_table.use_count() << std::endl;
        // std::cout << "Component destructor called" << std::endl; 
    }
};
#endif // COMPONENT_H
