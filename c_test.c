#include <stdio.h>
#include <stack_debug.h>

#define LOG_ERR     0
#define LOG_NORMAL  1
#define LOG_DEBUG   2
#define LOG_LEVEL   LOG_DEBUG

#define _log(lvl, fmt, ...)\
   if(lvl >= LOG_LEVEL){\
       fprintf (stdout, fmt, __VA_ARGS__);\
   }

#define Debug(fmt, ...) _log(LOG_DEBUG, fmt, __VA_ARGS__);
   


/*copy a table of the index on the stack */
void t_shallow_copy(lua_State* L, int index) {
    lua_newtable(L);
    lua_pushnil(L);
    while(lua_next(L, index) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_settable(L, -4);
    }
}

int main (void) {
    lua_State *L = lua_open();
   
    lua_newtable(L);

    lua_pushstring(L, "key-a");
    lua_pushstring(L, "I-am-value-9");

    lua_settable(L, -3);

    t_shallow_copy(L, 1);
    lua_pushstring(L, "key-a");
    lua_pushstring(L, "I-am-value-8888");
    lua_settable(L, -3);
    stackDump(L, "table");
    PrintTable(L);
    lua_pop(L, 1);
    PrintTable(L);

    Debug("Imadfdfa[%d] [%s]\n",1231, "dadd");
    

 return 0;

}
