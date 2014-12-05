#include "Player.h"
#include "AnimatedSprite.h"
#include "TextureCache.h"
#include "Scene.h"
#include "PlayerCursor.h"
#include "Utils.h"
#include <chipmunk/chipmunk.h>


Player* Player_create(Scene* scene, Input* input) {
    Player* this = malloc(sizeof(Player));
    this->input = input;
    this->scene = scene;
    this->controlledEntity.player = this;
    this->controlledEntity.entity = NULL;
    this->controlledEntity.originalContext = NULL;
    this->controlledEntity.originalDestroy = NULL;
    this->controlledEntity.originalDraw = NULL;
    this->controlledEntity.destroyOnRelease= false;
    this->controlledEntity.indicator = NULL;

    Player_loadIndicator(this);

    this->entity = Player_spawnPlayerEntity(this);
    ControlledEntity_set(&this->controlledEntity, this->entity);

    return this;
}

Entity* _Player_onEntityDestroyed(void* context) {
    Player* this = context;
    if (this->controlledEntity.entity == this->entity) {
        this->entity = NULL;
        this->entity = Player_spawnPlayerEntity(this);
    }
    return this->entity;
}

void Player_destroy(void* context) {
    CANCEL_IF_FALSE(context);
    Player* this = context;
    this->controlledEntity.destroyOnRelease = false;
    ControlledEntity_release(&this->controlledEntity);
    free(this);
}

void Player_loadIndicator(Player* this) {
    withSprintf(path, "images/%s/indicator.png", anyArgs("red"), {
        this->controlledEntity.indicator = Sprite_create(TextureCache_getForUnconstantPath(this->scene->engine->textureCache, path));
    });
}

void Player_updateEntity(void* context, RawTime dt) {
}

void Player_destroyEntity(void* context) {
}

void Player_onEntityHealthModified(void* context) {
    //ControlledEntity* ce = context;
    //Player* this = ce->player;
    //Healthbar_set(this->sidebarUi->healthbar, this->entity->health, this->entity->maxHealth);
}

void Player_update(void* context, RawTime dt) {
    Player* this = context;
    Player_processInput(this);
}

Entity* Player_spawnPlayerEntity(Player* this) {
    AnimatedSprite* sprite = NULL;
    withSprintf(path, "images/%s/knight.png", anyArgs("red"), {
        sprite = AnimatedSprite_create(Sprite_create(TextureCache_getForUnconstantPath(this->scene->engine->textureCache, path)));
    });

    Animation* idleAnimation = Animation_create("idle", true);
    List_pushBack(idleAnimation->frames, Frame_create(0,0, 32, 32, 200));
    List_pushBack(idleAnimation->frames, Frame_create(32,0, 32, 32, 200));
    List_pushBack(idleAnimation->frames, Frame_create(64,0, 32, 32, 200));
    List_pushBack(idleAnimation->frames, Frame_create(32,0, 32, 32, 200));
    List_pushBack(sprite->animations, idleAnimation);

    Animation* attack1PrepareAnimation = Animation_create(ANIMATION_PREPARE_ATTACK1, false);
    List_pushBack(attack1PrepareAnimation->frames, Frame_create(0, 32, 32, 32, 60));
    List_pushBack(attack1PrepareAnimation->frames, Frame_create(32,32, 32, 32, 60));
    List_pushBack(attack1PrepareAnimation->frames, Frame_create(0, 32, 32, 32, 60));
    List_pushBack(sprite->animations, attack1PrepareAnimation);

    Animation* attack1Animation = Animation_create(ANIMATION_ATTACK1, false);
    List_pushBack(attack1Animation->frames, Frame_create(64,32, 36, 32, 120));
    List_pushBack(sprite->animations, attack1Animation);

    AnimatedSprite_setFrame(sprite, ((Frame*)idleAnimation->frames->first->data)->rect);
    sprite->progress.animation = idleAnimation;

    sprite->sprite->flip = false;

    Entity* entity = Entity_create(this, this->scene, sprite);
    entity->draw = Player_drawEntity;
    entity->update = Player_updateEntity;
    entity->destroy = Player_destroyEntity;
    entity->onHealthModified = Player_onEntityHealthModified;

    SDL_Point tilePos = { 0, 0 };
    entity->physics.bounds.x = tilePos.x * TILE_W * PHYSICS_SCALE;
    entity->physics.bounds.y = tilePos.y * TILE_H * PHYSICS_SCALE;
    entity->physics.bounds.w = 15 * PHYSICS_SCALE; // Make sure Mr. Fatty get's though those slim trapdoors
    entity->physics.bounds.h = 15 * PHYSICS_SCALE;
    entity->physics.belongsToGroups = COLLISION_GROUP_PLAYER;
    entity->physics.collidesWithGroupMask = COLLISION_GROUP_TERRAIN | COLLISION_GROUP_COLLECTABLE | COLLISION_GROUP_ENEMY;
    entity->offset.x = -10;
    entity->offset.y = -17;

    Scene_addEntity(this->scene, entity);
    this->scene->camera->trackedEntity = entity;

    return entity;
}

