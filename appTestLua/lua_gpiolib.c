/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

typedef void (*gpio_f)(int, int);

static int _gpio_process( lua_State *L, gpio_f f )
{
    int port, pin;

    switch( lua_gettop(L) )
    {
    case 2:
        port = luaL_checkinteger(L, 1);
        pin = luaL_checkinteger(L, 2);
        (*f)( port, 1<<pin );
        break;
    default:
        lua_writestring( "parms: (port, pin)", 18 );
        lua_writeline();
        break;
    }
    return 0;
}

static int lua_gpio_out(lua_State *L)
{
    return _gpio_process( L, hal_gpio_set_output );
}

static int lua_gpio_in(lua_State *L)
{
    return _gpio_process( L, hal_gpio_set_input );
}

static int lua_gpio_set(lua_State *L)
{
    return _gpio_process( L, hal_gpio_set );
}

static int lua_gpio_clr(lua_State *L)
{
    return _gpio_process( L, hal_gpio_clr );
}

static int lua_gpio_toggle(lua_State *L)
{
    return _gpio_process( L, hal_gpio_toggle );
}

static int lua_gpio_get(lua_State *L)
{
    int port, pin;

    switch( lua_gettop(L) )
    {
    case 1:
        port = luaL_checkinteger(L, 1);
        lua_pushinteger( L, hal_gpio_get(port, -1) );
        return 1;
        break;
    case 2:
        port = luaL_checkinteger(L, 1);
        pin = luaL_checkinteger(L, 2);
        lua_pushboolean( L, hal_gpio_get(port, 1<<pin) );
        return 1;
        break;
    default:
        lua_writestring( "parms: (port[, pin])", 20 );
        lua_writeline();
        break;
    }
    return 0;
}


static const struct luaL_Reg gpiolib[] =
{
    { "input", lua_gpio_in },
    { "output", lua_gpio_out },
    { "set", lua_gpio_set },
    { "clr", lua_gpio_clr },
    { "toggle", lua_gpio_toggle },
    { "get", lua_gpio_get },
    { NULL, NULL}
};


LUAMOD_API int luaopen_gpiolib(lua_State *L)
{
    luaL_newlib(L, gpiolib);
    return 1;
}

