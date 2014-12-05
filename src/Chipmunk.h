#pragma once

#include <chipmunk/chipmunk.h>

void Chipmunk_removeEachShapeFromSpace(cpBody* body, cpShape* shape, void* data);
void Chipmunk_addEachShapeToSpace(cpBody* body, cpShape* shape, void* data);
