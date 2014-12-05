#pragma once
#include "Constants.h"
#include "Camera.h"
#include "List.h"
#include "Sprite.h"
#include "GameEngine.h"
#include "Vector.h"
#include "CollisionHandler.h"
#include "Tile.h"
#include "Entity.h"
#include "Sky.h"

CLASS(Scene) {
    SDL_Rect walkableBounds;
    Sprite* background;
    Vector* entities;
    Vector* triggers;
    Vector* bodies;
    Vector* shapes;
    Camera* camera;
    GameEngine* engine;
    struct Sky* sky;
    int gravity;
};

scm_t_bits Scene_guile_tag;

Scene* Scene_create(GameEngine* engine, Scene* lastScene);
Scene* Scene_createForPath(GameEngine* engine, const char* filePath, Scene* lastScene);
void Scene_destroy(Scene* this);

void Scene_update(Scene* this, RawTime dt);
void Scene_draw(Scene* this, SDL_Renderer* renderer);

void Scene_transfereEntity(Scene* this, struct Entity* entity, Scene* other);
void Scene_addEntity(Scene* this, struct Entity* entity);

void Scene_setBounds(Scene* this, int x, int y, int w, int h);

SDL_Point Scene_positionForTileId(Scene* this, const int tileId);

void Scene_openDoors(struct Scene* this, int type);
void Scene_spawnCollectable(Scene* this, int tileId, int type);

Tile* Scene_getTile(Scene* this, SDL_Point tilePos);
SDL_Point Scene_positionToTilePosition(SDL_Point position);

void Scene_guile_registerType();

cpShape* Scene_makeGround(Scene* this, cpVect a, cpVect b);
