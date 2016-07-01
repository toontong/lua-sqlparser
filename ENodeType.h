#ifndef _ENodeType_Header_H
#define _ENodeType_Header_H 1

#include <stdlib.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

void Enum_register_on_luaopen(lua_State *L);

void EVendor_init(lua_State *L);
void EStmtType_init(lua_State *L);
void ECreateType_init(lua_State *L);
void EExecType_init(lua_State *L);
void ESetType_init(lua_State *L);

/* TODO: 以下Enum类型还未加入
void gsp_efindsqlstatetype_init(lua_State *L);
void EErrorType_init(lua_State *L);
void gsp_token_code_init(lua_State *L);
void ETokenStatus_init(lua_State *L);
void EJoinSource_init(lua_State *L);
void EJoinType_init(lua_State *L);
void EFireMode_init(lua_State *L);
void ETriggerMode_init(lua_State *L);
void EStoreProcedureMode_init(lua_State *L);
void EParameterMode_init(lua_State *L);
void EHowtoSetValue_init(lua_State *L);
void EWhatDeclared_init(lua_State *L);
void EInsertValue_init(lua_State *L);
void EIndexType_init(lua_State *L);
void EAggregateType_init(lua_State *L);
void EAlterTableOptionType_init(lua_State *L);
void ETableSource_init(lua_State *L);
void EConstraintType_init(lua_State *L);
void EKeyReferenceType_init(lua_State *L);
void ESetOperator_init(lua_State *L);
void EDataType_init(lua_State *L);
void EFunctionType_init(lua_State *L);
void EDBObjectType_init(lua_State *L);
void ENodeType_init(lua_State *L);
void EExpressionType_init(lua_State *L);
void gsp_dbvendor_init(lua_State *L);
void EAccessMode_init(lua_State *L);
void EQeuryClause_init(lua_State *L);
void EConstantType_init(lua_State *L);
void EKeyActionType_init(lua_State *L);
void EMatchType_init(lua_State *L);
void EDistinctType_init(lua_State *L);
void gsp_EDeclareType_init(lua_State *L);
void gsp_EVariableType_init(lua_State *L);
*/

#endif
