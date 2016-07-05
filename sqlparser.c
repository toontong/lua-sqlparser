#include "Parser.h"
#include "Statement.h"
#include "Node.h"
#include "ENodeType.h"

#include "lua.h"

#define _GSQL_MODEL_NAME "sqlparser"

int new_node(lua_State *L){
   Node_new(L);
   lua_pop(L, 1);
   luaL_getmetatable(L, NodeMetatable);
   lua_setmetatable(L, -2);
   return 1;
}

static const struct luaL_Reg sqlparser_module_functions[] = {
    {"NewParser",          Parser_new},
    {"NewNode",            new_node},
    // {"NewStatement",       Statement_new},
    {NULL, NULL}
};

int luaopen_sqlparser(lua_State *L) {
    Node_register_on_luaopen(L);
    
    Statement_register_on_luaopen(L);

    Parser_register_on_luaopen(L);
        
    luaL_register(L, _GSQL_MODEL_NAME, sqlparser_module_functions);
   
    Enum_register_on_luaopen(L);
    
    return 1;
}
