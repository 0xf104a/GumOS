#include "lenginestart.h"
#include <kernel/klog.h>

int l_klog(lua_State *L){
    msg_type tp=(msg_type)lua_tonumber(L, 1);
    const char* appname=luaL_checkstring(L, 2);
    const char* msg=luaL_checkstring(L,3);
    klog(tp,appname,msg);
    return 0;
}
