/*
 * Game.c
 *
 *  Created on: 06.01.2013
 *      Author: nehmulos
 */

#include "Game.h"
#include "GameEngine.h"
#include "CollisionHandler.h"
#include <chipmunk/chipmunk.h>

Game* Game_create(GameEngine* engine) {
    Game* this = malloc(sizeof(Game));
    this->engine = engine;
    this->module = GameModule_create(this);
    this->module->destroy = Game_destroy;
    this->module->init = Game_init;
    this->module->update = Game_update;
    this->module->draw = Game_draw;
    return this;
}

void Game_destroy(void* context) {
    CANCEL_IF_FALSE(context);
    Game* this = context;

    Player_destroy(this->player);
    // important: must be after player destroy, since player calls respawn on destroy,
    // TODO this will change though when destroy will be preceeded by a death event
    Scene_destroy(this->scene);

    // lua propbaby kills everything
    /* wtf is wrong with this too much headache to fix
    for (int i=0; i < PLAYER_COUNT; ++i) {
        Player_destroy(this->players[i]);
    }
    free(this->players);
    */
    free(this);
}

void Game_init(void* context) {
    Game* this = context;
    this->scene = NULL;

    this->scene = Scene_createForPath(this->engine, "scenes/test.scm", this->scene);
    Game_setupPlayer(this, 0, this->scene);

    CollisionHandler_register(this->engine->space);
}

void Game_recreateScene(Game* this) {
    Scene* oldScene = this->scene;
    Scene* newScene = Scene_createForPath(this->engine, "scenes/test.scm", this->scene);
    this->player->scene = newScene;
    this->scene = newScene;
    Scene_destroy(oldScene);
}

void Game_setupPlayer(Game* this, int i, Scene* scene) {

    this->player = Player_create(scene, this->engine->input[i]);
    Scene_setBounds(scene, 0, 0, SCENE_WIDTH, SCENE_HEIGHT);
}

void Game_update(void* context, RawTime dt) {
    Game* this = context;

    Scene_update(this->scene, dt);
    Player_update(this->player, dt);

    static bool reload_wasDown = false; // FIXME remove static
    bool reload_isDown = Input_isDown(this->player->input, reloadScene);

    if (reload_wasDown && !reload_isDown) {
        Game_recreateScene(this);
    }
    reload_wasDown = reload_isDown;
}

void Game_draw(void* context, SDL_Renderer* renderer) {
    Game* this = context;
    Scene_draw(this->scene, renderer);
    Player_draw(this->player, renderer);

 }
