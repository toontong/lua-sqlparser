CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT(gsqlparser)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -fPIC")
SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O0 -g -fPIC")

option(USE_LUA "Use Lua (also called 'C' Lua) includes (default)" ON)
option(USE_LUAJIT "Use LuaJIT includes instead of 'C' Lua ones (recommended, if you're using LuaJIT, but disabled by default)")
set(USE_LUA_VERSION 5.1 CACHE STRING "Set the Lua version to use (default: 5.1)")

if(USE_LUAJIT)
# Find luajit
        find_package(LuaJIT REQUIRED)
        set(USE_LUA OFF)
# / Find lua
endif()

if(USE_LUA)
# Find lua
#        find_package(Lua ${USE_LUA_VERSION} EXACT REQUIRED)
# / Find lua
endif()


# Basic configurations
SET(INSTALL_CMOD share/lua/cmod CACHE PATH "Directory to install Lua binary modules (configure lua via LUA_CPATH)")
SET(GSP_DIR ${CMAKE_SOURCE_DIR}/general_sql_parser)


INCLUDE_DIRECTORIES(. ${GSP_DIR}/core/)
INCLUDE_DIRECTORIES(. ${GSP_DIR}/ext/collection/includes/)
INCLUDE_DIRECTORIES(. ${GSP_DIR}/ext/expr_traverse/)
INCLUDE_DIRECTORIES(. ${GSP_DIR}/ext/modifysql/)
INCLUDE_DIRECTORIES(. ${GSP_DIR}/ext/node_visitor/)

AUX_SOURCE_DIRECTORY(. ${CMAKE_SOURCE_DIR})

LINK_DIRECTORIES(${GSP_DIR}/lib)
LINK_LIBRARIES(gspcollection gspcore)

target_link_libraries(lua)

SET(LUA_C_MODEL_FILE gsqlparser.c Parser.c ENodeType.c Node.c Statement.c
    ${GSP_DIR}/ext/node_visitor/node_visitor.c
    ${GSP_DIR}/ext/expr_traverse/expr_traverse.c
    ${GSP_DIR}/ext/modifysql/modifysql.c)

INCLUDE_DIRECTORIES((${CMAKE_SOURCE_DIR}/include/ ${LUA_INCLUDE_DIR}))

ADD_LIBRARY(lua_cmod_sql_parser MODULE
    ${LUA_C_MODEL_FILE} )
SET_TARGET_PROPERTIES(lua_cmod_sql_parser PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(lua_cmod_sql_parser PROPERTIES OUTPUT_NAME "gsqlparser")
# / build gsqlparser.so
