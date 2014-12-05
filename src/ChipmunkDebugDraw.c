#include "ChipmunkDebugDraw.h"
#include "Camera.h"
#include "Math.h"

cpSpaceDebugDrawOptions ChipmunkDebugDrawSDL2_make(struct ChipmunkDebugDrawData* data) {
    enum cpSpaceDebugDrawFlags flags =
        CP_SPACE_DEBUG_DRAW_SHAPES |
        CP_SPACE_DEBUG_DRAW_CONSTRAINTS |
        CP_SPACE_DEBUG_DRAW_COLLISION_POINTS;

    cpSpaceDebugDrawOptions options = {
        .drawCircle = ChipmunkDebugDrawSDL2_drawCircle,
        .drawSegment = ChipmunkDebugDrawSDL2_drawSegment,
        .drawFatSegment = ChipmunkDebugDrawSDL2_drawFatSegment,
        .drawPolygon = ChipmunkDebugDrawSDL2_drawPolygon,
        .drawDot = ChipmunkDebugDrawSDL2_drawDot,
        .flags = flags,
        .shapeOutlineColor = {240, 30, 100, 0.7},
        .colorForShape = ChipmunkDebugDrawSDL2_colorForShape,
        .constraintColor = {100, 30, 200, 0.7},
        .collisionPointColor = {100, 200, 30, 0.7},
        .data = data,
    };
    return options;
}


void ChipmunkDebugDrawSDL2_drawCircle(cpVect rawPos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer dataPointer) {
    struct ChipmunkDebugDrawData* data  = (struct ChipmunkDebugDrawData*)dataPointer;
    SDL_SetRenderDrawColor(data->renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);

    SDL_SetRenderDrawColor(data->renderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);

    const cpVect pos = Camera_translatePhysics(data->camera, rawPos);
    const SDL_Point center = {pos.x, pos.y};
    const SDL_Point segmentStart = Math_pointOnCircle(center, 0, radius);
    const int segments = 14;
    SDL_Point last = segmentStart;
    for (int i=1; i < segments+1; ++i) {
        const float segmentAngle = ((double)i/(double)segments) * 360.0;
        const SDL_Point current = Math_pointOnCircle(center, segmentAngle, radius);
        SDL_RenderDrawLine(data->renderer, last.x, last.y, current.x, current.y);
        last = current;
    }

    const SDL_Point angleSegment = Math_pointOnCircle(center, angle, radius/2);
    SDL_RenderDrawLine(data->renderer, center.x, center.y, angleSegment.x, angleSegment.y);
    //printf("ChipmunkDebugDrawSDL2\n");
}

void ChipmunkDebugDrawSDL2_drawSegment(cpVect aRaw, cpVect bRaw, cpSpaceDebugColor color, cpDataPointer dataPointer) {
    struct ChipmunkDebugDrawData* data  = (struct ChipmunkDebugDrawData*)dataPointer;

    const cpVect a = Camera_translatePhysics(data->camera, aRaw);
    const cpVect b = Camera_translatePhysics(data->camera, bRaw);
    SDL_SetRenderDrawColor(data->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(data->renderer, a.x, a.y, b.x, b.y);
    //printf("ChipmunkDebugDrawSDL2\n");
}

void ChipmunkDebugDrawSDL2_drawFatSegment(cpVect aRaw, cpVect bRaw, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer dataPointer) {
    struct ChipmunkDebugDrawData* data  = (struct ChipmunkDebugDrawData*)dataPointer;

    SDL_SetRenderDrawColor(data->renderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);

    const cpVect a = Camera_translatePhysics(data->camera, aRaw);
    const cpVect b = Camera_translatePhysics(data->camera, bRaw);

    SDL_RenderDrawLine(data->renderer, a.x, a.y, b.x, b.y);

    SDL_SetRenderDrawColor(data->renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    for (int i=0; i < radius/2; ++i) {
        SDL_RenderDrawLine(data->renderer, a.x+i, a.y+i, b.x+i, b.y+i);
        SDL_RenderDrawLine(data->renderer, a.x-i, a.y-i, b.x-i, b.y-i); // TODO shitty hack calculate it correctly
    }
    //printf("ChipmunkDebugDrawSDL2\n");
}
void ChipmunkDebugDrawSDL2_drawPolygon(int count, const cpVect *verts, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer dataPointer) {
    struct ChipmunkDebugDrawData* data  = (struct ChipmunkDebugDrawData*)dataPointer;
    SDL_SetRenderDrawColor(data->renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    // TODO must fill
    SDL_SetRenderDrawColor(data->renderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
    const cpVect* last = NULL;
    const cpVect* first = NULL;
    for (int i=0; i < count; ++i) {
        const cpVect* current = &(verts[i]);
        if (last) {
            // TODO angle
            const cpVect a = Camera_translatePhysics(data->camera, *last);
            const cpVect b = Camera_translatePhysics(data->camera, *current);
            SDL_RenderDrawLine(data->renderer, a.x, a.y, b.x, b.y);
        }
        if (!first) {
            first = current;
        }
        last = current;
    }

    if (last && first) {
        const cpVect a = Camera_translatePhysics(data->camera, *last);
        const cpVect b = Camera_translatePhysics(data->camera, *first);
        SDL_RenderDrawLine(data->renderer, a.x, a.y, b.x, b.y);
    }


    //printf("count %d", count);
    if (last) {
        //printf("last %f,%f", last->x, last->y);
    }
    //printf("\n");
    //printf("ChipmunkDebugDrawSDL2\n");
}
void ChipmunkDebugDrawSDL2_drawDot(cpFloat size, cpVect pos, cpSpaceDebugColor color, cpDataPointer dataPointer) {
    struct ChipmunkDebugDrawData* data  = (struct ChipmunkDebugDrawData*)dataPointer;
    SDL_SetRenderDrawColor(data->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(data->renderer, pos.x, pos.y); // TODO size
    //printf("ChipmunkDebugDrawSDL2\n");
}
cpSpaceDebugColor ChipmunkDebugDrawSDL2_colorForShape(cpShape *shape, cpDataPointer dataPointer) {
    //struct ChipmunkDebugDrawData* data  = (struct ChipmunkDebugDrawData*)dataPointer;
    cpSpaceDebugColor color = {
        .r = 200,
        .g = 40,
        .b = 120,
        .a = 0.7
    };
    //printf("ChipmunkDebugDrawSDL2\n");
    return color;
}
