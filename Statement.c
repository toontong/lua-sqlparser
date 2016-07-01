#include "Statement.h"
#include "Node.h"
#include "modifysql.h"

static const char* StatementMetatable = "gsqlparser.Statement";

// Initialize Statement Type module
int Statement_register_on_luaopen(lua_State *L){
    if(0 == luaL_newmetatable(L, StatementMetatable)){
       luaL_error(L, "userdata key already has the key tname");
    }
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);    /* pushes the metatable */
    lua_settable(L, -3); /* metatable.__index = metatable */

    if(!lua_istable(L, -1)){
      luaL_error(L,"must table");
    }
    luaL_register(L, NULL, Statement_methods_metatable);
    return 1;
}

int Statement_new(lua_State *L)
{
    Statement *self = (Statement *)lua_newuserdata(L, sizeof(Statement));

    if (self != NULL) {
        luaL_error(L,"new userdata of{%s} fail.", StatementMetatable);
        return 0;
    }
    self->_statement = NULL;
    luaL_getmetatable(L, StatementMetatable);
    lua_setmetatable(L, -2);
    return 1;
}

int Statement_dealloc(lua_State *L)
{   
    Statement *self = (Statement *)luaL_checkudata(L, 1, StatementMetatable);
    if(self && self->_statement != NULL){
        printf("free(self._statement) ???\n");
    }
    return 0;
}

Statement *Statement_FromStatement(lua_State *L, gsp_sql_statement *stmt) {
    Statement *self;
    int rc = Statement_new(L);
    if(rc == 1){
        self = (Statement *)lua_gettop(L);
        self->_statement = stmt;
        lua_pushvalue(L, self);
        return self;
    }
    return NULL;
}

// Get root node
// Statement.get_root()
int Statement_get_root(lua_State *L) 
{
    SqlNode *n;
    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    //printf("Statement_get_root\n");
    if (self == NULL || self->_statement == NULL || self->_statement->stmt == NULL) {
        luaL_error(L, "Empty statement.");
        return 0;
    }

    //n = (SqlNode*) Node_FromNode( ((Statement*)self)->_statement->parseTree, (Statement*) self );
    n = Node_FromNode(L, (gsp_node*) ((Statement*)self)->_statement->stmt, (Statement*) self );
    if (n == NULL){
        luaL_error(L, "Empty Node.");
        return 0;
    }

    return 1;
}

// Statement.remove_whereclause(node)
int Statement_remove_whereclause(lua_State *L) 
{
    SqlNode *newNode;
    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);
    assert(self != NULL);
    if (NULL == node) {
        luaL_error(L, "remove_whereclause() takes exactly one Node argument");
        return NULL;
    }

    gsp_removeWhereClause((gsp_base_statement*) ((SqlNode*)node)->_node);
    newNode = Node_FromNode(L, ((SqlNode*)node)->_node, (Statement*) self);

    return NULL == newNode ? 1 : 0;
}

// Statement.remove_orderby(selectNode)
int Statement_remove_orderby(lua_State *L)
{
    SqlNode *newNode;
    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);
    assert(self != NULL);

    if (NULL == node) {
        luaL_error(L, "remove_orderby() takes exactly one node argument");
        return NULL;        
    }

    gsp_removeOrderBy((gsp_selectStatement*) ((SqlNode*)node)->_node);
    newNode = Node_FromNode(L, ((SqlNode*)node)->_node, (Statement*) self);

    return NULL == newNode ? 1 : 0;
}


// Statement.remove_groupby(selectNode)
int Statement_remove_groupby(lua_State *L)
{
    SqlNode *newNode;
    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);

    if (NULL == node) {
        luaL_error(L, "remove_groupby() takes exactly one node argument");
        return NULL;        
    }

    gsp_removeGroupBy((gsp_selectStatement*) ((SqlNode*)node)->_node);
    newNode = Node_FromNode(L, ((SqlNode*)node)->_node, (Statement*) self);

    return NULL == newNode ? 1 : 0;
}


