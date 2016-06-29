#include "Parser.h"

#include <lauxlib.h>
#include <lualib.h>

static const char* ParserMetaName = "gsqlparser.Parser";

int Parser_init_on_luaopen(lua_State *L){
    if(0 == luaL_newmetatable(L, ParserMetaName)){
       luaL_error(L, "userdata key already has the key tname");
    }
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);    /* pushes the metatable */
    lua_settable(L, -3); /* metatable.__index = metatable */

    luaL_openlib(L, NULL, Parser_methods, 0);
    luaL_openlib(L, "gsqlparser", Parser_functions, 0);
    return 1;
}

/* Allocate new Parser object */
int Parser_new(lua_State *L){
    int vendor = luaL_checkinteger(L, 1);
    Parser *p = (Parser *)lua_newuserdata(L, sizeof(Parser));
    p->vendor = vendor;
    if( gsp_parser_create((gsp_dbvendor) vendor, &p->_parser) != 0){
        luaL_error(L, "gsp_parser_create(vendor=%d) faild.", vendor);
        return 0;
    }
    printf("Parser_new\n");
    return 1;
}
/* Deallocate Parser object */
int Parser_free(lua_State *L) {
    Parser *p = (Parser *)luaL_checkudata(L, 1, ParserMetaName);
    if (p->_parser != NULL){
        printf("Parser_free\n");
        gsp_parser_free(p->_parser);
        p->_parser = NULL;
    }
    return 0;
}

int Parser_tostring(lua_State *L){
    printf("Parser_tostring\n");
    Parser *p = (Parser *)luaL_checkudata(L, 1, ParserMetaName);
    lua_pushfstring(L, "%s(vendor=%d)", ParserMetaName, p->vendor);
    return 1;
}

int Parser_check_syntax(lua_State *L){return 1;}
int Parser_tokenize(lua_State *L){return 1;}
int Parser_get_statement(lua_State *L){return 1;}
int Parser_get_statement_count(lua_State *L){return 1;}
int Parser_get_tokens(lua_State *L){return 1;}
