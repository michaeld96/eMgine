#include "Component.h"
#include "ComponentManager.h"

// luabridge::LuaRef Component::deepCopyLuaTable(luabridge::LuaRef src)
// {
//     luabridge::LuaRef dest = luabridge::newTable(ComponentManager::_lua_state);
//     for (luabridge::Iterator iter(src); !iter.isNil(); ++iter)
//     {
//         luabridge::LuaRef key = iter.key();
//         luabridge::LuaRef value = iter.value();
//         if (value.isTable())
//         {
//             dest[key] = deepCopyLuaTable(value);
//         }
//         else
//         {
//             dest[key] = value;
//         }
//     }
//     // copying the metatable.
//     luabridge::LuaRef src_metatable = src["__metatable"];
//     if (src_metatable.isTable())
//     {
//         luabridge::LuaRef dest_metatable = deepCopyLuaTable(src_metatable);
//         dest_metatable.push(ComponentManager::_lua_state);
//         dest.push(ComponentManager::_lua_state);
//         lua_setmetatable(ComponentManager::_lua_state, -2);
//     }
//     // idk.
//     return dest;
// }