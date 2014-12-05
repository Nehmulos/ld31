#include "Sky.h"
#include "Utils.h"
#include "Math.h"

struct Sky* Sky_create(struct TextureCache* textureCache) {
    struct Sky* this = allocType(struct Sky);
    this->moon = Sprite_create(TextureCache_get(textureCache, "images/moon.png"));
    this->sun = Sprite_create(TextureCache_get(textureCache, "images/sun.png"));
    this->clouds = Vector_Create();
    this->stars = Vector_Create();
    this->fixStars = Vector_Create();

    this->windSpeedX = 0;

    SDL_Rect_init(&this->bounds);
    this->bounds.w = YOM_WINDOW_W; //2024;
    this->bounds.h = YOM_WINDOW_H;//400;

    this->scrollY = 0.00;
    this->moon->scrollY = this->scrollY;
    this->moon->scrollX = this->scrollY;
    this->sun->scrollY = this->scrollY;
    this->sun->scrollX = this->scrollY;


    this->moon->bounds.w = 128;
    this->moon->bounds.h = 128;

    struct SkyColor morning = {.top = {155, 218, 231, 1}, .bottom = {68, 209, 214, 1}};
    struct SkyColor midday = {.top = {175, 238, 231, 1}, .bottom = {88, 229, 214, 1}};
    struct SkyColor evening = {.top = {240, 213, 104, 1}, .bottom = {238, 81, 60, 1}};
    struct SkyColor night = {.top = {0, 9, 77, 1}, .bottom = {10, 20, 95, 1}};

    struct SkyColors colors = {
        .lateNight =    { .range = {0,4}, .currentColor = night, .nextColor = night},
        .earlyMorning = { .range = {4,7}, .currentColor = night, .nextColor = morning},
        .morning =      { .range = {7,12}, .currentColor = morning, .nextColor = midday},
        .midday =       { .range = {12,16}, .currentColor = midday, .nextColor = midday},
        .earlyEvening = { .range = {16,20}, .currentColor = midday, .nextColor = evening},
        .evening =      { .range = {20,22}, .currentColor = evening, .nextColor = night},
        .night =        { .range = {22,24}, .currentColor = night, .nextColor = night}
    };
    this->colors = colors;

    return this;
}

double p = 0;

SDL_Color Sky_gradientTransition(const SDL_Color top, const SDL_Color bot, double progress) {
    const short diff[3] = {bot.r - top.r, bot.g - top.g, bot.b - top.b};
    const SDL_Color color = {
        top.r + ((double)diff[0]*progress),
        top.g + ((double)diff[1]*progress),
        top.b + ((double)diff[2]*progress),
        1
    };
    return color;
}



void Sky_drawGradient(struct Sky* this, SDL_Renderer* renderer, Camera* camera) {
    double hour = 24 * p;

    struct SkyColorTime colorTime;


#   define checkRange(_ignore, name)                                         \
    else if (hour >= this->colors. name .range[0] && hour < this->colors. name .range[1]) { \
        colorTime =  this->colors. name ;                               \
        /* double timeProgress = (hour - colorTime.range[0]) / (colorTime.range[1] - colorTime.range[0]); */ \
        /*printf("%05.2fh that's %02d%% of %s\n", hour, (int)(timeProgress*100.0), #name);*/ \
    }

    if (false) {} // shitty else if starter
    SkyColors_DATA(checkRange)
    else {
        struct SkyColor night = {.top = {0, 9, 77, 1}, .bottom = {10, 20, 95, 1}};
        struct SkyColorTime fuckCforBeeingSoVerbous = { .range = {0,0}, .currentColor = night, .nextColor = night};
        colorTime = fuckCforBeeingSoVerbous;
        //printf("%f of %f %s\n", hour, p, "error:, no time-range for sky");
    }



    int yFinal = this->bounds.y + this->bounds.h;
    double timeProgress = (hour - colorTime.range[0]) / (colorTime.range[1] - colorTime.range[0]);
    SDL_Color top = Sky_gradientTransition(colorTime.currentColor.top, colorTime.nextColor.top, timeProgress);
    SDL_Color bottom = Sky_gradientTransition(colorTime.currentColor.bottom, colorTime.nextColor.bottom, timeProgress);

    for (int y=this->bounds.y; y < yFinal; ++y) {
        double progress = (double)y / ((double)abs(this->bounds.y - yFinal));
        const SDL_Color color = Sky_gradientTransition(top, bottom, progress);
        const int drawY = y - ((double)camera->viewport.y * this->scrollY);
        const int drawX = this->bounds.x - ((double)camera->viewport.x * this->scrollY);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLine(renderer, drawX, drawY, drawX + this->bounds.w, drawY);

    }
}

void Sky_draw(struct Sky* this, SDL_Renderer* renderer, Camera* camera) {
    Sky_drawGradient(this, renderer, camera);
    Sprite_drawOnCamera(this->sun, renderer, camera);
    Sprite_drawOnCamera(this->moon, renderer, camera);
    VECTOR_EACH(this->clouds, cloud, {
            Sprite_drawOnCamera(cloud, renderer, camera);
    });
}

void Sky_updateForDateTime(struct Sky* this /*, struct tm dateTime */) {
    double hour = 24.0 * p;
    int sunTime[2] = {5,21};

    double moonAngle = 0;
    double sunAngle = 0;

    if (hour >= sunTime[0] && hour < sunTime[1]) {
        double sunProgress = (hour - (double)sunTime[0]) / (double)(sunTime[1]-sunTime[0]);
        sunAngle = sunProgress * 180;
    } else {
        const int moonDuration = 24 - sunTime[1] + sunTime[0];
        const double moonProgressH = hour < sunTime[0]
            ? 24 - sunTime[1] + hour
            : hour - sunTime[1];
        const double moonProgress = moonProgressH / moonDuration;
        moonAngle = moonProgress * 180;
    }

    int angle = ((360.0*p));
    angle = angle > 360 ? angle % 360 : angle;

    SDL_Point moonPos = Sky_celestialPositionForAngle(this->bounds, moonAngle + 180.0);
    SDL_Point sunPos = Sky_celestialPositionForAngle(this->bounds, sunAngle + 180.0);
    this->moon->bounds.x = moonPos.x - this->moon->bounds.w/2;
    this->moon->bounds.y = moonPos.y;

    this->sun->bounds.x = sunPos.x - this->sun->bounds.w/2;
    this->sun->bounds.y = sunPos.y;

    //lprintf("%d,%d on %d %f \n", moonPos.x, moonPos.y, angle, p);

    p += 0.0002;
    if (p >= 1) {
        p = 0;
    }
}

SDL_Point Sky_celestialPositionForAngle(SDL_Rect bounds, double angle) {
    SDL_Point center = {.x = bounds.x, .y = bounds.y + bounds.h};
    SDL_Point circlePoisition = Math_pointOnCircle(center, angle, bounds.h/2);
    double xAspect = (double)bounds.w / (double)bounds.h;
    SDL_Point point = {
        .x = (double)(circlePoisition.x *xAspect) +(double)(bounds.w/2),
        .y = (double)(circlePoisition.y *2) - (bounds.h)
    };
    return point;
}
