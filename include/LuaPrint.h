#ifndef LUAPRINT_H
#define LUAPRINT_H
#include "../libs/lua-5.4.6/src/lua.hpp"
#include "../libs/LuaBridge-2.8/Source/LuaBridge/LuaBridge.h"
#include <string>
#include <iostream>
void printLuaTable(luabridge::LuaRef table) {
    if (!table.isTable()) {
        std::cerr << "Error: Not a table" << std::endl;
        return;
    }

    for (luabridge::Iterator iter(table); !iter.isNil(); ++iter) {
        luabridge::LuaRef key = iter.key();
        luabridge::LuaRef value = iter.value();
        std::cout << key.tostring() << ": ";
        if (value.isTable()) {
            std::cout << "Table" << std::endl;
            printLuaTable(value);  // Recursively print nested tables
        } else {
            std::cout << value.tostring() << std::endl;
        }
    }
}

#endif // LUAPRINT_H