void Player_processInput(Player* this) {
    cpFloat x = Input_getAxis(this->input, horizontal);
    if (x != 0) {
        cpFloat xSpeed = (x*this->controlledEntity.entity->speedMultiplier) / AXIS_MAX;
        cpBody* body = this->controlledEntity.entity->physics.body;
        //printf("%f of %f he %f movex\n", xSpeed, x, this->controlledEntity.entity->speedMultiplier);
        cpBodyApplyImpulseAtWorldPoint(body, cpv(xSpeed, 0), cpv(0,0));
        // shitty check to not flip the cursor
        if (x < 0) {
            this->controlledEntity.entity->animatedSprite->sprite->flip = true;
        } else  if (x > 0) {
            this->controlledEntity.entity->animatedSprite->sprite->flip = false;
        }
    }

    if (Input_isDown(this->input, attackSword) &&
        strcmp(this->controlledEntity.entity->animatedSprite->progress.animation->name,
               ANIMATION_PREPARE_ATTACK1) != 0 &&
        strcmp(this->controlledEntity.entity->animatedSprite->progress.animation->name,
               ANIMATION_ATTACK1) != 0) {

        AnimatedSprite_setAnimation(this->controlledEntity.entity->animatedSprite,
                                    ANIMATION_PREPARE_ATTACK1);
    }

    bool didJump = false;
    int y = -Input_getAxis(this->input, vertical);

    if (Input_isDown(this->input, jump) || y < 0) {
        didJump = Entity_jump(this->controlledEntity.entity);
    }

    if (!didJump && y != 0 && this->controlledEntity.entity->inFrontOfLadder) {
        this->controlledEntity.entity->physics.groundedStatus = onLadder;
        this->controlledEntity.entity->physics.dy += (y*this->controlledEntity.entity->speedMultiplier * PHYSICS_SCALE) / AXIS_MAX;
    }
}

void Player_drawEntity(void* context, SDL_Renderer* renderer, Camera* camera) {
    Player* this = context;
    Sprite_drawOnCamera(this->entity->animatedSprite->sprite, renderer, camera);
}

void Player_draw(void* context, SDL_Renderer* renderer) {
    /* Player* this = context; */ // TODO might draw ui here
}

void ControlledEntity_onEntityDestroyed(void* context) {
    ControlledEntity* this = context;
    Entity* nextEntity = _Player_onEntityDestroyed(this->player);

    Entity* oldEntity = this->entity;
    ControlledEntity_release(this);
    oldEntity->destroy(oldEntity->context);
    this->destroyOnRelease = false;
    ControlledEntity_set(this, nextEntity);
}

void ControlledEntity_release(ControlledEntity* this) {
    if (this->entity) {
        this->entity->destroy = this->originalDestroy;
        this->entity->draw = this->originalDraw;
        this->entity->context = this->originalContext;

        if (this->destroyOnRelease) {
            this->entity->destroyed = true;
            this->destroyOnRelease = false;
        }
        this->entity = NULL;
    }
}

void ControlledEntity_set(ControlledEntity* this, Entity* entity) {
    ControlledEntity_release(this);
    this->entity = entity;
    if (entity) {
        this->originalDestroy = entity->destroy;
        this->originalContext = entity->context;
        this->originalDraw = entity->draw;

        this->entity->draw = ControlledEntity_draw;
        this->entity->destroy = ControlledEntity_onEntityDestroyed;
        this->entity->context = this;
    }
}

void ControlledEntity_draw(void* context, SDL_Renderer* renderer, Camera* camera) {
    ControlledEntity* this = context;
    if (this->indicator) {
        this->indicator->bounds.y = this->entity->animatedSprite->sprite->bounds.y - this->indicator->bounds.h;
        this->indicator->bounds.x = this->entity->animatedSprite->sprite->bounds.x;
        Sprite_drawOnCamera(this->indicator, renderer, camera);
    }
    this->originalDraw(this->originalContext, renderer, camera);
}
