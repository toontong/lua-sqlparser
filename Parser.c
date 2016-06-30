#include "Parser.h"

#include <lauxlib.h>
#include <lualib.h>

static const char* ParserMetaName = "_metatable.Parser";

/* 建仓一个名为ParserMetaName的全局table变量,填充Parser_methods函数表 */
int Parser_register_on_luaopen(lua_State *L){
    if(0 == luaL_newmetatable(L, ParserMetaName)){
       luaL_error(L, "userdata key already has the key tname");
    }
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);    /* pushes the metatable */
    lua_settable(L, -3); /* metatable.__index = metatable */

    if(!lua_istable(L, -1)){
      luaL_error(L,"must table");
    }
   
    luaL_register(L, NULL, Parser_methods);
    return 1;
}

/* 创建对你Pasrer，同时把全局的ParserMetaName函数表设置给它 */
int Parser_new(lua_State *L){
    int vendor = luaL_checkinteger(L, 1);
    Parser *p = (Parser *)lua_newuserdata(L, sizeof(Parser));
    p->vendor = vendor;
    if( gsp_parser_create((gsp_dbvendor) vendor, &p->_parser) != 0){
        luaL_error(L, "gsp_parser_create(vendor=%d) faild.", vendor);
        return 0;
    }

    luaL_getmetatable(L, ParserMetaName);
    lua_setmetatable(L, -2);
    return 1;
}
/* Deallocate Parser object */
int Parser_free(lua_State *L) {
    Parser *p = (Parser *)luaL_checkudata(L, 1, ParserMetaName);
    if (p->_parser != NULL){
    //    printf("Parser_free(gsp_parser_free())\n");
        gsp_parser_free(p->_parser);
        p->_parser = NULL;
    }
    return 0;
}

int Parser_tostring(lua_State *L){
    Parser *p = (Parser *)luaL_checkudata(L, 1, ParserMetaName);
    lua_pushfstring(L, "%s(vendor=%d)", ParserMetaName, p->vendor);
    return 1;
}

int Parser_check_syntax(lua_State *L){return 1;}
int Parser_tokenize(lua_State *L){return 1;}
int Parser_get_statement(lua_State *L){return 1;}
int Parser_get_statement_count(lua_State *L){return 1;}
int Parser_get_tokens(lua_State *L){return 1;}