#include <stdio.h>
#include <string.h>
#include <string.h>
#include <assert.h>

#include <lauxlib.h>
#include <lualib.h>

#include "Parser.h"
#include "stack_debug.h"
#include "Statement.h"

#include "node_visitor.h"
#include "gsp_base.h"
#include "gsp_node.h"
#include "gsp_list.h"
#include "gsp_sourcetoken.h"
#include "gsp_sqlparser.h"

static const char* ParserMetatable = "gsqlparser.Parser";

/* 建仓一个名为ParserMetatable的全局table变量,填充Parser_methods函数表 */
int Parser_register_on_luaopen(lua_State *L){
    if(0 == luaL_newmetatable(L, ParserMetatable)){
       luaL_error(L, "userdata key already has the key tname");
    }
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);    /* pushes the metatable */
    lua_settable(L, -3); /* metatable.__index = metatable */

    if(!lua_istable(L, -1)){
      luaL_error(L,"must table");
    }
   
    luaL_register(L, NULL, Parser_metatable);
    return 1;
}

/* 创建对你Pasrer，同时把全局的ParserMetatable函数表设置给它 */
int Parser_new(lua_State *L){
    int vendor = luaL_checkinteger(L, 1);
    Parser *p = (Parser *)lua_newuserdata(L, sizeof(Parser));
    p->vendor = vendor;
    if( gsp_parser_create((gsp_dbvendor) vendor, &p->_parser) != 0){
        luaL_error(L, "gsp_parser_create(vendor=%d) faild.", vendor);
        return 0;
    }
    assert(lua_isuserdata (L, -1));

    luaL_getmetatable(L, ParserMetatable);
    lua_setmetatable(L, -2);
    return 1;
}
/* Deallocate Parser object */
int Parser_free(lua_State *L) {
    Parser *p = (Parser *)luaL_checkudata(L, 1, ParserMetatable);
    if (p->_parser != NULL){
        // printf("Parser_free(gsp_parser_free())\n");
        gsp_parser_free(p->_parser);
        p->_parser = NULL;
    }
    return 0;
}

int Parser_tostring(lua_State *L){
    Parser *p = (Parser *)luaL_checkudata(L, 1, ParserMetatable);
    lua_pushfstring(L, "%s(vendor=%d)", ParserMetatable, p->vendor);
    return 1;
}

int Parser_check_syntax(lua_State *L){
    int rc;
    Parser *p = (Parser *)luaL_checkudata(L, 1, ParserMetatable);
    const char *query = luaL_checkstring(L, 2);

    if (query == NULL || strlen(query)==0){
       luaL_error(L, "args 1:sql can not be empty string.");
       return 0;
    }

    rc = gsp_check_syntax(p->_parser, query);
    lua_pushnumber(L, rc);
    return 1;

}

int Parser_tokenize(lua_State *L){
    char * token_str;
    struct gsp_sourcetoken *token;
    int i, rc;
    int token_cnt;

    Parser *parser = (Parser *)luaL_checkudata(L, 1, ParserMetatable);
    char *query = luaL_checkstring(L, 2);

    rc = gsp_tokenize(parser->_parser, query);
    if (rc != 0) {
        luaL_error(L, "tokenize() failed, rc=[%d]", rc);
        return 0;
    }

    token_cnt = parser->_parser->number_of_token;
    lua_newtable(L); 
    for(i=0; i< token_cnt; i++){
        token = &parser->_parser->sourcetokenlist[i];
        // TODO: any unicode handling?
        token_str = gsp_token_text(token);
        if (!token_str) {
            lua_gettop(L); // drop stack top ?
            luaL_error(L, "tokenize() invalid token");
            return 0;
        }
        lua_pushstring(L, token_str);
        free(token_str);
        lua_rawseti (L, -2, i+1);
    }
    return 1;
}

// get nth statement
// Parser.get_statement(n)
int Parser_get_statement(lua_State *L) {
    int n = -1;
    gsp_sql_statement *stmt;
    Statement *statement;
    Parser *parser = (Parser *)luaL_checkudata(L, 1, ParserMetatable);
    n = luaL_checkinteger(L, 2);

    if (parser->_parser == NULL || n < 0 || n >= parser->_parser->nStatement) {
        luaL_error(L, "get_statement() index out of bounds");
        return 0;
    }

    stmt = &parser->_parser->pStatement[n];
    
    if (stmt->parseTree == NULL && stmt->stmt == NULL) {
        luaL_error(L, "Invalid syntax");
        return 0;
    }
    Statement_FromStatement(L, stmt);
    return statement != NULL ? 1 : 0;
}

// Parser.get_statement_count()
int Parser_get_statement_count(lua_State *L){
    Parser *parser = (Parser *)luaL_checkudata(L, 1, ParserMetatable);
    if (parser==NULL || parser->_parser == NULL){
        return 0;
    }
    lua_pushnumber(L, (double)parser->_parser->nStatement);
    return 1;
}
