#include "Chipmunk.h"

void Chipmunk_removeEachShapeFromSpace(cpBody* body, cpShape* shape, void* data) {
    cpSpace* space = data ? data : cpBodyGetSpace(body);
    cpSpaceRemoveShape(space, shape);
}

void Chipmunk_addEachShapeToSpace(cpBody* body, cpShape* shape, void* data) {
    cpSpace* space = data ? data : cpBodyGetSpace(body);
    cpSpaceAddShape(space, shape);
}
