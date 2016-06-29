#ifndef _LUA_GSP_PARSER_PARSER_H_
#define _LUA_GSP_PARSER_PARSER_H_ 1
#include "gsp_base.h"
#include "gsp_node.h"
#include "gsp_list.h"
#include "gsp_sourcetoken.h"
#include "gsp_sqlparser.h"

#include <lua.h>
#include "lauxlib.h"
#include "lualib.h"

// Parser object
typedef struct {
    gsp_sqlparser *_parser;
    int vendor;
} Parser;

int Parser_new(lua_State *L); /* Allocate new Parser object */
int Parser_free(lua_State *L); /* Deallocate Parser object */
int Parser_check_syntax(lua_State *L);
int Parser_tokenize(lua_State *L);
int Parser_get_statement(lua_State *L);
int Parser_get_statement_count(lua_State *L);
int Parser_tostring(lua_State *L);
int Parser_init_on_luaopen(lua_State *L);

static const struct luaL_Reg Parser_functions[] = {
    {"Parser",          Parser_new},
    {"__gc",                    Parser_free},
    {NULL, NULL}
};

static const struct luaL_Reg Parser_methods[] = {
    {"__tostring",              Parser_tostring},
    {"check_syntax",            Parser_check_syntax},
    {"tokenize",                Parser_tokenize},
    {"get_statement",           Parser_get_statement},
    {"get_statement_count",     Parser_get_statement_count},
    {NULL, NULL}
};

#endif