// Statement.remove_havingclause(selectNode)
int Statement_remove_havingclause(lua_State *L)
{
    SqlNode *newNode;
    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);

    if (NULL == node) {
        luaL_error(L, "remove_havingclause() takes exactly one node argument");
        return NULL;        
    }

    gsp_removeHavingClause((gsp_selectStatement*) ((SqlNode*)node)->_node);
    newNode = Node_FromNode(L, ((SqlNode*)node)->_node, (Statement*) self);

    return NULL == newNode ? 1 : 0;
}


// Statement.remove_expression(exprNode)
int Statement_remove_expression(lua_State *L)
{
    SqlNode *newNode;
    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);

    if (NULL == node) {
        luaL_error(L, "remove_expression() takes exactly one node argument");
        return NULL;
    }

    gsp_removeExpression((gsp_expr*) ((SqlNode*)node)->_node);
    newNode = Node_FromNode(L, ((SqlNode*)node)->_node, (Statement*) self);

    return NULL == newNode ? 1 : 0;
}


// Statement.remove_joinitem(selectNode, joinIndex)
int Statement_remove_joinitem(lua_State *L)
{
    SqlNode *newNode;
    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);
    int i = luaL_checkint(L, 3);

    if (NULL == node) {
        luaL_error(L, "remove_joinitem() takes exactly two arguments(Node, int)");
        return NULL;
    }

    gsp_removeJoinItem((gsp_selectStatement*) ((SqlNode*)node)->_node, i);
    newNode = Node_FromNode(L, ((SqlNode*)node)->_node, (Statement*) self);

    return NULL == newNode ? 1 : 0;
}


// Statement.remove_resultcolumn(node, columnIndex)
int Statement_remove_resultcolumn(lua_State *L)
{
    SqlNode *newNode;
    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);
    int i = luaL_checkint(L, 3);

    if (NULL == node) {
        luaL_error(L, "remove_resultcolumn() takes exactly two arguments(Node, int)");
        return NULL;
    }

    gsp_removeResultColumn((gsp_base_statement*) ((Statement*)self)->_statement->stmt, i);
    newNode = Node_FromNode(L, (gsp_node*)((Statement*)self)->_statement->stmt, (Statement*) self);
    return NULL == newNode ? 1 : 0;
}


// Statement.remove_orderbyitem(node, orderByIndex)
int Statement_remove_orderbyitem(lua_State *L)
{
    SqlNode *newNode;
    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);
    int i = luaL_checkint(L, 3);
    if (NULL == node) {
        luaL_error(L, "remove_orderbyitem() takes exactly two arguments(Node, int)");
        return NULL;
    }

    gsp_removeOrderByItem((gsp_selectStatement*) ((SqlNode*)node)->_node, i);
    newNode = Node_FromNode(L, ((SqlNode*)node)->_node, (Statement*) self);

    return NULL == newNode ? 1 : 0;
}


// Statement.remove_groupbyitem(selectNode, groupbyIndex)
int Statement_remove_groupbyitem(lua_State *L)
{
    SqlNode *newNode;
    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);
    int i = luaL_checkint(L, 3);
    if (NULL == node) {
        luaL_error(L, "remove_groupby() takes exactly two arguments(Node, int)");
        return NULL;
    }

    gsp_removeGroupByItem((gsp_selectStatement*) ((SqlNode*)node)->_node, i);
    newNode = Node_FromNode(L, ((SqlNode*)node)->_node, (Statement*) self);

    return NULL == newNode ? 1 : 0;
}

int Statement_add_whereclause(lua_State *L)
{
    char *newQuery;

    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);
    char *text = luaL_checkstring(L, 3);

    if (NULL == node || text == NULL) {
        luaL_error(L, "add_whereclause() takes exactly two arguments(Node, string)");
        return NULL;
    }

    //printf("add_whereclause: %s\n", text);
    //printf("old: %s\n", gsp_getNodeText( (gsp_node*) ((Statement*)self)->_statement->stmt));
    gsp_addWhereClause( ((Statement*)self)->_statement->sqlparser, (gsp_base_statement*) ((SqlNode*)node)->_node, text);
    //printf("new: %s\n", gsp_getNodeText( (gsp_node*) ((Statement*)self)->_statement->stmt));

    newQuery = gsp_getNodeText( (gsp_node*) ((Statement*)self)->_statement->stmt );
    if(newQuery != NULL){
        lua_pushstring(L, newQuery);
        free(newQuery);
        return 1;
    }

    return 0;
}

