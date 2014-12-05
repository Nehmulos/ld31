#pragma once
#include "Constants.h"
#include "Entity.h"
#include "Input.h"

#define PLAYER_SPAWN_ENEMY_DELAY 2000
#define PLAYER_SELECTION_CHANGE_DELAY 300

CLASS(ControlledEntity) {
    void* originalContext;
    void (*originalDraw)(void* context, SDL_Renderer*, Camera* camera);
    void (*originalDestroy)(void* context);
    Entity* entity;
    void* player;
    bool destroyOnRelease;
    Sprite* indicator;
};

CLASS(Player) {
    Input* input;
    Entity* entity; // a fallback for controlledEntity
    ControlledEntity controlledEntity;
    struct Scene* scene;
};

Player* Player_create(struct Scene* scene, Input* input);
void Player_destroy(void* context);
void Player_update(void* context, RawTime dt);
void Player_draw(void* context, SDL_Renderer* renderer);
Entity* Player_spawnPlayerEntity(struct Player* this);

void Player_destroyEntity(void* context);
void Player_drawEntity(void* context, SDL_Renderer* renderer, Camera* camera);
void Player_updateEntity(void* context, RawTime dt);
void Player_onEntityHealthModified(void* context);

Entity* _Player_onEntityDestroyed(void* context);

void Player_processInput(Player* this);
void Player_loadIndicator(Player* this);

void ControlledEntity_set(ControlledEntity* this, Entity* entity);
void ControlledEntity_release(ControlledEntity* this);
void ControlledEntity_draw(void* context, SDL_Renderer* renderer, Camera* camera);
