#include "Entity.h"
#include "Hitbox.h"
#include "Utils.h"
#include "Scene.h"
#include "Tile.h"
#include "GameOverModule.h"
#include "CollisionHandler.h"
#include <chipmunk/chipmunk.h>

Entity* Entity_create(void* context, Scene* scene, AnimatedSprite* sprite) {
    Entity* this = malloc(sizeof(Entity));
    this->scene = scene;
    this->animatedSprite = sprite;
    this->animatedSprite->context = this;
    this->animatedSprite->onAnimationEnd = Entity_AnimationEnded;
    this->destroyed = false;
    EntityPhysics_init(&this->physics, sprite ? sprite->sprite : NULL, this);
    this->update = emptyUpdate;
    this->draw = Entity_emptyDraw;
    this->destroy = emptyDestroy;
    this->onCollision = Entity_emptyOnCollision;
    this->onHealthModified = Entity_emptyOnHealthModified;
    this->context = context;
    this->speedMultiplier = 1.0 * (UPDATE_TIME / 1000.0);
    this->inFrontOfLadder = false;
    this->offset.x = 0;
    this->offset.y = 0;
    this->health = 100;
    this->maxHealth = 100;
    this->attack1 = 35;
    this->immuneToDamageTime = 0;
    this->maxImmuneToDamageTime = 1500;

    this->remainingJumps = 10;
    this->timeSinceGrounded = 0;

    return this;
}

void Entity_destroy(Entity* this) {
    this->destroy(this->context);

    cpBodyEachShape(this->physics.body, Chipmunk_removeEachShapeFromSpace, NULL);
    cpSpaceRemoveBody(cpBodyGetSpace(this->physics.body), this->physics.body);

    cpBodyFree(this->physics.body); // TODO may not destroy, but pass to others object on death
    free(this);
}

bool Entity_collides(Entity* this, Entity* other, SDL_Rect* newpos) {
    if ((this->physics.collidesWithGroupMask & other->physics.belongsToGroups) != 0 &&
        SDL_Rect_touches(newpos, &other->physics.bounds)) {
        int blocks = this->onCollision(this->context, other);
        blocks = other->onCollision(other->context, this) || blocks;

        if (strcmp(this->animatedSprite->progress.animation->name, ANIMATION_ATTACK1) == 0) {
            if (other->immuneToDamageTime <= 0) {
                Entity_hurt(other, -this->attack1);
            }
        }

        return blocks; //TODO
    }
    return false;
}


void EntityPhysics_init(EntityPhysics* this, Sprite* sprite, Entity* entity) {
    if (sprite) {
        this->bounds.x = 0;
        this->bounds.y = 0;
        this->bounds.w = PIXEL_TO_PHYSICS(sprite->bounds.w);
        this->bounds.h = PIXEL_TO_PHYSICS(sprite->bounds.h);
    } else {
        SDL_Rect_init(&this->bounds);
    }

    this->body = NULL;
    this->shapes = Vector_Create();

    cpFloat mass = 1;
    // TODO store and apply moment for ragdolling
    /* cpFloat moment = cpMomentForBox(mass, this->bounds.w, this->bounds.h); */

    this->body = cpBodyNew(mass, INFINITY);

    cpFloat rotation = 0;
    cpShape* boxShape = cpBoxShapeNew(this->body, this->bounds.w, this->bounds.h, rotation);
    cpShape* circle = cpCircleShapeNew(this->body, this->bounds.w/2, cpv(0, this->bounds.h/2));

    cpShapeSetFriction(boxShape, 0.7);
    cpShapeSetFriction(circle, 0.7);
    cpShapeSetCollisionType(boxShape, COLLISION_TYPE_ENTITY_MAIN);
    cpShapeSetCollisionType(circle, COLLISION_TYPE_ENTITY_FEET);
    cpShapeSetUserData(boxShape, entity);
    cpShapeSetUserData(circle, entity);
    Vector_InsertInFirstFreeSpace(this->shapes, boxShape);
    Vector_InsertInFirstFreeSpace(this->shapes, circle);

    this->dx = 0;
    this->dy = 0;
    this->collidesWithGroupMask = COLLISION_GROUP_TERRAIN;
    this->belongsToGroups = COLLISION_GROUP_NONE;
    this->groundedStatus = inAir;
    this->onGround = NULL;
}

