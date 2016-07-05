#ifndef _LUA_GSP_PARSER_STATEMENT_H_
#define _LUA_GSP_PARSER_STATEMENT_H_ 1
#include "gsp_base.h"
#include "gsp_node.h"
#include "gsp_list.h"
#include "gsp_sourcetoken.h"
#include "gsp_sqlparser.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static const char* StatementMetatable = "gsqlparser.Statement";

// Statement object of lua-userdata
typedef struct {
    gsp_sql_statement *_statement;
} Statement;

// Object register
int Statement_register_on_luaopen(lua_State *L);

// C utils methods
#define Statement_New(L)\
   if(L){\
          Statement *self = (Statement *)lua_newuserdata(L, sizeof(Statement));\
          self->_statement = NULL;\
          luaL_getmetatable(L, StatementMetatable);\
          lua_setmetatable(L, -2);\
   }

#define Statement_FromStatement(L, stmt)\
   if(L){\
           Statement *self = (Statement *)lua_newuserdata(L, sizeof(Statement));\
           self->_statement = stmt;\
           luaL_getmetatable(L, StatementMetatable);\
           lua_setmetatable(L, -2);\
   }

// Lua export Module methods
int Statement_dealloc(lua_State *L);


/* no-used methods
PyObject *Statement_getattr(Statement *obj, char *name);
int Statement_iterate_node_next(lua_State *L);
*/

// Lua export Object methods
int Statement_get_stmttype(lua_State *L);
int Statement_get_root(lua_State *L);
int Statement_remove_whereclause(lua_State *L);
int Statement_remove_orderby(lua_State *L);
int Statement_remove_groupby(lua_State *L);
int Statement_remove_havingclause(lua_State *L);
int Statement_remove_expression(lua_State *L);
int Statement_remove_joinitem(lua_State *L);
int Statement_remove_resultcolumn(lua_State *L);
int Statement_remove_orderbyitem(lua_State *L);
int Statement_remove_groupbyitem(lua_State *L);
int Statement_add_whereclause(lua_State *L);
int Statement_add_orderby(lua_State *L);
int Statement_add_groupby(lua_State *L);
int Statement_add_havingclause(lua_State *L);
int Statement_add_joinitem(lua_State *L);
int Statement_add_resultcolumn(lua_State *L);

// Lua export Object methods metatable
static const struct luaL_Reg Statement_methods_metatable[] = {
    {"__gc",                   Statement_dealloc},
    {"get_stmttype",           Statement_get_stmttype},
    {"get_root",               Statement_get_root}, // "get_root()\nGets the root :class:`Node` of the statement.\n\n:returns: :class:`Node`" },
    {"remove_whereclause",     Statement_remove_whereclause}, // "remove_whereclause(node)\nRemoves *node*'s Where clause (if it exists)\n\n:param node: A select statement Node\n:type node: :class:`Node`\n:returns: :class:`Node` -- The updated version of the *node*" },
    {"remove_orderby",         Statement_remove_orderby}, // "remove_orderby(node)\nRemoves *node*'s OrderBy clause (if it exists)\n\n:param node: A select statement Node\n:type node: :class:`Node`\n:returns: :class:`Node` -- The updated version of the *node*" },
    {"remove_groupby",         Statement_remove_groupby}, // "remove_groupby(node)\nRemoves *node*'s GroupBy clause (if it exists)\n\n:param node: A select statement Node\n:type node: :class:`Node`\n:returns: :class:`Node` -- The updated version of the *node*" },
    {"remove_havingclause",    Statement_remove_havingclause}, // "remove_havingclause(node)\nRemoves *node*'s Having clause (if it exists)\n\n:param node: A select statement Node\n:type node: :class:`Node`\n:returns: :class:`Node` -- The updated version of the *node*" },
    {"remove_expression",      Statement_remove_expression}, // "remove_expression(node)\nRemoves the expression *node*.\n\n:param node: An expression Node\n:type node: :class:`Node`\n:returns: :class:`Node` -- The updated version of the *node*" },
    {"remove_joinitem",        Statement_remove_joinitem}, // "remove_joinitem(node, index)\nRemoves *node*'s Join item at the specified *index*\n\n:param node: A base statement Node\n:type node: :class:`Node`\n:param index: The item's index in the Join clause\n:type index: int\n:returns: :class:`Node` -- The updated version of the *node*" },
    {"remove_resultcolumn",    Statement_remove_resultcolumn}, // "remove_resultcolumn(node, index)\nRemoves *node*'s Result column at the specified *index*\n\n:param node: A select statement Node\n:type node: :class:`Node`\n:param index: The item's index in the Join clause\n:type index: int\n:returns: :class:`Node` -- The updated version of the *node*" },
    {"remove_orderbyitem",     Statement_remove_orderbyitem}, // "remove_orderbyitem(node, index)\nRemoves *node*'s OrderBy item at the specified *index*\n\n:param node: A base statement Node\n:type node: :class:`Node`\n:param index: The item's index in the Join clause\n:type index: int\n:returns: :class:`Node` -- The updated version of the *node*" },
    {"remove_groupbyitem",     Statement_remove_groupbyitem}, // "remove_groupbyitem(node, index)\nRemoves *node*'s GroupBy item at the specified *index*\n\n:param node: A base statement Node\n:type node: :class:`Node`\n:param index: The item's index in the Join clause\n:type index: int\n:returns: :class:`Node` -- The updated version of the *node*" },
    {"add_whereclause",        Statement_add_whereclause}, // "add_whereclause(node, text)\nAdds a new Where clause to *node*.\n\n:param node: A base statement Node\n:type node: :class:`Node`\n:param text: String representation of a Where clause\n:type text: str\n:returns: str -- The updated query string" },
    {"add_orderby",            Statement_add_orderby}, // "add_orderby(node, text)\nAdds a new OrderBy clause to *node*.\n\n:param node: A select statement Node\n:type node: :class:`Node`\n:param text: String representation of an OrderBy clause\n:type text: str\n:returns: str -- The updated query string" },
    {"add_groupby",            Statement_add_groupby}, // "add_groupby(node, text)\nAdds a new GroupBy clause to *node*.\n\n:param node: A select statement Node\n:type node: :class:`Node`\n:param text: String representation of a GroupBy clause\n:type text: str\n:returns: str -- The updated query string" },
    {"add_havingclause",       Statement_add_havingclause}, // "add_havingclause(node, text)\nAdds a new Having clause to *node*.\n\n:param node: A select statement Node\n:type node: :class:`Node`\n:param text: String representation of a Having clause\n:type text: str\n:returns: str -- The updated query string" },
    {"add_joinitem",           Statement_add_joinitem}, // "add_joinitem(node, text)\nAdds a new Join item to *node*.\n\n:param node: A select statement Node\n:type node: :class:`Node`\n:param text: String representation of a Joint item\n:type text: str\n:returns: str -- The updated query string" },
    {"add_resultcolumn",       Statement_add_resultcolumn}, // "add_resultcolumn(node, text)\nAdds a new Result column to *node*.\n\n:param node: A base statement Node\n:type node: :class:`Node`\n:param text: String representation of a Result column\n:type text: str\n:returns: str -- The updated query string" },
    {NULL, NULL}  /* Sentinel */
};

#endif
