#include "Statement.h"
#include "Node.h"
#include "cstring.h"
#include "modifysql.h"

#include <assert.h>

// Initialize this Type
int Node_register_on_luaopen(lua_State *L) {
    // Fill node parsing functions
    printf("node reg...\n");
    //Node_init();

    if(0 == luaL_newmetatable(L, NodeMetatable)){
       luaL_error(L, "userdata key already has the key tname");
    }
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);    /* pushes the metatable */
    lua_settable(L, -3); /* metatable.__index = metatable */

    if(!lua_istable(L, -1)){
      luaL_error(L,"must table");
    }
    luaL_register(L, NULL, Node_methods_metatable);
    return 1;
}

/*char* gsp_getNodeText( gsp_node* node )
{
    gsp_sourcetoken* startToken = NULL;
    gsp_sourcetoken* endToken = NULL;
    gsp_sourcetoken* currentToken = NULL;
    CString* content;
    char* rc; 

    if(node == NULL)
        return NULL;
    if (node->nodeType == t_gsp_list)
    {
        if(((gsp_list*)node)->length>0){
            startToken = (gsp_list_first((gsp_list *)node))->fragment.startToken;
            endToken = (gsp_list_last((gsp_list *)node))->fragment.endToken;
        }

    }else{
        startToken = node->fragment.startToken;
        endToken = node->fragment.endToken;
    }

    currentToken = startToken;
    if(currentToken == NULL)
        return NULL;


    while( startToken!=NULL && startToken!=endToken && startToken->tokenStatus == ets_deleted){
        startToken = startToken->pNext;
    }

    if(startToken == NULL || startToken->tokenStatus == ets_deleted){
        startToken = NULL;
        endToken = NULL;
        return NULL;
    }
    else{
        while(endToken!=NULL && endToken!=startToken && endToken->tokenStatus == ets_deleted){
            endToken = endToken->pPrev;
        }

        if(endToken == NULL || endToken->tokenStatus == ets_deleted){
            startToken = NULL;
            endToken = NULL;
            return NULL;
        }

        content = CStringNew();

        if(currentToken->tokenStatus!=ets_deleted)
            CStringNAppend(content, currentToken->pStr, currentToken->nStrLen);

        while(currentToken != endToken && currentToken->pNext!=NULL){
            currentToken = currentToken->pNext;
            if(currentToken->tokenStatus!=ets_deleted)
                CStringNAppend(content, currentToken->pStr, currentToken->nStrLen);
            if(currentToken == endToken)
                break;
        }

        rc = content->buffer;
        CStringDeleteWithoutBuffer(content);
        return rc;
    }
}*/

char* gsp_getSimpleNodeText(gsp_node* node, gsp_sqlparser *parser)
{
    gsp_sourcetoken* startToken = NULL;
    gsp_sourcetoken* endToken = NULL;
    gsp_sourcetoken* currentToken = NULL;
    CString* content;
    char* rc;

    if (node == NULL)
        return NULL;
    if (node->nodeType == t_gsp_list)
    {
        if (((gsp_list*)node)->length>0){
            startToken = (gsp_list_first((gsp_list *)node))->fragment.startToken;
            endToken = (gsp_list_last((gsp_list *)node))->fragment.endToken;
        }

    }
    else{
        startToken = node->fragment.startToken;
        endToken = node->fragment.endToken;
    }

    currentToken = startToken;
    if (currentToken == NULL)
        return NULL;


    while (startToken != NULL && startToken != endToken && startToken->tokenStatus == ets_deleted){
        startToken = startToken->pNext;
    }

    if (startToken == NULL || startToken->tokenStatus == ets_deleted){
        startToken = NULL;
        endToken = NULL;
        return NULL;
    }
    else{
        int start, stop, len;

        while (endToken != NULL && endToken != startToken && endToken->tokenStatus == ets_deleted){
            endToken = endToken->pPrev;
        }

        if (endToken == NULL || endToken->tokenStatus == ets_deleted){
            startToken = NULL;
            endToken = NULL;
            return NULL;
        }

        content = CStringNew();

        if (startToken == endToken) {
            start = startToken->nColumn - 1;
            stop = start + startToken->nStrLen;
            len = startToken->nStrLen;
        }
        else {
            start = startToken->nColumn - 1;
            stop = endToken->nColumn + endToken->nStrLen - 1;
            len = stop - start;
        }


        //printf("%d, %d, %d\n", start, stop, len);

        CStringNAppend(content, parser->sqltext + start, len);

        rc = content->buffer;
        CStringDeleteWithoutBuffer(content);
        return rc;
    }
}

int Node_get_text(lua_State *L)
{
    char *tmp;
    SqlNode *self = (SqlNode *)luaL_checkudata(L, 1, NodeMetatable);
    //tmp = gsp_getNodeText(self->_node);
    tmp = gsp_getSimpleNodeText(self->_node, self->_parser);
    if (tmp == NULL) {
        lua_pushstring(L, "");
    } else {
        lua_pushstring(L, tmp);
    }

    return 1;
}

int Node_get_position(lua_State *L)
{
    SqlNode *self = (SqlNode *)luaL_checkudata(L, 1, NodeMetatable);
    if (self == NULL){
        luaL_error(L, "`%s` expected", NodeMetatable);
        return 0;
    }

    if (self->_node != NULL && self->_node->fragment.startToken) {
        char *name;

        name = gsp_getSimpleNodeText(self->_node, self->_parser);

        if (name == NULL) {
            return 0;
        }

        lua_pushinteger(L, self->_node->fragment.startToken->nColumn);
        lua_pushlstring(L, name, strlen(name));
        return 2;
    }
    return 0;
}

/* Provide a (standard) iterator if it's a list node
SqlNode *Node_list_iterator(PyObject *o) {
    SqlNode *node = (SqlNode*) o;

    if (node->_node->nodeType == t_gsp_list) {
        PyObject *iter = PyObject_GetIter(PyDict_GetItemString(node->dict, "list"));
        Py_XDECREF(iter);
        return iter;
    }

    Py_RETURN_NONE;
}*/

int Node_getattro(lua_State *L)
{
    SqlNode *self = (SqlNode *)luaL_checkudata(L, 1, NodeMetatable);
    if (self == NULL){
        luaL_error(L, "`%s` expected", NodeMetatable);
        return 0;
    }
    const char* name = luaL_checkstring(L, 2);

    if (strcmp(name, "node_text") == 0) {
        return Node_get_text(L);
    }

    lua_pushlightuserdata(L, self);

    assert(lua_istable(L, -1));

    lua_getfield(L, -1, name);
    return 1;
}

int Node_set_attr_n(lua_State *L, const char* attr, int v)
{
    lua_pushstring(L, attr);
    lua_pushinteger(L, v);
    lua_settable(L, -3); /*mean: table[key] = value */
}

int Node_dealloc(lua_State *L)
{
    SqlNode *p = (SqlNode *)luaL_checkudata(L, 1, NodeMetatable);
    if (p == NULL){
        luaL_error(L, "`%s` expected", NodeMetatable);
        return 0;
    }
    if (p->_node != NULL){
        printf("TODO: free the Node->_node\n");
    }
    if (p->_parser != NULL){
        printf("TODO: free the Node->_parser\n");
    }
    return 1;
}
// create a SqlNode onto the stack top and return it
SqlNode *Node_new(lua_State *L)
{
    SqlNode *self = (SqlNode *)lua_newuserdata(L, sizeof(SqlNode));
    if (self == NULL) {
        luaL_error(L, "new Node() faild.");
        return NULL;
    }
    self->_node = NULL;
    self->_parser = NULL;
    luaL_getmetatable(L, NodeMetatable);
    lua_setmetatable(L, -2);
    // assert((SqlNode *)lua_gettop(L) == self);
    return self;
}

SqlNode *Node_FromNode(lua_State *L, gsp_node *node, Statement *stmt) {
    // PyObject *self;
    SqlNode *self;

    // SqlNode *self = (SqlNode *)luaL_checkudata(L, 1, NodeMetatable);
    // if (self == NULL){
    //  luaL_error(L, "`%s` expected", NodeMetatable)
    //  return 0;
    // }
    printf("Node_FromNode(%d) : %p\n", node->nodeType, node);
    if (Node_parse_functions[node->nodeType] != NULL) {
        self = Node_parse_functions[node->nodeType](L, node, stmt);
        if (self == NULL){
            luaL_error(L, "`%s` expected", NodeMetatable);
            return 0;
        }

            lua_getmetatable(L, -1); // push metatable of self onto the stack top.
            Node_set_attr_n(L,"node_type", node->nodeType);
            // SqlNode *p = (SqlNode *)lua_gettop(L); // pop self?
            //assert(p == self);
            if (node->nodeType == t_gsp_list) {
                /*name = PyString_FromString("LIST");
                PyDict_SetItemString(((SqlNode*)self)->dict, "node_text", name);
                Py_XDECREF(name);*/
            } else {
                /*tmp = gsp_getNodeText(node);
                name = PyString_FromString(tmp);
                gsp_free(tmp);
                PyDict_SetItemString(((SqlNode*)self)->dict, "node_text", name);
                Py_XDECREF(name);*/
            }
    } else {
        printf("NO PARSER FOR NODE TYPE: %d\n", node->nodeType);
        self = Node_new(L);
        if(self == NULL)
            return NULL;

        lua_getmetatable(L, -1); // push metatable of self onto the stack top.
        Node_set_attr_n(L,"node_type", node->nodeType);
    }

    printf("Node_FromNode: %p %p\n", stmt->_statement, stmt->_statement->sqlparser);
    self->_parser = stmt->_statement->sqlparser;
    return self;
}



#define ADD_TOKEN(L, path, name) if (path->name) { lua_getmetatable(L, -1); lua_pushstring(L, #name); lua_pushlstring(L, path->name->pStr, path->name->nStrLen); lua_settable(L, -3);} else {lua_getmetatable(L, -1); lua_pushstring(L, #name); lua_pushnil(L);lua_settable(L, -3); }

#define ADD_INT(L, path, name) if (true) {lua_getmetatable(L, -1); lua_pushstring(L, #name); lua_pushinteger(L, path->name);lua_settable(L, -3); }
#define ADD_NODE(L, path, name) if (path->name) { lua_getmetatable(L, -1);lua_pushstring(L, #name);SqlNode *o = Node_FromNode(L, (gsp_node*)path->name, stmt); lua_settable(L, -3); } else {lua_getmetatable(L, -1); lua_pushstring(L, #name);lua_pushnil(L); lua_settable(L, -3); }

// #define ADD_LIST(d, path, name) if (path->name) { PyObject *o = Node_parsepath->name; PyDict_SetItemString(d, #name, o); Py_XDECREF(o); } else { PyDict_SetItemString(d, #name, Py_None); } 








SqlNode *Node_parse_list(lua_State *L, gsp_node *node, Statement *stmt)
{
    struct gsp_listcell *cell;

    // generate new Node object
    SqlNode *obj;
    obj = (SqlNode*) Node_new(L); obj->_node = node;
    int i = 0;
    // New list
    lua_newtable(L);
    foreach(cell, ((gsp_list*)node)) {
        // generate new Node object from list item
        SqlNode *o;

        lua_pushinteger(L, i); 
        i++;
        o = Node_FromNode(L, (gsp_node*)cell->node, stmt);
        lua_settable(L, -3);
    }

    // Add list to node
    lua_pushstring(L, "list");
    lua_insert(L, -2);
    lua_settable(L, -3); /*mean: table[key] = table{} */

    return obj;
}

