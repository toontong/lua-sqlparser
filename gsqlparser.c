#include "Parser.h"
#include "EnodeType.h"

#include "lua.h"

#define _GSQL_MODEL_NAME "gsqlparser"

static const struct luaL_Reg gsqlparser_module_functions[] = {
    {"Parser",          Parser_new},
    {NULL, NULL}
};

int luaopen_gsqlparser(lua_State *L) {

    Parser_register_on_luaopen(L);
    Enum_register_on_luaopen(L);

    luaL_register(L, _GSQL_MODEL_NAME, gsqlparser_module_functions);
    return 1;
}