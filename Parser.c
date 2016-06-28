

#include "Parser.h"

#include <lauxlib.h>
#include <lualib.h>

static int Parser_init_on_luaopen(lua_State *L){
    luaL_newmetatable(L, ParserMetaName);
    /* metatable.__index = metatable */
    lua_pushvalue(L, -1);  /* duplicates the metatable */
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, Parser_methods, 0);
    luaL_newlib(L, Parser_functions);
}

/* Allocate new Parser object */
static int Parser_new(lua_State *L){
    int vendor = luaL_checkint(L, 1);
    Parser *p = (Parser *)lua_newuserdata(L, sizeof(Parser));
    p->vendor = vendor;
    if( gsp_parser_create((gsp_dbvendor) vendor, &p->_parser) != 0){
        luaL_error(L, "gsp_parser_create(vendor=%d) faild.", vendor);
        return 0;
    }
    return 1;
}
/* Deallocate Parser object */
static int Parser_free(lua_State *L) {
    Parser *p = (Parser *)luaL_checkudata(L, 1, ParserMetaName);
    if (p->_parser != NULL){
        //printf("Parser_free\n");
        gsp_parser_free(p->_parser);
        p->_parser = NULL;
    }
    return 0;
}

static int Parser_check_syntax(lua_State *L){return 1;}
static int Parser_tokenize(lua_State *L){return 1;}
static int Parser_get_statement(lua_State *L){return 1;}
static int Parser_get_statement_count(lua_State *L){return 1;}
static int Parser_get_tokens(lua_State *L){return 1;}