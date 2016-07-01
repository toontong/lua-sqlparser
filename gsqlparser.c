#include "Parser.h"
#include "Statement.h"
#include "Node.h"
#include "ENodeType.h"

#include "lua.h"

#define _GSQL_MODEL_NAME "gsqlparser"

static const struct luaL_Reg gsqlparser_module_functions[] = {
    {"NewParser",          Parser_new},
    // {"NewNode",            Node_new},
    // {"NewStatement",       Statement_new},
    {NULL, NULL}
};

int luaopen_gsqlparser(lua_State *L) {
    printf("luaopen\n");
    
    Parser_register_on_luaopen(L);
    
    Node_register_on_luaopen(L);
    
    Statement_register_on_luaopen(L);

    Enum_register_on_luaopen(L);

    luaL_register(L, _GSQL_MODEL_NAME, gsqlparser_module_functions);
    return 1;
}
