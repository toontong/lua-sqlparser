#ifndef _GSP_NODE_HREAD_H__
#define _GSP_NODE_HREAD_H__ 1
#include "node_visitor.h"
#include "gsp_base.h"
#include "gsp_node.h"
#include "gsp_list.h"
#include "gsp_sourcetoken.h"
#include "gsp_sqlparser.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static const char* NodeMetatable = "gsqlparser.Node";

// Node object
typedef struct {
    gsp_node *_node;
    gsp_sqlparser *_parser;
} SqlNode;

// Object register
int Node_register_on_luaopen(lua_State *L);

// C utils methods
SqlNode *Node_FromNode(lua_State *L, gsp_node *node, Statement *stmt);
int Node_getattro(lua_State *L);
int Node_set_attr_n(lua_State *L, const char* attr, int v);

// int Node_list_iterator(PyObject *o);

// Lua export Module methods
SqlNode *Node_new(lua_State *L);

// Lua export Object methods
int Node_dealloc(lua_State *L);
int Node_get_text(lua_State *L);
int Node_get_position(lua_State *L);


// Lua export Object methods metatable
static const struct luaL_Reg Node_methods_metatable[] = {
    {"__gc",           Node_dealloc},
    {"get_text",       Node_get_text}, // "get_text()\nGets the string representation of the node.\n\n:returns: str" },
    {"get_position",   Node_get_position}, // "get_position()\nGets the node's position in the query.\n\n:returns: tuple" },
    {NULL, NULL}  /* Sentinel */
};


#define MAX_NODE_PARSE_FUNCS 300 // arbitrary

SqlNode *Node_parse_listcell(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_sql_statement(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_dummy(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_constant(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_objectname(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_expr(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_objectAccess(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_aliasClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_resultColumn(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_trimArgument(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_orderByItem(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_orderBy(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_keepDenseRankClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_analyticFunction(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_functionCall(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_whenClauseItem(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_caseExpression(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_intervalExpression(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_precisionScale(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_typename(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_keyReference(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_keyAction(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_constraint(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mergeInsertClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mergeUpdateClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mergeDeleteClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mergeWhenClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_dataChangeTable(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_fromTable(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_table(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mergeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_alterTableOption(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_alterTableSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createSequenceSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createSynonymSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createDirectorySqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_dropViewSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_dropTableSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_dropIndexItem(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_dropIndexSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_truncateTableSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_viewAliasItem(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_viewAliasClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createViewSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createMaterializedViewSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createMaterializedViewLogSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createIndexSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_commitSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_rollbackSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_saveTransSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_columnDefinition(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_tableElement(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createTableSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_returningClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_isolationClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_includeColumns(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_deleteSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_updateSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_multiTarget(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_insertRest(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_insertValuesClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_insertIntoValue(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_insertCondition(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_insertSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_whereClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_joinExpr(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_tableSamplePart(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_tableSample(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_pxGranule(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_flashback(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_forUpdate(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_groupingSetItem(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_groupingSet(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_rollupCube(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_gruopByItem(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_groupBy(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_selectDistinct(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_topClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_hierarchical(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_intoClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_valueClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_fetchFirstClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_optimizeForClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_valueRowItem(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_selectSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_cte(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_commentSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_callSpec(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_returnSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_continueSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_breakSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_grantSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_executeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_dropRoleSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_dropTriggerSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_lockTableSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_revokeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_fetchSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_openSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_closeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_iterateSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_leaveSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createFunctionSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_parameterDeclaration(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createProcedureSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_exceptionClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_blockSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_arrayAccess(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_declareVariable(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_declareSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createTriggerUpdateColumn(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_ifSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_elseIfSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_whileSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_repeatSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_loopSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_unknownStatement(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createTriggerSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_exceptionHandler(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_pivotClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_unPivotClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_renameColumnSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_renameSequenceSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_renameTableSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_renameIndexSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_dropSequenceSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_dropSynonymSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_dropRowTypeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_alterIndexSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_alterSequenceSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_alterViewSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_intoTableClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_informixOuterClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createRowTypeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_subscripts(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_limitClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_callSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createDatabaseSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_lockingClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_windowClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_partitionClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_windowDef(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_indices(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_collectStatisticsSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_teradataWithClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_qualifyClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_sampleClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_expandOnClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_datatypeAttribute(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_newVariantTypeArgument(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_outputFormatPhrase(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlCreateTypeBody(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_typeAttribute(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlCreateType(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_dmlEventClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_nonDmlTriggerClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_compoundDmlTriggerClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_simpleDmlTriggerClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_createPackageSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlVarDeclStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlRaiseStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlLoopStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlCaseStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlForallStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlElsifStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlIfStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlGotoStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlExitStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlAssignStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlCursorDeclStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlRecordTypeDefStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlVarrayTypeDefStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlTableTypeDefStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlNullStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlPipeRowStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_bindArgument(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_execImmeNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlOpenforStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlOpenStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_plsqlBasicStmt(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_trigger_event(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_executeAsSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_executeSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_execParameter(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_dropDbObjectSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_setSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_beginTranSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_raiserrorSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_gotoSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_labelSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_deallocateSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_beginDialogSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_sendOnConversationSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_endConversationSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_revertSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_goSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_useSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_printSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_computeClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_computeClauseItem(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_computeExpr(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_containsTable(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_freeTable(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_openXML(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_openRowSet(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_openQuery(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_openDatasource(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_tableHint(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_bulkInsertSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_outputClause(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_mssql_updateTextSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_db2_signal(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_db2_compoundSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_db2_triggerAction(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_db2_callStmtSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_db2_forSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_db2_iterateStmtSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_db2_leaveStmtSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_db2_setSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_db2_whileSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_db2_repeatSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_db2_gotoSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_db2_loopSqlNode(lua_State *L, gsp_node *node, Statement *stmt) ;
SqlNode *Node_parse_list(lua_State *L, gsp_node *node, Statement *stmt);

typedef SqlNode *(*NodeParseFunc)(lua_State *L, gsp_node *, Statement *);
static NodeParseFunc Node_parse_functions[MAX_NODE_PARSE_FUNCS];

void Node_init();
#endif
