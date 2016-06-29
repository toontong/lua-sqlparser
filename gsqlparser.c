#include "Parser.h"

#include "lua.h"


int luaopen_gsqlparser(lua_State *L) {
    Parser_init_on_luaopen(L);
    return 1;
}