void Entity_update(Entity* this, RawTime dt) {
    this->update(this->context, dt);

    if (this->immuneToDamageTime > 0) {
        this->immuneToDamageTime -= dt;
    }

    /* SDL_Rect newPosition = this->physics.bounds; */
    /* newPosition.x += this->physics.dx; */
    /* if (!Entity_wouldCollide(this, &newPosition)) { */
    /*     this->physics.bounds.x += this->physics.dx; */
    /* } */
    /* newPosition.x -= this->physics.dx; */
    /* newPosition.y += this->physics.dy; */
    /* if (!Entity_wouldCollide(this, &newPosition)) { */
    /*     this->physics.bounds.y += this->physics.dy; */
    /*     if (this->physics.groundedStatus == grounded || (this->physics.groundedStatus == onLadder && !this->inFrontOfLadder)) { */
    /*         this->physics.groundedStatus = inAir; */
    /*     } */
    /* } */
    /* this->physics.dx = this->physics.dy = 0; */

    if (this->animatedSprite) {
        AnimationProgress_update(&this->animatedSprite->progress, dt);

        cpVect pos = cpBodyGetPosition(this->physics.body);
        this->animatedSprite->sprite->bounds.x = this->offset.x + PHYSICS_TO_PIXEL(pos.x);
        this->animatedSprite->sprite->bounds.y = this->offset.y + PHYSICS_TO_PIXEL(pos.y);
    }

    if (this->physics.onGround != NULL) {
        this->timeSinceGrounded += dt;
    } else {
        this->timeSinceGrounded = 0;
    }
}

bool Entity_jump(Entity* this) {
    if (this->physics.groundedStatus == onLadder ||
        (this->physics.onGround != NULL && this->timeSinceGrounded > 120)) {
        this->remainingJumps = 10;
    }
    if (this->remainingJumps > 0) {
        if (this->physics.groundedStatus == onLadder) {
            //this->physics.dy -= (32*5) / PHYSICS_SCALE;
        } else {
            cpBodyApplyImpulseAtWorldPoint(this->physics.body, cpv(0, -PIXEL_TO_PHYSICS(1.0/10.0)), cpv(0,0));
            //this->physics.dy -= (32*10) / PHYSICS_SCALE;
        }
        this->remainingJumps--;
        return true;
    }
    return false;
}

bool Entity_wouldCollide(Entity* this, SDL_Rect *rect) {
    // test scene
    if (rect->x < this->scene->walkableBounds.x ||
        rect->x + rect->w > this->scene->walkableBounds.x + this->scene->walkableBounds.w) {
        return true;
    }
    if (rect->y + rect->h < this->scene->walkableBounds.y) {
        if ((this->physics.belongsToGroups & COLLISION_GROUP_PLAYER)) {
            GameOverModule* gm = GameOverModule_create(this->scene->engine);
            gm->winnerName = "red"; // lol
            this->scene->engine->nextModule = gm->module;
        }

        return true;
    }

    if (rect->y + rect->h > this->scene->walkableBounds.y + this->scene->walkableBounds.h) {
        if (this->physics.groundedStatus != immuneToGravity) {
            this->physics.groundedStatus = grounded;
        }
        return true;
    }

    // test entities
    if (this->physics.collidesWithGroupMask & (COLLISION_GROUP_ENEMY | COLLISION_GROUP_COLLECTABLE | COLLISION_GROUP_PLAYER)) {
        for (int i=0; i < this->scene->entities->usedElements; ++i) {
            Entity* it = this->scene->entities->elements[i];
            if (it != NULL) {

                for (int j=0; j < this->scene->entities->usedElements; ++j) {
                    Entity* jt = this->scene->entities->elements[j];
                    if (jt != NULL && jt != it && Entity_collides(it, jt, rect)) {
                        return true;
                    }
                }
            }
        }
    }

    this->inFrontOfLadder = false;
    return false;
}

void Entity_AnimationEnded(void* context) {
    Entity* this = context;
    if (strcmp(this->animatedSprite->progress.animation->name, ANIMATION_PREPARE_ATTACK1) == 0) {
        AnimatedSprite_setAnimation(this->animatedSprite, ANIMATION_ATTACK1);
    } else if (strcmp(this->animatedSprite->progress.animation->name, ANIMATION_ATTACK1) == 0) {
        AnimatedSprite_setAnimation(this->animatedSprite, ANIMATION_IDLE);
    } else {
        AnimatedSprite_setAnimation(this->animatedSprite, ANIMATION_IDLE);
    }
}

void EntityPhysics_destroy(EntityPhysics* this) {
    free(this);
}

void Entity_hurt(Entity* this, const int health) {
    if (health < 0) {
        this->immuneToDamageTime = this->maxImmuneToDamageTime;
    }
    Entity_modifyHealth(this, health);
}

void Entity_modifyHealth(Entity* this, const int health) {
    this->health += health;
    this->health = imin(this->health, 100);
    if (this->health <= 0) {
        this->destroyed = true;
    }
    this->onHealthModified(this->context);
}

void Entity_emptyDraw(void* context, SDL_Renderer* renderer, Camera* camera) {}
bool Entity_emptyOnCollision(void* context, Entity* otherEntity) { return false; }
void Entity_emptyOnHealthModified(void* context) {}
