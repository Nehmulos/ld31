#pragma once
#include "Constants.h"
#include "GameModule.h"
#include "GameEngine.h"

#include "Scene.h"
#include "Entity.h"
#include "Player.h"

#include "ui/UiNode.h"

#include <chipmunk/chipmunk.h>

CLASS(Game) {
    Scene* scene;
    Player* player;

    GameModule* module;
    GameEngine* engine;
};

Game* Game_create(GameEngine* engine);
void Game_init(void* context);
void Game_destroy(void* context);
void Game_update(void* context, RawTime dt);
void Game_draw(void* context, SDL_Renderer* renderer);

void Game_setupPlayer(Game* this, int i, Scene* scene);