/// AUTO-GENERATED PARSING FUNCTIONS
SqlNode *Node_parse_listcell(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_listcell*)node), node);
    ADD_NODE(L, ((gsp_listcell*)node), nextCell);
    return obj;
}
SqlNode *Node_parse_sql_statement(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_sql_statement*)node), stmtType);
    ADD_NODE(L, ((gsp_sql_statement*)node), parseTree);
    ADD_INT(L, ((gsp_sql_statement*)node), start_token_pos);
    ADD_INT(L, ((gsp_sql_statement*)node), end_token_pos);
    ADD_INT(L, ((gsp_sql_statement*)node), bCteQuery);
    ADD_INT(L, ((gsp_sql_statement*)node), isParsed);
    ADD_INT(L, ((gsp_sql_statement*)node), dummyTag);
    return obj;
}
SqlNode *Node_parse_dummy(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_dummy*)node), node1);
    ADD_INT(L, ((gsp_dummy*)node), int1);
    return obj;
}
SqlNode *Node_parse_constant(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_TOKEN(L, ((gsp_constant*)node), signToken);
    ADD_TOKEN(L, ((gsp_constant*)node), stringToken);
    ADD_INT(L, ((gsp_constant*)node), constantType);
    return obj;
}
SqlNode *Node_parse_objectname(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_objectname*)node), objectType);
    ADD_TOKEN(L, ((gsp_objectname*)node), serverToken);
    ADD_TOKEN(L, ((gsp_objectname*)node), databaseToken);
    ADD_TOKEN(L, ((gsp_objectname*)node), schemaToken);
    ADD_TOKEN(L, ((gsp_objectname*)node), objectToken);
    ADD_TOKEN(L, ((gsp_objectname*)node), partToken);
    ADD_TOKEN(L, ((gsp_objectname*)node), propertyToken);
    ADD_NODE(L, ((gsp_objectname*)node), fields);
    ADD_INT(L, ((gsp_objectname*)node), nTokens);
    ADD_NODE(L, ((gsp_objectname*)node), dblink);
    ADD_NODE(L, ((gsp_objectname*)node), indices);
    return obj;
}
SqlNode *Node_parse_expr(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_expr*)node), expressionType);
    ADD_NODE(L, ((gsp_expr*)node), objectOperand);
    ADD_TOKEN(L, ((gsp_expr*)node), operatorToken);
    ADD_NODE(L, ((gsp_expr*)node), leftOperand);
    ADD_NODE(L, ((gsp_expr*)node), rightOperand);
    ADD_NODE(L, ((gsp_expr*)node), subQueryNode);
    ADD_NODE(L, ((gsp_expr*)node), subQueryStmt);
    ADD_NODE(L, ((gsp_expr*)node), constantOperand);
    ADD_NODE(L, ((gsp_expr*)node), exprList);
    ADD_NODE(L, ((gsp_expr*)node), functionCall);
    ADD_NODE(L, ((gsp_expr*)node), objectAccess);
    ADD_NODE(L, ((gsp_expr*)node), caseExpression);
    ADD_TOKEN(L, ((gsp_expr*)node), quantifier);
    ADD_TOKEN(L, ((gsp_expr*)node), notToken);
    ADD_NODE(L, ((gsp_expr*)node), likeEscapeOperand);
    ADD_NODE(L, ((gsp_expr*)node), betweenOperand);
    ADD_NODE(L, ((gsp_expr*)node), arrayAccess);
    ADD_NODE(L, ((gsp_expr*)node), dataTypeName);
    ADD_NODE(L, ((gsp_expr*)node), intervalExpression);
    ADD_NODE(L, ((gsp_expr*)node), indices);
    ADD_NODE(L, ((gsp_expr*)node), newVariantTypeArgumentList);
    return obj;
}
SqlNode *Node_parse_objectAccess(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_objectAccess*)node), objectExpr);
    ADD_NODE(L, ((gsp_objectAccess*)node), attributes);
    ADD_NODE(L, ((gsp_objectAccess*)node), method);
    return obj;
}
SqlNode *Node_parse_aliasClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_aliasClause*)node), aliasName);
    ADD_TOKEN(L, ((gsp_aliasClause*)node), AsToken);
    ADD_NODE(L, ((gsp_aliasClause*)node), nameList);
    return obj;
}
SqlNode *Node_parse_resultColumn(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_resultColumn*)node), expr);
    ADD_NODE(L, ((gsp_resultColumn*)node), aliasClause);
    return obj;
}
SqlNode *Node_parse_trimArgument(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_TOKEN(L, ((gsp_trimArgument*)node), both_trailing_leading);
    ADD_NODE(L, ((gsp_trimArgument*)node), stringExpression);
    ADD_NODE(L, ((gsp_trimArgument*)node), trimCharacter);
    return obj;
}
SqlNode *Node_parse_orderByItem(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_orderByItem*)node), sortKey);
    ADD_TOKEN(L, ((gsp_orderByItem*)node), sortToken);
    return obj;
}
SqlNode *Node_parse_orderBy(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_orderBy*)node), items);
    return obj;
}
SqlNode *Node_parse_keepDenseRankClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_keepDenseRankClause*)node), orderBy);
    return obj;
}
SqlNode *Node_parse_analyticFunction(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_analyticFunction*)node), keepDenseRankClause);
    ADD_NODE(L, ((gsp_analyticFunction*)node), partitionBy_ExprList);
    ADD_NODE(L, ((gsp_analyticFunction*)node), orderBy);
    return obj;
}
SqlNode *Node_parse_functionCall(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_functionCall*)node), functionType);
    ADD_NODE(L, ((gsp_functionCall*)node), functionName);
    ADD_INT(L, ((gsp_functionCall*)node), aggregateType);
    ADD_NODE(L, ((gsp_functionCall*)node), trimArgument);
    ADD_NODE(L, ((gsp_functionCall*)node), analyticFunction);
    ADD_NODE(L, ((gsp_functionCall*)node), Args);
    ADD_TOKEN(L, ((gsp_functionCall*)node), extract_time_token);
    ADD_NODE(L, ((gsp_functionCall*)node), expr1);
    ADD_NODE(L, ((gsp_functionCall*)node), expr2);
    ADD_NODE(L, ((gsp_functionCall*)node), expr3);
    ADD_NODE(L, ((gsp_functionCall*)node), dataTypename);
    ADD_NODE(L, ((gsp_functionCall*)node), windowDef);
    ADD_NODE(L, ((gsp_functionCall*)node), sortClause);
    ADD_NODE(L, ((gsp_functionCall*)node), sortList);
    return obj;
}
SqlNode *Node_parse_whenClauseItem(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_whenClauseItem*)node), comparison_expr);
    ADD_NODE(L, ((gsp_whenClauseItem*)node), stmts);
    ADD_NODE(L, ((gsp_whenClauseItem*)node), return_expr);
    ADD_NODE(L, ((gsp_whenClauseItem*)node), countFractionDescriptionList);
    return obj;
}
SqlNode *Node_parse_caseExpression(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_caseExpression*)node), input_expr);
    ADD_NODE(L, ((gsp_caseExpression*)node), else_expr);
    ADD_NODE(L, ((gsp_caseExpression*)node), whenClauseItemList);
    ADD_NODE(L, ((gsp_caseExpression*)node), else_statement_node_list);
    return obj;
}
SqlNode *Node_parse_intervalExpression(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_precisionScale(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_precisionScale*)node), precision);
    ADD_NODE(L, ((gsp_precisionScale*)node), scale);
    return obj;
}
SqlNode *Node_parse_typename(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); 
    obj->_node = node;
    ADD_INT(L, ((gsp_typename*)node), dataType);
    ADD_NODE(L, ((gsp_typename*)node), precisionScale);
    ADD_NODE(L, ((gsp_typename*)node), secondsPrecision);
    ADD_NODE(L, ((gsp_typename*)node), length);
    ADD_NODE(L, ((gsp_typename*)node), genericName);
    ADD_NODE(L, ((gsp_typename*)node), indices);
    ADD_NODE(L, ((gsp_typename*)node), datatypeAttributeList);
    return obj;
}
SqlNode *Node_parse_keyReference(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); 
    obj->_node = node;  
    ADD_INT(L, ((gsp_keyReference*)node), referenceType);
    return obj;
}
SqlNode *Node_parse_keyAction(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_keyAction*)node), actionType);
    ADD_NODE(L, ((gsp_keyAction*)node), keyReference);
    return obj;
}
SqlNode *Node_parse_constraint(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_constraint*)node), constraintType);
    ADD_NODE(L, ((gsp_constraint*)node), constraintName);
    ADD_NODE(L, ((gsp_constraint*)node), checkCondition);
    ADD_NODE(L, ((gsp_constraint*)node), columnList);
    ADD_NODE(L, ((gsp_constraint*)node), automaticProperties);
    ADD_NODE(L, ((gsp_constraint*)node), referencedObject);
    ADD_NODE(L, ((gsp_constraint*)node), referencedColumnList);
    ADD_NODE(L, ((gsp_constraint*)node), keyActions);
    ADD_NODE(L, ((gsp_constraint*)node), defaultValue);
    ADD_NODE(L, ((gsp_constraint*)node), seedExpr);
    ADD_NODE(L, ((gsp_constraint*)node), incrementExpr);
    return obj;
}
SqlNode *Node_parse_mergeInsertClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mergeInsertClause*)node), columnList);
    ADD_NODE(L, ((gsp_mergeInsertClause*)node), valuelist);
    ADD_NODE(L, ((gsp_mergeInsertClause*)node), deleteWhereClause);
    return obj;
}
SqlNode *Node_parse_mergeUpdateClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mergeUpdateClause*)node), updateColumnList);
    ADD_NODE(L, ((gsp_mergeUpdateClause*)node), updateWhereClause);
    ADD_NODE(L, ((gsp_mergeUpdateClause*)node), deleteWhereClause);
    return obj;
}
SqlNode *Node_parse_mergeDeleteClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_mergeWhenClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mergeWhenClause*)node), condition);
    ADD_NODE(L, ((gsp_mergeWhenClause*)node), updateClause);
    ADD_NODE(L, ((gsp_mergeWhenClause*)node), insertClause);
    ADD_NODE(L, ((gsp_mergeWhenClause*)node), deleteClause);
    ADD_NODE(L, ((gsp_mergeWhenClause*)node), signalStmt);
    return obj;
}
SqlNode *Node_parse_dataChangeTable(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_dataChangeTable*)node), stmtNode);
    return obj;
}
SqlNode *Node_parse_fromTable(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_fromTable*)node), aliasClause);
    ADD_INT(L, ((gsp_fromTable*)node), fromtableType);
    ADD_NODE(L, ((gsp_fromTable*)node), subQueryNode);
    ADD_NODE(L, ((gsp_fromTable*)node), tableName);
    ADD_NODE(L, ((gsp_fromTable*)node), pxGranule);
    ADD_NODE(L, ((gsp_fromTable*)node), tableSample);
    ADD_NODE(L, ((gsp_fromTable*)node), flashback);
    ADD_NODE(L, ((gsp_fromTable*)node), joinExpr);
    ADD_NODE(L, ((gsp_fromTable*)node), tableExpr);
    ADD_TOKEN(L, ((gsp_fromTable*)node), tableonly);
    ADD_NODE(L, ((gsp_fromTable*)node), dataChangeTable);
    ADD_NODE(L, ((gsp_fromTable*)node), tableHints);
    ADD_NODE(L, ((gsp_fromTable*)node), functionCall);
    ADD_NODE(L, ((gsp_fromTable*)node), openQuery);
    ADD_NODE(L, ((gsp_fromTable*)node), openDatasource);
    ADD_NODE(L, ((gsp_fromTable*)node), containsTable);
    ADD_NODE(L, ((gsp_fromTable*)node), freeTable);
    ADD_NODE(L, ((gsp_fromTable*)node), openRowSet);
    ADD_NODE(L, ((gsp_fromTable*)node), openXML);
    ADD_NODE(L, ((gsp_fromTable*)node), pivotClause);
    ADD_NODE(L, ((gsp_fromTable*)node), unPivotClause);
    ADD_NODE(L, ((gsp_fromTable*)node), rowList);
    ADD_NODE(L, ((gsp_fromTable*)node), outerClause);
    return obj;
}
SqlNode *Node_parse_table(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_table*)node), tableSource);
    ADD_NODE(L, ((gsp_table*)node), aliasClause);
    ADD_NODE(L, ((gsp_table*)node), tableName);
    ADD_NODE(L, ((gsp_table*)node), tableExpr);
    return obj;
}
SqlNode *Node_parse_mergeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mergeSqlNode*)node), cteList);
    ADD_NODE(L, ((gsp_mergeSqlNode*)node), whenClauses);
    ADD_NODE(L, ((gsp_mergeSqlNode*)node), targetTableNode);
    ADD_NODE(L, ((gsp_mergeSqlNode*)node), usingTableNode);
    ADD_NODE(L, ((gsp_mergeSqlNode*)node), condition);
    ADD_NODE(L, ((gsp_mergeSqlNode*)node), outputClause);
    ADD_NODE(L, ((gsp_mergeSqlNode*)node), columnList);
    ADD_NODE(L, ((gsp_mergeSqlNode*)node), targetTable);
    ADD_NODE(L, ((gsp_mergeSqlNode*)node), usingTable);
    return obj;
}
SqlNode *Node_parse_alterTableOption(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_alterTableOption*)node), optionType);
    ADD_NODE(L, ((gsp_alterTableOption*)node), constraintName);
    ADD_NODE(L, ((gsp_alterTableOption*)node), newConstraintName);
    ADD_NODE(L, ((gsp_alterTableOption*)node), columnName);
    ADD_NODE(L, ((gsp_alterTableOption*)node), newColumnName);
    ADD_NODE(L, ((gsp_alterTableOption*)node), referencedObjectName);
    ADD_NODE(L, ((gsp_alterTableOption*)node), newTableName);
    ADD_NODE(L, ((gsp_alterTableOption*)node), indexName);
    ADD_NODE(L, ((gsp_alterTableOption*)node), columnDefinitionList);
    ADD_NODE(L, ((gsp_alterTableOption*)node), constraintList);
    ADD_NODE(L, ((gsp_alterTableOption*)node), columnNameList);
    ADD_NODE(L, ((gsp_alterTableOption*)node), referencedColumnList);
    ADD_NODE(L, ((gsp_alterTableOption*)node), datatype);
    return obj;
}
SqlNode *Node_parse_alterTableSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_alterTableSqlNode*)node), tableName);
    ADD_NODE(L, ((gsp_alterTableSqlNode*)node), tableElementList);
    ADD_NODE(L, ((gsp_alterTableSqlNode*)node), alterTableOptionList);
    return obj;
}
SqlNode *Node_parse_createSequenceSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_createSequenceSqlNode*)node), sequenceName);
    ADD_NODE(L, ((gsp_createSequenceSqlNode*)node), options);
    return obj;
}
SqlNode *Node_parse_createSynonymSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_createSynonymSqlNode*)node), synonymName);
    ADD_NODE(L, ((gsp_createSynonymSqlNode*)node), forName);
    ADD_INT(L, ((gsp_createSynonymSqlNode*)node), isPublic);
    ADD_INT(L, ((gsp_createSynonymSqlNode*)node), isReplace);
    return obj;
}
SqlNode *Node_parse_createDirectorySqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_createDirectorySqlNode*)node), directoryName);
    ADD_NODE(L, ((gsp_createDirectorySqlNode*)node), path);
    return obj;
}
SqlNode *Node_parse_dropViewSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_dropViewSqlNode*)node), viewNameList);
    return obj;
}
SqlNode *Node_parse_dropTableSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_dropTableSqlNode*)node), tableNameList);
    return obj;
}
SqlNode *Node_parse_dropIndexItem(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_dropIndexItem*)node), indexName);
    ADD_NODE(L, ((gsp_dropIndexItem*)node), tableName);
    return obj;
}
SqlNode *Node_parse_dropIndexSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_dropIndexSqlNode*)node), indexName);
    ADD_NODE(L, ((gsp_dropIndexSqlNode*)node), itemList);
    return obj;
}
SqlNode *Node_parse_truncateTableSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_truncateTableSqlNode*)node), tableName);
    return obj;
}
SqlNode *Node_parse_viewAliasItem(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_viewAliasItem*)node), alias);
    ADD_NODE(L, ((gsp_viewAliasItem*)node), columnConstraintList);
    ADD_NODE(L, ((gsp_viewAliasItem*)node), tableConstraint);
    return obj;
}
SqlNode *Node_parse_viewAliasClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_viewAliasClause*)node), viewAliasItemList);
    return obj;
}
SqlNode *Node_parse_createViewSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_createViewSqlNode*)node), viewName);
    ADD_NODE(L, ((gsp_createViewSqlNode*)node), selectSqlNode);
    ADD_NODE(L, ((gsp_createViewSqlNode*)node), viewAliasClause);
    ADD_INT(L, ((gsp_createViewSqlNode*)node), isForce);
    ADD_INT(L, ((gsp_createViewSqlNode*)node), isReplace);
    ADD_NODE(L, ((gsp_createViewSqlNode*)node), rowTypeName);
    return obj;
}
SqlNode *Node_parse_createMaterializedViewSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_createMaterializedViewSqlNode*)node), viewName);
    ADD_NODE(L, ((gsp_createMaterializedViewSqlNode*)node), selectSqlNode);
    ADD_NODE(L, ((gsp_createMaterializedViewSqlNode*)node), viewAliasClause);
    return obj;
}
SqlNode *Node_parse_createMaterializedViewLogSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_createMaterializedViewLogSqlNode*)node), mvName);
    return obj;
}
SqlNode *Node_parse_createIndexSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_createIndexSqlNode*)node), indexType);
    ADD_NODE(L, ((gsp_createIndexSqlNode*)node), indexName);
    ADD_NODE(L, ((gsp_createIndexSqlNode*)node), tableName);
    ADD_NODE(L, ((gsp_createIndexSqlNode*)node), indexItemList);
    return obj;
}
SqlNode *Node_parse_commitSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_commitSqlNode*)node), transName);
    return obj;
}
SqlNode *Node_parse_rollbackSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_rollbackSqlNode*)node), transName);
    return obj;
}
SqlNode *Node_parse_saveTransSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_saveTransSqlNode*)node), transName);
    return obj;
}
SqlNode *Node_parse_columnDefinition(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_columnDefinition*)node), columnName);
    ADD_NODE(L, ((gsp_columnDefinition*)node), datatype);
    ADD_NODE(L, ((gsp_columnDefinition*)node), constraints);
    ADD_NODE(L, ((gsp_columnDefinition*)node), defaultExpression);
    ADD_NODE(L, ((gsp_columnDefinition*)node), computedColumnExpression);
    ADD_INT(L, ((gsp_columnDefinition*)node), isNull);
    ADD_INT(L, ((gsp_columnDefinition*)node), isRowGuidCol);
    return obj;
}
SqlNode *Node_parse_tableElement(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_tableElement*)node), columnDefinition);
    ADD_NODE(L, ((gsp_tableElement*)node), constraint);
    return obj;
}
SqlNode *Node_parse_createTableSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_createTableSqlNode*)node), table);
    ADD_NODE(L, ((gsp_createTableSqlNode*)node), oftable);
    ADD_NODE(L, ((gsp_createTableSqlNode*)node), tableElementList);
    ADD_NODE(L, ((gsp_createTableSqlNode*)node), columnList);
    ADD_NODE(L, ((gsp_createTableSqlNode*)node), columnName);
    ADD_NODE(L, ((gsp_createTableSqlNode*)node), subQueryNode);
    ADD_NODE(L, ((gsp_createTableSqlNode*)node), superTableName);
    ADD_NODE(L, ((gsp_createTableSqlNode*)node), ofTypeName);
    ADD_NODE(L, ((gsp_createTableSqlNode*)node), likeTableName);
    return obj;
}
SqlNode *Node_parse_returningClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_returningClause*)node), columnValueList);
    ADD_NODE(L, ((gsp_returningClause*)node), variableList);
    return obj;
}
SqlNode *Node_parse_isolationClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_includeColumns(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_includeColumns*)node), columnList);
    return obj;
}
SqlNode *Node_parse_deleteSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_TOKEN(L, ((gsp_deleteSqlNode*)node), deleteToken);
    ADD_NODE(L, ((gsp_deleteSqlNode*)node), cteList);
    ADD_NODE(L, ((gsp_deleteSqlNode*)node), whereCondition);
    ADD_NODE(L, ((gsp_deleteSqlNode*)node), returningClause);
    ADD_NODE(L, ((gsp_deleteSqlNode*)node), isolationClause);
    ADD_NODE(L, ((gsp_deleteSqlNode*)node), includeColumns);
    ADD_NODE(L, ((gsp_deleteSqlNode*)node), targetTableNode);
    ADD_NODE(L, ((gsp_deleteSqlNode*)node), topClause);
    ADD_NODE(L, ((gsp_deleteSqlNode*)node), outputClause);
    ADD_NODE(L, ((gsp_deleteSqlNode*)node), sourceTableList);
    ADD_NODE(L, ((gsp_deleteSqlNode*)node), targetTableList);
    ADD_NODE(L, ((gsp_deleteSqlNode*)node), limitClause);
    ADD_NODE(L, ((gsp_deleteSqlNode*)node), sortClause);
    return obj;
}
SqlNode *Node_parse_updateSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_TOKEN(L, ((gsp_updateSqlNode*)node), updateToken);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), cteList);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), resultColumnList);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), whereCondition);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), returningClause);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), isolationClause);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), includeColumns);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), targetTableNode);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), topClause);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), outputClause);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), sourceTableList);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), targetTableList);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), limitClause);
    ADD_NODE(L, ((gsp_updateSqlNode*)node), sortClause);
    return obj;
}
SqlNode *Node_parse_multiTarget(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_multiTarget*)node), subQueryNode);
    ADD_NODE(L, ((gsp_multiTarget*)node), resultColumnList);
    return obj;
}
SqlNode *Node_parse_insertRest(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_insertRest*)node), valueType);
    ADD_NODE(L, ((gsp_insertRest*)node), multiTargetList);
    ADD_NODE(L, ((gsp_insertRest*)node), subQueryNode);
    ADD_NODE(L, ((gsp_insertRest*)node), functionCall);
    ADD_NODE(L, ((gsp_insertRest*)node), recordName);
    ADD_NODE(L, ((gsp_insertRest*)node), updateTargetList);
    return obj;
}
SqlNode *Node_parse_insertValuesClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_insertValuesClause*)node), multiTargetList);
    return obj;
}
SqlNode *Node_parse_insertIntoValue(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_insertIntoValue*)node), fromTable);
    ADD_NODE(L, ((gsp_insertIntoValue*)node), columnList);
    ADD_NODE(L, ((gsp_insertIntoValue*)node), valuesClause);
    ADD_NODE(L, ((gsp_insertIntoValue*)node), table);
    return obj;
}
SqlNode *Node_parse_insertCondition(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_insertCondition*)node), condition);
    ADD_NODE(L, ((gsp_insertCondition*)node), insertIntoValues);
    return obj;
}
SqlNode *Node_parse_insertSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_TOKEN(L, ((gsp_insertSqlNode*)node), insertToken);
    ADD_INT(L, ((gsp_insertSqlNode*)node), valueType);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), cteList);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), columnList);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), insertIntoValues);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), insertConditions);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), returningClause);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), isolationClause);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), includeColumns);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), targetTableNode);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), insertRest);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), subQueryNode);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), topClause);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), outputClause);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), onDuplicateUpdateList);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), multiTargetList);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), recordName);
    ADD_NODE(L, ((gsp_insertSqlNode*)node), functionCall);
    return obj;
}
SqlNode *Node_parse_whereClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_whereClause*)node), condition);
    return obj;
}
SqlNode *Node_parse_joinExpr(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_joinExpr*)node), jointype);
    ADD_INT(L, ((gsp_joinExpr*)node), original_jontype);
    ADD_NODE(L, ((gsp_joinExpr*)node), aliasClause);
    ADD_NODE(L, ((gsp_joinExpr*)node), leftOperand);
    ADD_NODE(L, ((gsp_joinExpr*)node), rightOperand);
    ADD_NODE(L, ((gsp_joinExpr*)node), onCondition);
    ADD_NODE(L, ((gsp_joinExpr*)node), usingColumns);
    return obj;
}
SqlNode *Node_parse_tableSamplePart(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_tableSample(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_pxGranule(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_flashback(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_forUpdate(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_forUpdate*)node), columnRefs);
    return obj;
}
SqlNode *Node_parse_groupingSetItem(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_groupingSetItem*)node), rollupCubeClause);
    ADD_NODE(L, ((gsp_groupingSetItem*)node), expressionItem);
    return obj;
}
SqlNode *Node_parse_groupingSet(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_groupingSet*)node), items);
    return obj;
}
SqlNode *Node_parse_rollupCube(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_rollupCube*)node), items);
    return obj;
}
SqlNode *Node_parse_gruopByItem(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_gruopByItem*)node), expr);
    ADD_NODE(L, ((gsp_gruopByItem*)node), rollupCube);
    ADD_NODE(L, ((gsp_gruopByItem*)node), groupingSet);
    ADD_NODE(L, ((gsp_gruopByItem*)node), aliasClause);
    return obj;
}
SqlNode *Node_parse_groupBy(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_TOKEN(L, ((gsp_groupBy*)node), stGroup);
    ADD_TOKEN(L, ((gsp_groupBy*)node), stBy);
    ADD_TOKEN(L, ((gsp_groupBy*)node), stHaving);
    ADD_NODE(L, ((gsp_groupBy*)node), havingClause);
    ADD_NODE(L, ((gsp_groupBy*)node), items);
    return obj;
}
SqlNode *Node_parse_selectDistinct(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_selectDistinct*)node), exprList);
    return obj;
}
SqlNode *Node_parse_topClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_topClause*)node), bPercent);
    ADD_INT(L, ((gsp_topClause*)node), bWithTies);
    ADD_NODE(L, ((gsp_topClause*)node), expr);
    return obj;
}
SqlNode *Node_parse_hierarchical(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_hierarchical*)node), connectByClause);
    ADD_NODE(L, ((gsp_hierarchical*)node), startWithClause);
    return obj;
}
SqlNode *Node_parse_intoClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_intoClause*)node), exprList);
    return obj;
}
SqlNode *Node_parse_valueClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_valueClause*)node), valueList);
    ADD_NODE(L, ((gsp_valueClause*)node), nameList);
    return obj;
}
SqlNode *Node_parse_fetchFirstClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_optimizeForClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_valueRowItem(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_valueRowItem*)node), expr);
    ADD_NODE(L, ((gsp_valueRowItem*)node), exprList);
    return obj;
}
SqlNode *Node_parse_selectSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_selectSqlNode*)node), setOperator);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), cteList);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), valueClause);
    ADD_TOKEN(L, ((gsp_selectSqlNode*)node), selectToken);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), selectDistinct);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), resultColumnList);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), intoClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), whereCondition);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), hierarchicalClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), groupByClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), orderbyClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), forupdateClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), fetchFirstClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), optimizeForClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), isolationClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), computeClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), topClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), intoTableClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), limitClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), lockingClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), windowClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), withClauses);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), qualifyClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), sampleClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), expandOnClause);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), leftNode);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), rightNode);
    ADD_NODE(L, ((gsp_selectSqlNode*)node), fromTableList);
    return obj;
}
SqlNode *Node_parse_cte(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_cte*)node), tableName);
    ADD_NODE(L, ((gsp_cte*)node), selectSqlNode);
    ADD_NODE(L, ((gsp_cte*)node), insertSqlNode);
    ADD_NODE(L, ((gsp_cte*)node), updateSqlNode);
    ADD_NODE(L, ((gsp_cte*)node), deleteSqlNode);
    ADD_NODE(L, ((gsp_cte*)node), columnList);
    return obj;
}
SqlNode *Node_parse_commentSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_commentSqlNode*)node), dbObjType);
    ADD_NODE(L, ((gsp_commentSqlNode*)node), objectName);
    ADD_NODE(L, ((gsp_commentSqlNode*)node), message);
    return obj;
}
SqlNode *Node_parse_callSpec(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_callSpec*)node), declaration);
    return obj;
}
SqlNode *Node_parse_returnSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_returnSqlNode*)node), expr);
    ADD_NODE(L, ((gsp_returnSqlNode*)node), subQueryNode);
    return obj;
}
SqlNode *Node_parse_continueSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_breakSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_grantSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_grantSqlNode*)node), nameList);
    return obj;
}
SqlNode *Node_parse_executeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_executeSqlNode*)node), moduleName);
    ADD_NODE(L, ((gsp_executeSqlNode*)node), paramList);
    return obj;
}
SqlNode *Node_parse_dropRoleSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_dropRoleSqlNode*)node), roleNameList);
    return obj;
}
SqlNode *Node_parse_dropTriggerSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_dropTriggerSqlNode*)node), triggerName);
    ADD_NODE(L, ((gsp_dropTriggerSqlNode*)node), tableName);
    return obj;
}
SqlNode *Node_parse_lockTableSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_revokeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_fetchSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_fetchSqlNode*)node), cursorName);
    ADD_NODE(L, ((gsp_fetchSqlNode*)node), variableNames);
    return obj;
}
SqlNode *Node_parse_openSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_openSqlNode*)node), cursorName);
    ADD_NODE(L, ((gsp_openSqlNode*)node), nameList);
    return obj;
}
SqlNode *Node_parse_closeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_closeSqlNode*)node), cursorName);
    return obj;
}
SqlNode *Node_parse_iterateSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_iterateSqlNode*)node), cursorName);
    return obj;
}
SqlNode *Node_parse_leaveSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_leaveSqlNode*)node), cursorName);
    return obj;
}
SqlNode *Node_parse_createFunctionSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_createFunctionSqlNode*)node), kind);
    ADD_NODE(L, ((gsp_createFunctionSqlNode*)node), functionName);
    ADD_NODE(L, ((gsp_createFunctionSqlNode*)node), parameters);
    ADD_NODE(L, ((gsp_createFunctionSqlNode*)node), returnDataType);
    ADD_NODE(L, ((gsp_createFunctionSqlNode*)node), declareStmts);
    ADD_NODE(L, ((gsp_createFunctionSqlNode*)node), stmts);
    ADD_NODE(L, ((gsp_createFunctionSqlNode*)node), exceptionClause);
    ADD_NODE(L, ((gsp_createFunctionSqlNode*)node), returnSqlNode);
    ADD_NODE(L, ((gsp_createFunctionSqlNode*)node), compoundSqlNode);
    ADD_NODE(L, ((gsp_createFunctionSqlNode*)node), callSpec);
    ADD_NODE(L, ((gsp_createFunctionSqlNode*)node), blockSqlNode);
    ADD_NODE(L, ((gsp_createFunctionSqlNode*)node), stmtSqlNode);
    return obj;
}
SqlNode *Node_parse_parameterDeclaration(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_parameterDeclaration*)node), parameterName);
    ADD_NODE(L, ((gsp_parameterDeclaration*)node), dataType);
    ADD_NODE(L, ((gsp_parameterDeclaration*)node), defaultValue);
    ADD_INT(L, ((gsp_parameterDeclaration*)node), isVarying);
    ADD_NODE(L, ((gsp_parameterDeclaration*)node), varyPrecision);
    ADD_INT(L, ((gsp_parameterDeclaration*)node), isNotNull);
    ADD_INT(L, ((gsp_parameterDeclaration*)node), mode);
    ADD_INT(L, ((gsp_parameterDeclaration*)node), howtoSetValue);
    return obj;
}
SqlNode *Node_parse_createProcedureSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_createProcedureSqlNode*)node), kind);
    ADD_NODE(L, ((gsp_createProcedureSqlNode*)node), procedureName);
    ADD_NODE(L, ((gsp_createProcedureSqlNode*)node), parameters);
    ADD_NODE(L, ((gsp_createProcedureSqlNode*)node), declareStmts);
    ADD_NODE(L, ((gsp_createProcedureSqlNode*)node), innerStmts);
    ADD_NODE(L, ((gsp_createProcedureSqlNode*)node), stmts);
    ADD_NODE(L, ((gsp_createProcedureSqlNode*)node), exceptionClause);
    ADD_NODE(L, ((gsp_createProcedureSqlNode*)node), callSpec);
    ADD_NODE(L, ((gsp_createProcedureSqlNode*)node), blockSqlNode);
    ADD_NODE(L, ((gsp_createProcedureSqlNode*)node), stmtSqlNode);
    return obj;
}
SqlNode *Node_parse_exceptionClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_exceptionClause*)node), Handlers);
    return obj;
}
SqlNode *Node_parse_blockSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_blockSqlNode*)node), stmts);
    ADD_NODE(L, ((gsp_blockSqlNode*)node), exceptionClause);
    ADD_NODE(L, ((gsp_blockSqlNode*)node), declareStmts);
    return obj;
}
SqlNode *Node_parse_arrayAccess(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_arrayAccess*)node), arrayName);
    ADD_NODE(L, ((gsp_arrayAccess*)node), index1);
    ADD_NODE(L, ((gsp_arrayAccess*)node), index2);
    ADD_NODE(L, ((gsp_arrayAccess*)node), index3);
    ADD_NODE(L, ((gsp_arrayAccess*)node), propertyName);
    return obj;
}
SqlNode *Node_parse_declareVariable(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_declareVariable*)node), varName);
    ADD_NODE(L, ((gsp_declareVariable*)node), varDatatype);
    ADD_NODE(L, ((gsp_declareVariable*)node), defaultValue);
    ADD_NODE(L, ((gsp_declareVariable*)node), tableTypeDefinitions);
    ADD_INT(L, ((gsp_declareVariable*)node), variableType);
    return obj;
}
SqlNode *Node_parse_declareSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_declareSqlNode*)node), declareType);
    ADD_NODE(L, ((gsp_declareSqlNode*)node), vars);
    ADD_NODE(L, ((gsp_declareSqlNode*)node), varType);
    ADD_INT(L, ((gsp_declareSqlNode*)node), howtoSetValue);
    ADD_NODE(L, ((gsp_declareSqlNode*)node), defaultValue);
    ADD_NODE(L, ((gsp_declareSqlNode*)node), conditionName);
    ADD_NODE(L, ((gsp_declareSqlNode*)node), cursorName);
    ADD_NODE(L, ((gsp_declareSqlNode*)node), subQueryNode);
    ADD_NODE(L, ((gsp_declareSqlNode*)node), stmt);
    return obj;
}
SqlNode *Node_parse_createTriggerUpdateColumn(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_createTriggerUpdateColumn*)node), columnName);
    return obj;
}
SqlNode *Node_parse_ifSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_ifSqlNode*)node), condition);
    ADD_NODE(L, ((gsp_ifSqlNode*)node), stmts);
    ADD_NODE(L, ((gsp_ifSqlNode*)node), elseStmts);
    ADD_NODE(L, ((gsp_ifSqlNode*)node), elseifList);
    ADD_NODE(L, ((gsp_ifSqlNode*)node), stmt);
    ADD_NODE(L, ((gsp_ifSqlNode*)node), elseStmt);
    ADD_NODE(L, ((gsp_ifSqlNode*)node), updateColumnList);
    return obj;
}
SqlNode *Node_parse_elseIfSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_elseIfSqlNode*)node), condition);
    ADD_NODE(L, ((gsp_elseIfSqlNode*)node), stmts);
    return obj;
}
SqlNode *Node_parse_whileSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_whileSqlNode*)node), condition);
    ADD_NODE(L, ((gsp_whileSqlNode*)node), stmt);
    ADD_NODE(L, ((gsp_whileSqlNode*)node), stmts);
    return obj;
}
SqlNode *Node_parse_repeatSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_repeatSqlNode*)node), condition);
    ADD_NODE(L, ((gsp_repeatSqlNode*)node), stmts);
    return obj;
}
SqlNode *Node_parse_loopSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_loopSqlNode*)node), stmts);
    return obj;
}
SqlNode *Node_parse_unknownStatement(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_createTriggerSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_createTriggerSqlNode*)node), fireMode);
    ADD_NODE(L, ((gsp_createTriggerSqlNode*)node), triggerName);
    ADD_NODE(L, ((gsp_createTriggerSqlNode*)node), tableName);
    ADD_NODE(L, ((gsp_createTriggerSqlNode*)node), whenCondition);
    ADD_NODE(L, ((gsp_createTriggerSqlNode*)node), triggerEvent);
    ADD_NODE(L, ((gsp_createTriggerSqlNode*)node), triggerAction);
    ADD_NODE(L, ((gsp_createTriggerSqlNode*)node), stmt);
    ADD_NODE(L, ((gsp_createTriggerSqlNode*)node), stmts);
    ADD_NODE(L, ((gsp_createTriggerSqlNode*)node), blockSqlNode);
    ADD_NODE(L, ((gsp_createTriggerSqlNode*)node), stmtSqlNode);
    return obj;
}
SqlNode *Node_parse_exceptionHandler(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_exceptionHandler*)node), exceptionNames);
    ADD_NODE(L, ((gsp_exceptionHandler*)node), stmts);
    return obj;
}
SqlNode *Node_parse_pivotClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_pivotClause*)node), aggregationFunction);
    ADD_NODE(L, ((gsp_pivotClause*)node), privotColumn);
    ADD_NODE(L, ((gsp_pivotClause*)node), privotColumnList);
    ADD_NODE(L, ((gsp_pivotClause*)node), inResultList);
    ADD_NODE(L, ((gsp_pivotClause*)node), aliasClause);
    return obj;
}
SqlNode *Node_parse_unPivotClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_unPivotClause*)node), aggregationFunction);
    ADD_NODE(L, ((gsp_unPivotClause*)node), valueColumn);
    ADD_NODE(L, ((gsp_unPivotClause*)node), privotColumn);
    ADD_NODE(L, ((gsp_unPivotClause*)node), privotColumnList);
    ADD_NODE(L, ((gsp_unPivotClause*)node), inResultList);
    ADD_NODE(L, ((gsp_unPivotClause*)node), aliasClause);
    return obj;
}
SqlNode *Node_parse_renameColumnSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_renameColumnSqlNode*)node), columnName);
    ADD_NODE(L, ((gsp_renameColumnSqlNode*)node), newColumnName);
    return obj;
}
SqlNode *Node_parse_renameSequenceSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_renameSequenceSqlNode*)node), sequenceName);
    ADD_NODE(L, ((gsp_renameSequenceSqlNode*)node), newSequenceName);
    return obj;
}
SqlNode *Node_parse_renameTableSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_renameTableSqlNode*)node), tableName);
    ADD_NODE(L, ((gsp_renameTableSqlNode*)node), newTableName);
    return obj;
}
SqlNode *Node_parse_renameIndexSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_renameIndexSqlNode*)node), indexName);
    ADD_NODE(L, ((gsp_renameIndexSqlNode*)node), newIndexName);
    return obj;
}
SqlNode *Node_parse_dropSequenceSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_dropSequenceSqlNode*)node), sequenceName);
    return obj;
}
SqlNode *Node_parse_dropSynonymSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_dropSynonymSqlNode*)node), synonymName);
    return obj;
}
SqlNode *Node_parse_dropRowTypeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_dropRowTypeSqlNode*)node), rowTypeName);
    return obj;
}
SqlNode *Node_parse_alterIndexSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_alterIndexSqlNode*)node), indexName);
    return obj;
}
SqlNode *Node_parse_alterSequenceSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_alterSequenceSqlNode*)node), sequenceName);
    return obj;
}
SqlNode *Node_parse_alterViewSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_alterViewSqlNode*)node), viewName);
    return obj;
}
SqlNode *Node_parse_intoTableClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_intoTableClause*)node), tableName);
    return obj;
}
SqlNode *Node_parse_informixOuterClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_informixOuterClause*)node), fromTable);
    ADD_NODE(L, ((gsp_informixOuterClause*)node), fromTableList);
    return obj;
}
SqlNode *Node_parse_createRowTypeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_createRowTypeSqlNode*)node), rowTypeName);
    ADD_NODE(L, ((gsp_createRowTypeSqlNode*)node), superTableName);
    ADD_NODE(L, ((gsp_createRowTypeSqlNode*)node), columnDefList);
    return obj;
}
SqlNode *Node_parse_subscripts(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_subscripts*)node), first);
    ADD_NODE(L, ((gsp_subscripts*)node), last);
    return obj;
}
SqlNode *Node_parse_limitClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_limitClause*)node), offset);
    ADD_NODE(L, ((gsp_limitClause*)node), rowCount);
    ADD_NODE(L, ((gsp_limitClause*)node), limitValue);
    return obj;
}
SqlNode *Node_parse_callSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_callSqlNode*)node), functionName);
    ADD_NODE(L, ((gsp_callSqlNode*)node), args);
    return obj;
}
SqlNode *Node_parse_createDatabaseSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_createDatabaseSqlNode*)node), databaseName);
    return obj;
}
SqlNode *Node_parse_lockingClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_lockingClause*)node), lockedObjects);
    return obj;
}
SqlNode *Node_parse_windowClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_windowClause*)node), windownsDefs);
    return obj;
}
SqlNode *Node_parse_partitionClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_partitionClause*)node), exprList);
    return obj;
}
SqlNode *Node_parse_windowDef(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_windowDef*)node), referenceName);
    ADD_NODE(L, ((gsp_windowDef*)node), partitionClause);
    ADD_NODE(L, ((gsp_windowDef*)node), sortClause);
    ADD_NODE(L, ((gsp_windowDef*)node), frameClause);
    ADD_NODE(L, ((gsp_windowDef*)node), startOffset);
    ADD_NODE(L, ((gsp_windowDef*)node), endOffset);
    ADD_NODE(L, ((gsp_windowDef*)node), windowName);
    return obj;
}
SqlNode *Node_parse_indices(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_indices*)node), attributeName);
    ADD_NODE(L, ((gsp_indices*)node), lowerSubscript);
    ADD_NODE(L, ((gsp_indices*)node), upperSubscript);
    return obj;
}
SqlNode *Node_parse_collectStatisticsSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_collectStatisticsSqlNode*)node), tableName);
    ADD_NODE(L, ((gsp_collectStatisticsSqlNode*)node), indexName);
    ADD_NODE(L, ((gsp_collectStatisticsSqlNode*)node), columnName);
    ADD_NODE(L, ((gsp_collectStatisticsSqlNode*)node), columnList);
    return obj;
}
SqlNode *Node_parse_teradataWithClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_teradataWithClause*)node), exprList);
    ADD_NODE(L, ((gsp_teradataWithClause*)node), byList);
    return obj;
}
SqlNode *Node_parse_qualifyClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_qualifyClause*)node), expr);
    return obj;
}
SqlNode *Node_parse_sampleClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_sampleClause*)node), countFractionDescriptionList);
    ADD_NODE(L, ((gsp_sampleClause*)node), whenThenList);
    return obj;
}
SqlNode *Node_parse_expandOnClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_expandOnClause*)node), expr);
    ADD_NODE(L, ((gsp_expandOnClause*)node), columnAlias);
    ADD_NODE(L, ((gsp_expandOnClause*)node), periodExpr);
    return obj;
}
SqlNode *Node_parse_datatypeAttribute(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_newVariantTypeArgument(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_newVariantTypeArgument*)node), expr);
    ADD_NODE(L, ((gsp_newVariantTypeArgument*)node), aliasName);
    return obj;
}
SqlNode *Node_parse_outputFormatPhrase(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_outputFormatPhrase*)node), dataTypeName);
    ADD_NODE(L, ((gsp_outputFormatPhrase*)node), datatypeAttribute);
    return obj;
}
SqlNode *Node_parse_plsqlCreateTypeBody(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlCreateTypeBody*)node), typeName);
    ADD_NODE(L, ((gsp_plsqlCreateTypeBody*)node), stmts);
    return obj;
}
SqlNode *Node_parse_typeAttribute(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_typeAttribute*)node), attributeName);
    ADD_NODE(L, ((gsp_typeAttribute*)node), datatype);
    return obj;
}
SqlNode *Node_parse_plsqlCreateType(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlCreateType*)node), typeName);
    ADD_NODE(L, ((gsp_plsqlCreateType*)node), attributes);
    return obj;
}
SqlNode *Node_parse_dmlEventClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_dmlEventClause*)node), tableName);
    ADD_NODE(L, ((gsp_dmlEventClause*)node), dml_event_items);
    return obj;
}
SqlNode *Node_parse_nonDmlTriggerClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_nonDmlTriggerClause*)node), schemaName);
    ADD_INT(L, ((gsp_nonDmlTriggerClause*)node), fireMode);
    ADD_INT(L, ((gsp_nonDmlTriggerClause*)node), isSchema);
    ADD_INT(L, ((gsp_nonDmlTriggerClause*)node), isDatabase);
    ADD_NODE(L, ((gsp_nonDmlTriggerClause*)node), ddl_event_list);
    ADD_NODE(L, ((gsp_nonDmlTriggerClause*)node), database_event_list);
    return obj;
}
SqlNode *Node_parse_compoundDmlTriggerClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_compoundDmlTriggerClause*)node), dmlEventClause);
    return obj;
}
SqlNode *Node_parse_simpleDmlTriggerClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_simpleDmlTriggerClause*)node), fireMode);
    ADD_NODE(L, ((gsp_simpleDmlTriggerClause*)node), dmlEventClause);
    return obj;
}
SqlNode *Node_parse_createPackageSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_createPackageSqlNode*)node), packageName);
    ADD_INT(L, ((gsp_createPackageSqlNode*)node), kind);
    ADD_NODE(L, ((gsp_createPackageSqlNode*)node), definitions_or_declarations);
    ADD_NODE(L, ((gsp_createPackageSqlNode*)node), stmts);
    ADD_NODE(L, ((gsp_createPackageSqlNode*)node), exceptionClause);
    ADD_NODE(L, ((gsp_createPackageSqlNode*)node), declareStatements);
    ADD_NODE(L, ((gsp_createPackageSqlNode*)node), bodyStatements);
    return obj;
}
SqlNode *Node_parse_plsqlVarDeclStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_plsqlVarDeclStmt*)node), whatDeclared);
    ADD_NODE(L, ((gsp_plsqlVarDeclStmt*)node), exception_name);
    ADD_NODE(L, ((gsp_plsqlVarDeclStmt*)node), error_number);
    ADD_NODE(L, ((gsp_plsqlVarDeclStmt*)node), elementName);
    ADD_NODE(L, ((gsp_plsqlVarDeclStmt*)node), dataType);
    ADD_NODE(L, ((gsp_plsqlVarDeclStmt*)node), value);
    ADD_INT(L, ((gsp_plsqlVarDeclStmt*)node), isNotNull);
    ADD_INT(L, ((gsp_plsqlVarDeclStmt*)node), howtoSetValue);
    return obj;
}
SqlNode *Node_parse_plsqlRaiseStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlRaiseStmt*)node), exceptionName);
    return obj;
}
SqlNode *Node_parse_plsqlLoopStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlLoopStmt*)node), stmts);
    ADD_NODE(L, ((gsp_plsqlLoopStmt*)node), condition);
    ADD_NODE(L, ((gsp_plsqlLoopStmt*)node), indexName);
    ADD_NODE(L, ((gsp_plsqlLoopStmt*)node), cursorName);
    ADD_NODE(L, ((gsp_plsqlLoopStmt*)node), lower_bound);
    ADD_NODE(L, ((gsp_plsqlLoopStmt*)node), upper_bound);
    ADD_NODE(L, ((gsp_plsqlLoopStmt*)node), cursorParameterNames);
    return obj;
}
SqlNode *Node_parse_plsqlCaseStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlCaseStmt*)node), caseExpr);
    return obj;
}
SqlNode *Node_parse_plsqlForallStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlForallStmt*)node), indexName);
    ADD_NODE(L, ((gsp_plsqlForallStmt*)node), stmt);
    return obj;
}
SqlNode *Node_parse_plsqlElsifStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlElsifStmt*)node), condition);
    ADD_NODE(L, ((gsp_plsqlElsifStmt*)node), thenStmts);
    return obj;
}
SqlNode *Node_parse_plsqlIfStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlIfStmt*)node), condition);
    ADD_NODE(L, ((gsp_plsqlIfStmt*)node), thenStmts);
    ADD_NODE(L, ((gsp_plsqlIfStmt*)node), elseStmts);
    ADD_NODE(L, ((gsp_plsqlIfStmt*)node), elsifStmts);
    return obj;
}
SqlNode *Node_parse_plsqlGotoStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlGotoStmt*)node), gotolabelName);
    return obj;
}
SqlNode *Node_parse_plsqlExitStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlExitStmt*)node), whenCondition);
    ADD_NODE(L, ((gsp_plsqlExitStmt*)node), exitlabelName);
    return obj;
}
SqlNode *Node_parse_plsqlAssignStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlAssignStmt*)node), left);
    ADD_NODE(L, ((gsp_plsqlAssignStmt*)node), right);
    return obj;
}
SqlNode *Node_parse_plsqlCursorDeclStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlCursorDeclStmt*)node), subqueryNode);
    ADD_NODE(L, ((gsp_plsqlCursorDeclStmt*)node), cursorName);
    ADD_NODE(L, ((gsp_plsqlCursorDeclStmt*)node), cursorParameterDeclarations);
    ADD_NODE(L, ((gsp_plsqlCursorDeclStmt*)node), rowtype);
    ADD_NODE(L, ((gsp_plsqlCursorDeclStmt*)node), cursorTypeName);
    return obj;
}
SqlNode *Node_parse_plsqlRecordTypeDefStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlRecordTypeDefStmt*)node), typeName);
    ADD_NODE(L, ((gsp_plsqlRecordTypeDefStmt*)node), fieldDeclarations);
    return obj;
}
SqlNode *Node_parse_plsqlVarrayTypeDefStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlVarrayTypeDefStmt*)node), typeName);
    ADD_NODE(L, ((gsp_plsqlVarrayTypeDefStmt*)node), elementDataType);
    ADD_NODE(L, ((gsp_plsqlVarrayTypeDefStmt*)node), sizeLimit);
    ADD_INT(L, ((gsp_plsqlVarrayTypeDefStmt*)node), isNotNull);
    return obj;
}
SqlNode *Node_parse_plsqlTableTypeDefStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlTableTypeDefStmt*)node), typeName);
    ADD_NODE(L, ((gsp_plsqlTableTypeDefStmt*)node), elementDataType);
    ADD_INT(L, ((gsp_plsqlTableTypeDefStmt*)node), isNotNull);
    ADD_NODE(L, ((gsp_plsqlTableTypeDefStmt*)node), indexByDataType);
    return obj;
}
SqlNode *Node_parse_plsqlNullStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_plsqlPipeRowStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_bindArgument(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_bindArgument*)node), mode);
    ADD_NODE(L, ((gsp_bindArgument*)node), bindArgumentExpr);
    return obj;
}
SqlNode *Node_parse_execImmeNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_execImmeNode*)node), dynamicStringExpr);
    ADD_NODE(L, ((gsp_execImmeNode*)node), intoVariables);
    ADD_NODE(L, ((gsp_execImmeNode*)node), bindArguments);
    ADD_NODE(L, ((gsp_execImmeNode*)node), returnNames);
    return obj;
}
SqlNode *Node_parse_plsqlOpenforStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlOpenforStmt*)node), cursorVariableName);
    ADD_NODE(L, ((gsp_plsqlOpenforStmt*)node), subqueryNode);
    ADD_NODE(L, ((gsp_plsqlOpenforStmt*)node), dynamic_string);
    return obj;
}
SqlNode *Node_parse_plsqlOpenStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlOpenStmt*)node), cursorName);
    ADD_NODE(L, ((gsp_plsqlOpenStmt*)node), cursorParameterNames);
    return obj;
}
SqlNode *Node_parse_plsqlBasicStmt(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_plsqlBasicStmt*)node), expr);
    return obj;
}
SqlNode *Node_parse_trigger_event(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_mssql_executeAsSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_mssql_executeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_mssql_executeSqlNode*)node), execType);
    ADD_NODE(L, ((gsp_mssql_executeSqlNode*)node), moduleName);
    ADD_NODE(L, ((gsp_mssql_executeSqlNode*)node), returnStatus);
    ADD_NODE(L, ((gsp_mssql_executeSqlNode*)node), parameterList);
    ADD_NODE(L, ((gsp_mssql_executeSqlNode*)node), stringValues);
    return obj;
}
SqlNode *Node_parse_mssql_execParameter(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_mssql_execParameter*)node), parameterMode);
    ADD_NODE(L, ((gsp_mssql_execParameter*)node), parameterName);
    ADD_NODE(L, ((gsp_mssql_execParameter*)node), parameterValue);
    return obj;
}
SqlNode *Node_parse_mssql_dropDbObjectSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_mssql_dropDbObjectSqlNode*)node), dbObjectType);
    ADD_NODE(L, ((gsp_mssql_dropDbObjectSqlNode*)node), objectNames);
    return obj;
}
SqlNode *Node_parse_mssql_setSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_INT(L, ((gsp_mssql_setSqlNode*)node), setType);
    ADD_NODE(L, ((gsp_mssql_setSqlNode*)node), varName);
    ADD_NODE(L, ((gsp_mssql_setSqlNode*)node), varExpr);
    ADD_NODE(L, ((gsp_mssql_setSqlNode*)node), selectSqlNode);
    ADD_NODE(L, ((gsp_mssql_setSqlNode*)node), exprList);
    return obj;
}
SqlNode *Node_parse_mssql_beginTranSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_beginTranSqlNode*)node), transactionName);
    ADD_INT(L, ((gsp_mssql_beginTranSqlNode*)node), distributed);
    ADD_INT(L, ((gsp_mssql_beginTranSqlNode*)node), withMark);
    ADD_NODE(L, ((gsp_mssql_beginTranSqlNode*)node), withMarkDescription);
    return obj;
}
SqlNode *Node_parse_mssql_raiserrorSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_raiserrorSqlNode*)node), msgs);
    return obj;
}
SqlNode *Node_parse_mssql_gotoSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_gotoSqlNode*)node), labelName);
    return obj;
}
SqlNode *Node_parse_mssql_labelSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_mssql_deallocateSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_deallocateSqlNode*)node), cursorName);
    ADD_INT(L, ((gsp_mssql_deallocateSqlNode*)node), bGlobal);
    return obj;
}
SqlNode *Node_parse_mssql_beginDialogSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_beginDialogSqlNode*)node), dialogHandle);
    ADD_NODE(L, ((gsp_mssql_beginDialogSqlNode*)node), initiatorServiceName);
    ADD_NODE(L, ((gsp_mssql_beginDialogSqlNode*)node), targetServiceName);
    return obj;
}
SqlNode *Node_parse_mssql_sendOnConversationSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_sendOnConversationSqlNode*)node), conversationHandle);
    return obj;
}
SqlNode *Node_parse_mssql_endConversationSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_endConversationSqlNode*)node), conversationHandle);
    return obj;
}
SqlNode *Node_parse_mssql_revertSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_mssql_goSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_mssql_useSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_useSqlNode*)node), databaseName);
    return obj;
}
SqlNode *Node_parse_mssql_printSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_printSqlNode*)node), vars);
    return obj;
}
SqlNode *Node_parse_mssql_computeClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_computeClause*)node), items);
    return obj;
}
SqlNode *Node_parse_mssql_computeClauseItem(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_computeClauseItem*)node), computeExprList);
    ADD_NODE(L, ((gsp_mssql_computeClauseItem*)node), exprList);
    return obj;
}
SqlNode *Node_parse_mssql_computeExpr(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_computeExpr*)node), expr);
    return obj;
}
SqlNode *Node_parse_mssql_containsTable(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_containsTable*)node), tableName);
    ADD_NODE(L, ((gsp_mssql_containsTable*)node), containExpr);
    ADD_NODE(L, ((gsp_mssql_containsTable*)node), searchCondition);
    return obj;
}
SqlNode *Node_parse_mssql_freeTable(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_freeTable*)node), tableName);
    ADD_NODE(L, ((gsp_mssql_freeTable*)node), containExpr);
    ADD_NODE(L, ((gsp_mssql_freeTable*)node), searchCondition);
    return obj;
}
SqlNode *Node_parse_mssql_openXML(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_mssql_openRowSet(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_mssql_openQuery(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_openQuery*)node), exprList);
    return obj;
}
SqlNode *Node_parse_mssql_openDatasource(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_openDatasource*)node), providerName);
    ADD_NODE(L, ((gsp_mssql_openDatasource*)node), initString);
    ADD_NODE(L, ((gsp_mssql_openDatasource*)node), tableName);
    ADD_NODE(L, ((gsp_mssql_openDatasource*)node), exprList);
    return obj;
}
SqlNode *Node_parse_mssql_tableHint(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_tableHint*)node), hintName);
    ADD_NODE(L, ((gsp_mssql_tableHint*)node), hintNameList);
    return obj;
}
SqlNode *Node_parse_mssql_bulkInsertSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_bulkInsertSqlNode*)node), tableName);
    ADD_NODE(L, ((gsp_mssql_bulkInsertSqlNode*)node), datafile);
    return obj;
}
SqlNode *Node_parse_mssql_outputClause(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_outputClause*)node), selectItemList);
    ADD_NODE(L, ((gsp_mssql_outputClause*)node), selectItemList2);
    ADD_NODE(L, ((gsp_mssql_outputClause*)node), intoColumnList);
    ADD_NODE(L, ((gsp_mssql_outputClause*)node), tableName);
    return obj;
}
SqlNode *Node_parse_mssql_updateTextSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_mssql_updateTextSqlNode*)node), destColumnName);
    ADD_NODE(L, ((gsp_mssql_updateTextSqlNode*)node), destTextPtr);
    return obj;
}
SqlNode *Node_parse_db2_signal(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_db2_compoundSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_db2_compoundSqlNode*)node), declareStmts);
    ADD_NODE(L, ((gsp_db2_compoundSqlNode*)node), bodyStmts);
    return obj;
}
SqlNode *Node_parse_db2_triggerAction(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_db2_triggerAction*)node), whenCondition);
    ADD_NODE(L, ((gsp_db2_triggerAction*)node), compoundSqlNode);
    ADD_NODE(L, ((gsp_db2_triggerAction*)node), stmt);
    return obj;
}
SqlNode *Node_parse_db2_callStmtSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_db2_callStmtSqlNode*)node), procedureName);
    ADD_NODE(L, ((gsp_db2_callStmtSqlNode*)node), args);
    return obj;
}
SqlNode *Node_parse_db2_forSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_db2_forSqlNode*)node), loopName);
    ADD_NODE(L, ((gsp_db2_forSqlNode*)node), cursorName);
    ADD_NODE(L, ((gsp_db2_forSqlNode*)node), subQueryNode);
    ADD_NODE(L, ((gsp_db2_forSqlNode*)node), stmts);
    return obj;
}
SqlNode *Node_parse_db2_iterateStmtSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_db2_iterateStmtSqlNode*)node), labelName);
    return obj;
}
SqlNode *Node_parse_db2_leaveStmtSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_db2_leaveStmtSqlNode*)node), labelName);
    return obj;
}
SqlNode *Node_parse_db2_setSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  return obj;
}
SqlNode *Node_parse_db2_whileSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_db2_whileSqlNode*)node), condition);
    ADD_NODE(L, ((gsp_db2_whileSqlNode*)node), stmts);
    return obj;
}
SqlNode *Node_parse_db2_repeatSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_db2_repeatSqlNode*)node), condition);
    ADD_NODE(L, ((gsp_db2_repeatSqlNode*)node), stmts);
    return obj;
}
SqlNode *Node_parse_db2_gotoSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_db2_gotoSqlNode*)node), labelName);
    return obj;
}
SqlNode *Node_parse_db2_loopSqlNode(lua_State *L, gsp_node *node, Statement *stmt) {
    SqlNode *obj = Node_new(L); obj->_node = node;  ADD_NODE(L, ((gsp_db2_loopSqlNode*)node), stmts);
    return obj;
}
///

