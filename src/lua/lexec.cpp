#include "lexec.h"
#include "lenginestart.h"

extern "C"{
    #include <lua/lualib.h>
    #include <lua/lauxlib.h>
    #include <lua/lua.h>
}

#include "lenginestart.h"
#include <kernel/klog.h>
#include <kernel/io.h>
#include <stdlib.h>
#include <string.h>

void print_error(lua_State* state) {
  // The error message is on top of the stack.
  // Fetch it, print it and then pop it off the stack.
  const char* message = lua_tostring(state, -1);
  klog(ERROR,"lua","%s",message);
  lua_pop(state, 1);
}

bool liblua_exec(char* buffer, char* name){
    auto L = luaL_newstate();
    // Push functions
    lua_pushcfunction(L, l_klog);
    lua_setglobal(L, "klog");
    // Load all Lua libraries
    luaL_openlibs(L);
    // Create co-routine
    auto coroutine = lua_newthread(L);

    if (luaL_loadbuffer(coroutine,buffer,strlen(buffer),name) == LUA_OK) {
        klog(INFO,"lua","Loaded program");
    } else {
        klog(ERROR,"lua", "Failed to load program!");
        return false;
    }
    lua_pcall(coroutine,0,0,0);
    return true;
}

bool liblua_exec_file(char* fname){
    klog(DEBUG,"lua", "Executing file: %s",fname);
    char *buffer=NULL;
    if(!sreadf(fname,&buffer)){
        klog(WARN, "lua","Nothing to execute from %s",fname);
        return false;
    }
    klog(DEBUG,"lua","buffer=%s",buffer);
    return liblua_exec(buffer,"lua");
}