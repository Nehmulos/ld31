#pragma once

#include "Constants.h"
#include "Camera.h"

struct ChipmunkDebugDrawData {
    SDL_Renderer* renderer;
    const Camera* camera;
};

cpSpaceDebugDrawOptions ChipmunkDebugDrawSDL2_make(struct ChipmunkDebugDrawData* data);

void ChipmunkDebugDrawSDL2_drawCircle(cpVect pos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer data);
void ChipmunkDebugDrawSDL2_drawSegment(cpVect a, cpVect b, cpSpaceDebugColor color, cpDataPointer data);
void ChipmunkDebugDrawSDL2_drawFatSegment(cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer data);
void ChipmunkDebugDrawSDL2_drawPolygon(int count, const cpVect *verts, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer data);
void ChipmunkDebugDrawSDL2_drawDot(cpFloat size, cpVect pos, cpSpaceDebugColor color, cpDataPointer data);
cpSpaceDebugColor ChipmunkDebugDrawSDL2_colorForShape(cpShape *shape, cpDataPointer data);
