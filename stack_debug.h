#ifndef _STACK_DEBUG_H_
#define _STACK_DEBUG_H_ 1
#define _G_DEBUG_ 0

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static void stackDump(lua_State *L, char *split) {
    if (_G_DEBUG_) {
      return;
    }

    int i;
    int top = lua_gettop(L);
    printf("-------Begin of [%s] size of stack = %d -------\n", split, top);
    for (i = 1; i <= top; i++) {  /* repeat for each level */
       int t = lua_type(L, i);
       switch (t) {
       case LUA_TSTRING:  /* strings */
           printf("%2d: `%s`",i, lua_tostring(L, i));
           break;
       case LUA_TBOOLEAN:  /* booleans */
           printf(lua_toboolean(L, i) ? "%2d: true" : "%2d: false", i);
           break;
       case LUA_TNUMBER:  /* numbers */
           printf("%2d: %g", i, lua_tonumber(L, i));
           break;
       default:  /* other values */
           printf("%2d: other{%s}", i, lua_typename(L, i));
           break;
       }
       printf(" | ");  /* put a separator */
    }
    printf("\n------- End of [%s] size of stack = %d -------\n\n", split, top);
}

/*copy a table of the index on the stack */
static void shallow_copy(lua_State* L, int index) {
        return;
    lua_newtable(L);
    printf("table{\n");
    lua_pushnil(L);
    printf("psh nill in table{\n");
    while(lua_next(L, index) != 0) {
        lua_pushvalue(L, -2);
        
        printf("push value in table{\n");
        lua_insert(L, -2);
        printf("insert -2 table{\n");
        lua_settable(L, -4);
        printf("set table{\n");
    }
}

static void PrintTable(lua_State *L)
{
    lua_pushnil(L);

    printf("table{\n");
    while(lua_next(L, -2) != 0)
    {
        if(lua_isstring(L, -1))
          printf("\t%s = %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
        else if(lua_isnumber(L, -1))
          printf("\t%s = %d\n", lua_tostring(L, -2), lua_tonumber(L, -1));
        else if(lua_istable(L, -1))
          PrintTable(L);

        lua_pop(L, 1);
    }
    printf("} -- end table.\n\n");
}

#endif
