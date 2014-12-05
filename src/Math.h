#pragma once

#include <math.h>
#include <SDL2/SDL.h>

#ifndef M_PI
# define M_PI       3.14159265358979323846  /* pi */
#endif

#define Math_radiansToDegrees(rad) (rad*(180.0/M_PI))
#define Math_degreesToRadians(deg) (deg*(M_PI/180.0))

#define Math_angleFromOriginToPoint(origin, pos)                        \
    Math_radiansToDegrees(-atan2(origin.x - pos.x, origin.y - pos.y))

SDL_Point Math_pointOnCircle(SDL_Point origin, double angle, int radius);
