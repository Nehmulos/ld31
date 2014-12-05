#include "Math.h"

SDL_Point Math_pointOnCircle(SDL_Point origin, double angle, int radius) {
    double rad = Math_degreesToRadians(angle);
    SDL_Point point = {
        .x = origin.x + (radius * cos(rad)),
        .y = origin.y + (radius * sin(rad)),
    };
    return point;
}