int Statement_add_orderby(lua_State *L)
{
    char *newQuery;

    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);
    char *text = luaL_checkstring(L, 3);

    if (NULL == node || text == NULL) {
        luaL_error(L, "add_orderby() takes exactly two arguments(Node, string)");
        return NULL;
    }

    gsp_addOrderBy( ((Statement*)self)->_statement->sqlparser, (gsp_selectStatement*) ((SqlNode*)node)->_node, text);

    newQuery = gsp_getNodeText( (gsp_node*) ((Statement*)self)->_statement->stmt );
    if(newQuery != NULL){
        lua_pushstring(L, newQuery);
        free(newQuery);
        return 1;
    }

    return 0;
}

int Statement_add_groupby(lua_State *L)
{
    char *newQuery;

    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);
    char *text = luaL_checkstring(L, 3);

    if (NULL == node || text == NULL) {
        luaL_error(L, "add_groupby() takes exactly two arguments(Node, string)");
        return NULL;
    }

    gsp_addGroupBy( ((Statement*)self)->_statement->sqlparser, (gsp_selectStatement*) ((SqlNode*)node)->_node, text);

    newQuery = gsp_getNodeText( (gsp_node*) ((Statement*)self)->_statement->stmt );
    if(newQuery != NULL){
        lua_pushstring(L, newQuery);
        free(newQuery);
        return 1;
    }
    return 0;
}

int Statement_add_havingclause(lua_State *L)
{
    char *newQuery;

    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);
    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);
    char *text = luaL_checkstring(L, 3);

    if (NULL == node || text == NULL) {
        luaL_error(L, "add_havingclause() takes exactly two arguments(Node, string)");
        return NULL;
    }

    gsp_addHavingClause( ((Statement*)self)->_statement->sqlparser, (gsp_selectStatement*) ((SqlNode*)node)->_node, text);

    newQuery = gsp_getNodeText( (gsp_node*) ((Statement*)self)->_statement->stmt );
    if(newQuery != NULL){
        lua_pushstring(L, newQuery);
        free(newQuery);
        return 1;
    }
    return 0;
}

int Statement_add_joinitem(lua_State *L)
{
    char *newQuery;

    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);

    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);
    char *text = luaL_checkstring(L, 3);

    if (NULL == node || text == NULL) {
        luaL_error(L, "add_joinitem() takes exactly two arguments(Node, string)");
        return NULL;
    }

    gsp_addJoinItem( ((Statement*)self)->_statement->sqlparser, (gsp_selectStatement*) ((SqlNode*)node)->_node, text);

    newQuery = gsp_getNodeText( (gsp_node*) ((Statement*)self)->_statement->stmt );
    if(newQuery != NULL){
        lua_pushstring(L, newQuery);
        free(newQuery);
        return 1;
    }
    return 0;
}

int Statement_add_resultcolumn(lua_State *L)
{
    char *newQuery;

    Statement *self = (Statement*) luaL_checkudata(L, 1, StatementMetatable);

    SqlNode *node = (SqlNode *)luaL_checkudata(L, 2, NodeMetatable);
    char *text = luaL_checkstring(L, 3);

    if (NULL == node || text == NULL) {
        luaL_error(L, "add_resultcolumn() takes exactly two argument(Node, string)s");
        return NULL;
    }

    gsp_addResultColumn( ((Statement*)self)->_statement->sqlparser, (gsp_base_statement*) ((SqlNode*)node)->_node, text);
    newQuery = gsp_getNodeText( (gsp_node*) ((Statement*)self)->_statement->stmt );
    if(newQuery != NULL){
        lua_pushstring(L, newQuery);
        free(newQuery);
        return 1;
    }

    return 0;
}