void Node_init() {
    int i;
    for (i=0; i<MAX_NODE_PARSE_FUNCS;i++)
        Node_parse_functions[i] = NULL;
    printf("node init \n");    
    Node_parse_functions[t_gsp_list] = Node_parse_list;
    Node_parse_functions[t_gsp_createTableStatement] = Node_parse_createTableSqlNode;
    Node_parse_functions[t_gsp_insertStatement] = Node_parse_insertSqlNode;
    Node_parse_functions[t_gsp_updateStatement] = Node_parse_updateSqlNode;
    Node_parse_functions[t_gsp_deleteStatement] = Node_parse_deleteSqlNode;
    
    Node_parse_functions[t_gsp_listcell] = Node_parse_listcell;
    Node_parse_functions[t_gsp_sql_statement] = Node_parse_sql_statement;
    Node_parse_functions[t_gsp_dummy] = Node_parse_dummy;
    Node_parse_functions[t_gsp_constant] = Node_parse_constant;
    Node_parse_functions[t_gsp_objectname] = Node_parse_objectname;
    Node_parse_functions[t_gsp_expr] = Node_parse_expr;
    Node_parse_functions[t_gsp_objectAccess] = Node_parse_objectAccess;
    Node_parse_functions[t_gsp_aliasClause] = Node_parse_aliasClause;
    Node_parse_functions[t_gsp_resultColumn] = Node_parse_resultColumn;
    Node_parse_functions[t_gsp_trimArgument] = Node_parse_trimArgument;
    Node_parse_functions[t_gsp_orderByItem] = Node_parse_orderByItem;
    Node_parse_functions[t_gsp_orderBy] = Node_parse_orderBy;
    Node_parse_functions[t_gsp_keepDenseRankClause] = Node_parse_keepDenseRankClause;
    Node_parse_functions[t_gsp_analyticFunction] = Node_parse_analyticFunction;
    Node_parse_functions[t_gsp_functionCall] = Node_parse_functionCall;
    Node_parse_functions[t_gsp_whenClauseItem] = Node_parse_whenClauseItem;
    Node_parse_functions[t_gsp_caseExpression] = Node_parse_caseExpression;
    Node_parse_functions[t_gsp_intervalExpression] = Node_parse_intervalExpression;
    Node_parse_functions[t_gsp_precisionScale] = Node_parse_precisionScale;
    Node_parse_functions[t_gsp_typename] = Node_parse_typename;
    Node_parse_functions[t_gsp_keyReference] = Node_parse_keyReference;
    Node_parse_functions[t_gsp_keyAction] = Node_parse_keyAction;
    Node_parse_functions[t_gsp_constraint] = Node_parse_constraint;
    Node_parse_functions[t_gsp_mergeInsertClause] = Node_parse_mergeInsertClause;
    Node_parse_functions[t_gsp_mergeUpdateClause] = Node_parse_mergeUpdateClause;
    Node_parse_functions[t_gsp_mergeDeleteClause] = Node_parse_mergeDeleteClause;
    Node_parse_functions[t_gsp_mergeWhenClause] = Node_parse_mergeWhenClause;
    Node_parse_functions[t_gsp_dataChangeTable] = Node_parse_dataChangeTable;
    Node_parse_functions[t_gsp_fromTable] = Node_parse_fromTable;
    Node_parse_functions[t_gsp_table] = Node_parse_table;
    Node_parse_functions[t_gsp_mergeSqlNode] = Node_parse_mergeSqlNode;
    Node_parse_functions[t_gsp_alterTableOption] = Node_parse_alterTableOption;
    Node_parse_functions[t_gsp_alterTableSqlNode] = Node_parse_alterTableSqlNode;
    Node_parse_functions[t_gsp_createSequenceSqlNode] = Node_parse_createSequenceSqlNode;
    Node_parse_functions[t_gsp_createSynonymSqlNode] = Node_parse_createSynonymSqlNode;
    Node_parse_functions[t_gsp_createDirectorySqlNode] = Node_parse_createDirectorySqlNode;
    Node_parse_functions[t_gsp_dropViewSqlNode] = Node_parse_dropViewSqlNode;
    Node_parse_functions[t_gsp_dropTableSqlNode] = Node_parse_dropTableSqlNode;
    Node_parse_functions[t_gsp_dropIndexItem] = Node_parse_dropIndexItem;
    Node_parse_functions[t_gsp_dropIndexSqlNode] = Node_parse_dropIndexSqlNode;
    Node_parse_functions[t_gsp_truncateTableSqlNode] = Node_parse_truncateTableSqlNode;
    Node_parse_functions[t_gsp_viewAliasItem] = Node_parse_viewAliasItem;
    Node_parse_functions[t_gsp_viewAliasClause] = Node_parse_viewAliasClause;
    Node_parse_functions[t_gsp_createViewSqlNode] = Node_parse_createViewSqlNode;
    Node_parse_functions[t_gsp_createMaterializedViewSqlNode] = Node_parse_createMaterializedViewSqlNode;
    Node_parse_functions[t_gsp_createMaterializedViewLogSqlNode] = Node_parse_createMaterializedViewLogSqlNode;
    Node_parse_functions[t_gsp_createIndexSqlNode] = Node_parse_createIndexSqlNode;
    Node_parse_functions[t_gsp_commitSqlNode] = Node_parse_commitSqlNode;
    Node_parse_functions[t_gsp_rollbackSqlNode] = Node_parse_rollbackSqlNode;
    Node_parse_functions[t_gsp_saveTransSqlNode] = Node_parse_saveTransSqlNode;
    Node_parse_functions[t_gsp_columnDefinition] = Node_parse_columnDefinition;
    Node_parse_functions[t_gsp_tableElement] = Node_parse_tableElement;
    Node_parse_functions[t_gsp_createTableSqlNode] = Node_parse_createTableSqlNode;
    Node_parse_functions[t_gsp_returningClause] = Node_parse_returningClause;
    Node_parse_functions[t_gsp_isolationClause] = Node_parse_isolationClause;
    Node_parse_functions[t_gsp_includeColumns] = Node_parse_includeColumns;
    Node_parse_functions[t_gsp_deleteSqlNode] = Node_parse_deleteSqlNode;
    Node_parse_functions[t_gsp_updateSqlNode] = Node_parse_updateSqlNode;
    Node_parse_functions[t_gsp_multiTarget] = Node_parse_multiTarget;
    Node_parse_functions[t_gsp_insertRest] = Node_parse_insertRest;
    Node_parse_functions[t_gsp_insertValuesClause] = Node_parse_insertValuesClause;
    Node_parse_functions[t_gsp_insertIntoValue] = Node_parse_insertIntoValue;
    Node_parse_functions[t_gsp_insertCondition] = Node_parse_insertCondition;
    Node_parse_functions[t_gsp_insertSqlNode] = Node_parse_insertSqlNode;
    Node_parse_functions[t_gsp_whereClause] = Node_parse_whereClause;
    Node_parse_functions[t_gsp_joinExpr] = Node_parse_joinExpr;
    Node_parse_functions[t_gsp_tableSamplePart] = Node_parse_tableSamplePart;
    Node_parse_functions[t_gsp_tableSample] = Node_parse_tableSample;
    Node_parse_functions[t_gsp_pxGranule] = Node_parse_pxGranule;
    Node_parse_functions[t_gsp_flashback] = Node_parse_flashback;
    Node_parse_functions[t_gsp_forUpdate] = Node_parse_forUpdate;
    Node_parse_functions[t_gsp_groupingSetItem] = Node_parse_groupingSetItem;
    Node_parse_functions[t_gsp_groupingSet] = Node_parse_groupingSet;
    Node_parse_functions[t_gsp_rollupCube] = Node_parse_rollupCube;
    Node_parse_functions[t_gsp_gruopByItem] = Node_parse_gruopByItem;
    Node_parse_functions[t_gsp_groupBy] = Node_parse_groupBy;
    Node_parse_functions[t_gsp_selectDistinct] = Node_parse_selectDistinct;
    Node_parse_functions[t_gsp_topClause] = Node_parse_topClause;
    Node_parse_functions[t_gsp_hierarchical] = Node_parse_hierarchical;
    Node_parse_functions[t_gsp_intoClause] = Node_parse_intoClause;
    Node_parse_functions[t_gsp_valueClause] = Node_parse_valueClause;
    Node_parse_functions[t_gsp_fetchFirstClause] = Node_parse_fetchFirstClause;
    Node_parse_functions[t_gsp_optimizeForClause] = Node_parse_optimizeForClause;
    Node_parse_functions[t_gsp_valueRowItem] = Node_parse_valueRowItem;
    Node_parse_functions[t_gsp_selectSqlNode] = Node_parse_selectSqlNode;
    Node_parse_functions[t_gsp_selectStatement] = Node_parse_selectSqlNode;
    Node_parse_functions[t_gsp_cte] = Node_parse_cte;
    Node_parse_functions[t_gsp_commentSqlNode] = Node_parse_commentSqlNode;
    Node_parse_functions[t_gsp_callSpec] = Node_parse_callSpec;
    Node_parse_functions[t_gsp_returnSqlNode] = Node_parse_returnSqlNode;
    Node_parse_functions[t_gsp_continueSqlNode] = Node_parse_continueSqlNode;
    Node_parse_functions[t_gsp_breakSqlNode] = Node_parse_breakSqlNode;
    Node_parse_functions[t_gsp_grantSqlNode] = Node_parse_grantSqlNode;
    Node_parse_functions[t_gsp_executeSqlNode] = Node_parse_executeSqlNode;
    Node_parse_functions[t_gsp_dropRoleSqlNode] = Node_parse_dropRoleSqlNode;
    Node_parse_functions[t_gsp_dropTriggerSqlNode] = Node_parse_dropTriggerSqlNode;
    Node_parse_functions[t_gsp_lockTableSqlNode] = Node_parse_lockTableSqlNode;
    Node_parse_functions[t_gsp_revokeSqlNode] = Node_parse_revokeSqlNode;
    Node_parse_functions[t_gsp_fetchSqlNode] = Node_parse_fetchSqlNode;
    Node_parse_functions[t_gsp_openSqlNode] = Node_parse_openSqlNode;
    Node_parse_functions[t_gsp_closeSqlNode] = Node_parse_closeSqlNode;
    Node_parse_functions[t_gsp_iterateSqlNode] = Node_parse_iterateSqlNode;
    Node_parse_functions[t_gsp_leaveSqlNode] = Node_parse_leaveSqlNode;
    Node_parse_functions[t_gsp_createFunctionSqlNode] = Node_parse_createFunctionSqlNode;
    Node_parse_functions[t_gsp_parameterDeclaration] = Node_parse_parameterDeclaration;
    Node_parse_functions[t_gsp_createProcedureSqlNode] = Node_parse_createProcedureSqlNode;
    Node_parse_functions[t_gsp_exceptionClause] = Node_parse_exceptionClause;
    Node_parse_functions[t_gsp_blockSqlNode] = Node_parse_blockSqlNode;
    Node_parse_functions[t_gsp_arrayAccess] = Node_parse_arrayAccess;
    Node_parse_functions[t_gsp_declareVariable] = Node_parse_declareVariable;
    Node_parse_functions[t_gsp_declareSqlNode] = Node_parse_declareSqlNode;
    Node_parse_functions[t_gsp_createTriggerUpdateColumn] = Node_parse_createTriggerUpdateColumn;
    Node_parse_functions[t_gsp_ifSqlNode] = Node_parse_ifSqlNode;
    Node_parse_functions[t_gsp_elseIfSqlNode] = Node_parse_elseIfSqlNode;
    Node_parse_functions[t_gsp_whileSqlNode] = Node_parse_whileSqlNode;
    Node_parse_functions[t_gsp_repeatSqlNode] = Node_parse_repeatSqlNode;
    Node_parse_functions[t_gsp_loopSqlNode] = Node_parse_loopSqlNode;
    Node_parse_functions[t_gsp_unknownStatement] = Node_parse_unknownStatement;
    Node_parse_functions[t_gsp_createTriggerSqlNode] = Node_parse_createTriggerSqlNode;
    Node_parse_functions[t_gsp_exceptionHandler] = Node_parse_exceptionHandler;
    Node_parse_functions[t_gsp_pivotClause] = Node_parse_pivotClause;
    Node_parse_functions[t_gsp_unPivotClause] = Node_parse_unPivotClause;
    Node_parse_functions[t_gsp_renameColumnSqlNode] = Node_parse_renameColumnSqlNode;
    Node_parse_functions[t_gsp_renameSequenceSqlNode] = Node_parse_renameSequenceSqlNode;
    Node_parse_functions[t_gsp_renameTableSqlNode] = Node_parse_renameTableSqlNode;
    Node_parse_functions[t_gsp_renameIndexSqlNode] = Node_parse_renameIndexSqlNode;
    Node_parse_functions[t_gsp_dropSequenceSqlNode] = Node_parse_dropSequenceSqlNode;
    Node_parse_functions[t_gsp_dropSynonymSqlNode] = Node_parse_dropSynonymSqlNode;
    Node_parse_functions[t_gsp_dropRowTypeSqlNode] = Node_parse_dropRowTypeSqlNode;
    Node_parse_functions[t_gsp_alterIndexSqlNode] = Node_parse_alterIndexSqlNode;
    Node_parse_functions[t_gsp_alterSequenceSqlNode] = Node_parse_alterSequenceSqlNode;
    Node_parse_functions[t_gsp_alterViewSqlNode] = Node_parse_alterViewSqlNode;
    Node_parse_functions[t_gsp_intoTableClause] = Node_parse_intoTableClause;
    Node_parse_functions[t_gsp_informixOuterClause] = Node_parse_informixOuterClause;
    Node_parse_functions[t_gsp_createRowTypeSqlNode] = Node_parse_createRowTypeSqlNode;
    Node_parse_functions[t_gsp_subscripts] = Node_parse_subscripts;
    Node_parse_functions[t_gsp_limitClause] = Node_parse_limitClause;
    Node_parse_functions[t_gsp_callSqlNode] = Node_parse_callSqlNode;
    Node_parse_functions[t_gsp_createDatabaseSqlNode] = Node_parse_createDatabaseSqlNode;
    Node_parse_functions[t_gsp_lockingClause] = Node_parse_lockingClause;
    Node_parse_functions[t_gsp_windowClause] = Node_parse_windowClause;
    Node_parse_functions[t_gsp_partitionClause] = Node_parse_partitionClause;
    Node_parse_functions[t_gsp_windowDef] = Node_parse_windowDef;
    Node_parse_functions[t_gsp_indices] = Node_parse_indices;
    Node_parse_functions[t_gsp_collectStatisticsSqlNode] = Node_parse_collectStatisticsSqlNode;
    Node_parse_functions[t_gsp_teradataWithClause] = Node_parse_teradataWithClause;
    Node_parse_functions[t_gsp_qualifyClause] = Node_parse_qualifyClause;
    Node_parse_functions[t_gsp_sampleClause] = Node_parse_sampleClause;
    Node_parse_functions[t_gsp_expandOnClause] = Node_parse_expandOnClause;
    Node_parse_functions[t_gsp_datatypeAttribute] = Node_parse_datatypeAttribute;
    Node_parse_functions[t_gsp_newVariantTypeArgument] = Node_parse_newVariantTypeArgument;
    Node_parse_functions[t_gsp_outputFormatPhrase] = Node_parse_outputFormatPhrase;
    Node_parse_functions[t_gsp_plsqlCreateTypeBody] = Node_parse_plsqlCreateTypeBody;
    Node_parse_functions[t_gsp_typeAttribute] = Node_parse_typeAttribute;
    Node_parse_functions[t_gsp_plsqlCreateType] = Node_parse_plsqlCreateType;
    Node_parse_functions[t_gsp_dmlEventClause] = Node_parse_dmlEventClause;
    Node_parse_functions[t_gsp_nonDmlTriggerClause] = Node_parse_nonDmlTriggerClause;
    Node_parse_functions[t_gsp_compoundDmlTriggerClause] = Node_parse_compoundDmlTriggerClause;
    Node_parse_functions[t_gsp_simpleDmlTriggerClause] = Node_parse_simpleDmlTriggerClause;
    Node_parse_functions[t_gsp_createPackageSqlNode] = Node_parse_createPackageSqlNode;
    Node_parse_functions[t_gsp_plsqlVarDeclStmt] = Node_parse_plsqlVarDeclStmt;
    Node_parse_functions[t_gsp_plsqlRaiseStmt] = Node_parse_plsqlRaiseStmt;
    Node_parse_functions[t_gsp_plsqlLoopStmt] = Node_parse_plsqlLoopStmt;
    Node_parse_functions[t_gsp_plsqlCaseStmt] = Node_parse_plsqlCaseStmt;
    Node_parse_functions[t_gsp_plsqlForallStmt] = Node_parse_plsqlForallStmt;
    Node_parse_functions[t_gsp_plsqlElsifStmt] = Node_parse_plsqlElsifStmt;
    Node_parse_functions[t_gsp_plsqlIfStmt] = Node_parse_plsqlIfStmt;
    Node_parse_functions[t_gsp_plsqlGotoStmt] = Node_parse_plsqlGotoStmt;
    Node_parse_functions[t_gsp_plsqlExitStmt] = Node_parse_plsqlExitStmt;
    Node_parse_functions[t_gsp_plsqlAssignStmt] = Node_parse_plsqlAssignStmt;
    Node_parse_functions[t_gsp_plsqlCursorDeclStmt] = Node_parse_plsqlCursorDeclStmt;
    Node_parse_functions[t_gsp_plsqlRecordTypeDefStmt] = Node_parse_plsqlRecordTypeDefStmt;
    Node_parse_functions[t_gsp_plsqlVarrayTypeDefStmt] = Node_parse_plsqlVarrayTypeDefStmt;
    Node_parse_functions[t_gsp_plsqlTableTypeDefStmt] = Node_parse_plsqlTableTypeDefStmt;
    Node_parse_functions[t_gsp_plsqlNullStmt] = Node_parse_plsqlNullStmt;
    Node_parse_functions[t_gsp_plsqlPipeRowStmt] = Node_parse_plsqlPipeRowStmt;
    Node_parse_functions[t_gsp_bindArgument] = Node_parse_bindArgument;
    Node_parse_functions[t_gsp_execImmeNode] = Node_parse_execImmeNode;
    Node_parse_functions[t_gsp_plsqlOpenforStmt] = Node_parse_plsqlOpenforStmt;
    Node_parse_functions[t_gsp_plsqlOpenStmt] = Node_parse_plsqlOpenStmt;
    Node_parse_functions[t_gsp_plsqlBasicStmt] = Node_parse_plsqlBasicStmt;
    Node_parse_functions[t_gsp_trigger_event] = Node_parse_trigger_event;
    Node_parse_functions[t_gsp_mssql_executeAsSqlNode] = Node_parse_mssql_executeAsSqlNode;
    Node_parse_functions[t_gsp_mssql_executeSqlNode] = Node_parse_mssql_executeSqlNode;
    Node_parse_functions[t_gsp_mssql_execParameter] = Node_parse_mssql_execParameter;
    Node_parse_functions[t_gsp_mssql_dropDbObjectSqlNode] = Node_parse_mssql_dropDbObjectSqlNode;
    Node_parse_functions[t_gsp_mssql_setSqlNode] = Node_parse_mssql_setSqlNode;
    Node_parse_functions[t_gsp_mssql_beginTranSqlNode] = Node_parse_mssql_beginTranSqlNode;
    Node_parse_functions[t_gsp_mssql_raiserrorSqlNode] = Node_parse_mssql_raiserrorSqlNode;
    Node_parse_functions[t_gsp_mssql_gotoSqlNode] = Node_parse_mssql_gotoSqlNode;
    Node_parse_functions[t_gsp_mssql_labelSqlNode] = Node_parse_mssql_labelSqlNode;
    Node_parse_functions[t_gsp_mssql_deallocateSqlNode] = Node_parse_mssql_deallocateSqlNode;
    Node_parse_functions[t_gsp_mssql_beginDialogSqlNode] = Node_parse_mssql_beginDialogSqlNode;
    Node_parse_functions[t_gsp_mssql_sendOnConversationSqlNode] = Node_parse_mssql_sendOnConversationSqlNode;
    Node_parse_functions[t_gsp_mssql_endConversationSqlNode] = Node_parse_mssql_endConversationSqlNode;
    Node_parse_functions[t_gsp_mssql_revertSqlNode] = Node_parse_mssql_revertSqlNode;
    Node_parse_functions[t_gsp_mssql_goSqlNode] = Node_parse_mssql_goSqlNode;
    Node_parse_functions[t_gsp_mssql_useSqlNode] = Node_parse_mssql_useSqlNode;
    Node_parse_functions[t_gsp_mssql_printSqlNode] = Node_parse_mssql_printSqlNode;
    Node_parse_functions[t_gsp_mssql_computeClause] = Node_parse_mssql_computeClause;
    Node_parse_functions[t_gsp_mssql_computeClauseItem] = Node_parse_mssql_computeClauseItem;
    Node_parse_functions[t_gsp_mssql_computeExpr] = Node_parse_mssql_computeExpr;
    Node_parse_functions[t_gsp_mssql_containsTable] = Node_parse_mssql_containsTable;
    Node_parse_functions[t_gsp_mssql_freeTable] = Node_parse_mssql_freeTable;
    Node_parse_functions[t_gsp_mssql_openXML] = Node_parse_mssql_openXML;
    Node_parse_functions[t_gsp_mssql_openRowSet] = Node_parse_mssql_openRowSet;
    Node_parse_functions[t_gsp_mssql_openQuery] = Node_parse_mssql_openQuery;
    Node_parse_functions[t_gsp_mssql_openDatasource] = Node_parse_mssql_openDatasource;
    Node_parse_functions[t_gsp_mssql_tableHint] = Node_parse_mssql_tableHint;
    Node_parse_functions[t_gsp_mssql_bulkInsertSqlNode] = Node_parse_mssql_bulkInsertSqlNode;
    Node_parse_functions[t_gsp_mssql_outputClause] = Node_parse_mssql_outputClause;
    Node_parse_functions[t_gsp_mssql_updateTextSqlNode] = Node_parse_mssql_updateTextSqlNode;
    Node_parse_functions[t_gsp_db2_signal] = Node_parse_db2_signal;
    Node_parse_functions[t_gsp_db2_compoundSqlNode] = Node_parse_db2_compoundSqlNode;
    Node_parse_functions[t_gsp_db2_triggerAction] = Node_parse_db2_triggerAction;
    Node_parse_functions[t_gsp_db2_callStmtSqlNode] = Node_parse_db2_callStmtSqlNode;
    Node_parse_functions[t_gsp_db2_forSqlNode] = Node_parse_db2_forSqlNode;
    Node_parse_functions[t_gsp_db2_iterateStmtSqlNode] = Node_parse_db2_iterateStmtSqlNode;
    Node_parse_functions[t_gsp_db2_leaveStmtSqlNode] = Node_parse_db2_leaveStmtSqlNode;
    Node_parse_functions[t_gsp_db2_setSqlNode] = Node_parse_db2_setSqlNode;
    Node_parse_functions[t_gsp_db2_whileSqlNode] = Node_parse_db2_whileSqlNode;
    Node_parse_functions[t_gsp_db2_repeatSqlNode] = Node_parse_db2_repeatSqlNode;
    Node_parse_functions[t_gsp_db2_gotoSqlNode] = Node_parse_db2_gotoSqlNode;
    Node_parse_functions[t_gsp_db2_loopSqlNode] = Node_parse_db2_loopSqlNode;
}
