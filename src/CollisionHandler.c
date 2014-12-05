#include "CollisionHandler.h"
#include "Entity.h"

void Entity_Ground_separate(cpArbiter *arb, cpSpace *space, void *data) {
    cpShape *entityShape, *groundShape;
    cpArbiterGetShapes(arb, &entityShape, &groundShape);
    Entity* entity = (Entity*)cpShapeGetUserData(entityShape);
    if (entity->physics.onGround == groundShape) {
        entity->physics.onGround = NULL;
    }
    //printf("ungrounded!\n");
}

void Entity_Ground_postSolve(cpArbiter *arb, cpSpace *space, void *data) {
    cpShape *entityShape, *groundShape;
    cpArbiterGetShapes(arb, &entityShape, &groundShape);
    Entity* entity = (Entity*)cpShapeGetUserData(entityShape);
    entity->physics.onGround = groundShape;
    //printf("grounded!\n");
}

void CollisionHandler_register(cpSpace* space) {
    cpCollisionHandler* handler = cpSpaceAddCollisionHandler(space, COLLISION_TYPE_ENTITY_FEET, COLLISION_TYPE_GROUND);
    handler->postSolveFunc = Entity_Ground_postSolve;
    handler->separateFunc = Entity_Ground_separate;
}
