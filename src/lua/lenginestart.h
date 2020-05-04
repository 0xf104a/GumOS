#ifndef LENGINESTART_H
#define LENGINESTART_H

extern "C"{
    #include <lua/lualib.h>
    #include <lua/lauxlib.h>
    #include <lua/lua.h>
}

int l_klog(lua_State *L);

#endif