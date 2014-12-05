#include "Trigger.h"
#include "Utils.h"

#define TRIGGER_MTABLE "yotm.Trigger"

void Trigger_destroy(struct Trigger* this) {
    free(this);
}

/* int Trigger_create(lua_State* l) { */
/*  Scene* scene = Scene_checkfromLua(l, 1); */
/*  int x = luaL_checkinteger(l, 2); */
/*  int luaCallback = luaL_ref(l, LUA_REGISTRYINDEX); */
/*  lua_pop(l, 1); */

/*  Trigger* this = lua_newuserdata(l, sizeof(Trigger)); */
/*  luaL_getmetatable(l, TRIGGER_MTABLE); */
/*  lua_setmetatable(l, -2); */

/*  this->entity = Entity_create(this, scene, NULL); */
/*  this->entity->physics.bounds.x = x; */
/*  this->luaCallbackReference = luaCallback; */
/*  stackDump(l); */
/*  return 1; */
/* } */

/* int Trigger_luaDestroy(lua_State* l) { */
/*  Trigger* this = Trigger_checkFromLua(l, 1); */
/*  luaL_unref(l, LUA_REGISTRYINDEX, this->luaCallbackReference); */
/*  return 0; */
/* } */

/* void Trigger_setScene(Trigger* this, Scene* scene) { */
/*  this->entity->physics.bounds.h = scene->camera->bounds.h; */
/* } */


/* void Trigger_exportToLua(lua_State* l) { */
/*  static struct luaL_Reg methods[] = { */
/* //      {"addBackground", Scene_luaAddBackground}, */
/*      {NULL, NULL} */
/*  }; */

/*  static struct luaL_Reg functions[] = { */
/*      {"create", Trigger_luaCreate}, */
/*      {NULL, NULL} */
/*  }; */

/*  lua_createLib(l, TRIGGER_MTABLE, "Trigger", functions, methods, Trigger_luaDestroy); */
/* } */

/* Trigger* Trigger_checkFromLua(lua_State* l, int idx) { */
/*  CHECK_LUA_USERCLASS(l, Trigger, TRIGGER_MTABLE, idx); */
/* } */
