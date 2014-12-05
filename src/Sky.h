#pragma once

#include "Constants.h"
#include "Sprite.h"
#include "Vector.h"
#include "TextureCache.h"
#include "DefinitionMacros.h"
#include <time.h>


struct SkyColor {
    SDL_Color top;
    SDL_Color bottom;
};

struct SkyColorTime {
    int range[2];
    struct SkyColor currentColor;
    struct SkyColor nextColor;
};

#define SkyColors_DATA(F)                       \
    F(struct SkyColorTime, lateNight)           \
    F(struct SkyColorTime, earlyMorning)        \
    F(struct SkyColorTime, morning)             \
    F(struct SkyColorTime, midday)              \
    F(struct SkyColorTime, earlyEvening)        \
    F(struct SkyColorTime, evening)             \
    F(struct SkyColorTime, night)


struct SkyColors {
    SkyColors_DATA(defineStructField)
};

struct Sky {
    Sprite* sun;
    Sprite* moon;
    Vector* clouds;
    Vector* stars;
    Vector* fixStars;

    cpFloat windSpeedX;
    double scrollY;

    SDL_Rect bounds;
    SDL_Color colorTop;
    SDL_Color colorBottom;
    struct SkyColors colors;
};

struct Sky* Sky_create();

void Sky_updateForDateTime(struct Sky* this /* , struct tm dateTime */);
SDL_Point Sky_celestialPositionForAngle(SDL_Rect bounds, double angle);
void Sky_draw(struct Sky* this, SDL_Renderer* renderer, Camera* camera);
