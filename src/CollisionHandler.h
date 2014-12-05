#pragma once
#include "Constants.h"

#define COLLISION_TYPE_GROUND 1

#define COLLISION_TYPE_ENTITY_MAIN 11
#define COLLISION_TYPE_ENTITY_FEET 12

void CollisionHandler_register(cpSpace* space);
