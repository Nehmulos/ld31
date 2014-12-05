#include "Camera.h"
#include "Entity.h"

Camera* Camera_create(int w, int h) {
    Camera* this = malloc(sizeof(Camera));
    this->trackedEntity = NULL;
    this->viewport.x = 0;
    this->viewport.y = 0;
    this->viewport.w = w;
    this->viewport.h = h;

    this->bounds.x = 0;
    this->bounds.y = 0;
    this->bounds.w = 0;
    this->bounds.h = 0;

    this->translation.x = 0;
    this->translation.y = 0;
    return this;
}

void Camera_destroy(Camera* this) {
    free(this);
}

void Camera_update(Camera* this) {
    if (this->trackedEntity) {
//      SDL_Rect* entityBounds = &((Entity*)this->trackedEntity)->physics.bounds;
        SDL_Rect* entityBounds = &((Entity*)this->trackedEntity)->animatedSprite->sprite->bounds;
        Camera_setTo(this, entityBounds->x+entityBounds->w/2 - this->viewport.w/2,
                           entityBounds->y+entityBounds->h/2 - this->viewport.h/2);
    }
}

bool Camera_setTo(Camera* this, int newX, int newY) {
    bool moved = false;
    bool hasBoundsX = this->bounds.w != 0;
    bool hasBoundsY = this->bounds.h != 0;
    if (!hasBoundsX ||
        (newX != this->bounds.x &&
         newX > this->bounds.x &&
         newX + this->viewport.w < this->bounds.x + this->bounds.w)) {

        this->viewport.x = newX;
        moved = true;
    }
    if (!hasBoundsY ||
        (newY != this->bounds.y &&
         newY > this->bounds.y &&
         newY + this->viewport.h < this->bounds.y + this->bounds.h)) {

        this->viewport.y = newY;
        moved = true;
    }
    return moved;
}

bool Camera_moveBy(Camera* this, int dx, int dy) {
    int newX = this->viewport.x + dx;
    int newY = this->viewport.y + dy;
    return Camera_setTo(this, newX, newY);
}


cpVect Camera_translatePhysics(const Camera* this, const cpVect v) {
    if (!this) {
        return cpv(PHYSICS_TO_PIXEL(v.x), PHYSICS_TO_PIXEL(v.y));
    }
    // what about scrollX / scrollY?
    return cpv((PHYSICS_TO_PIXEL(v.x) + this->translation.x) - this->viewport.x,
               (PHYSICS_TO_PIXEL(v.y) + this->translation.y) - this->viewport.y);
}

SDL_Point Camera_translateViewport(const Camera* this, const SDL_Point v) {
    SDL_Point point = {
        .x = v.x - this->viewport.x,
        .y = v.y - this->viewport.y
    };
    return point;
}

SDL_Point Camera_translateWindow(const Camera* this, SDL_Renderer* renderer, SDL_Window* window, const SDL_Point v) {
    SDL_Point point = {
        .x = v.x + this->viewport.x,
        .y = v.y + this->viewport.y
    };
    return point;
    /*
    struct SDL_Rect rect;
    int windowW;
    int windowH;
    SDL_GetWindowSize(window, &windowW, &windowH);
    SDL_RenderGetViewport(renderer, &rect);

    const SDL_Rect letterbox = {
        .x = (rect.x/2),
        .y = (rect.y/2),
        .w = (windowW - rect.x),
        .h = (windowH - rect.y)
    };
    if (SDL_Rect_isInside(&letterbox, v.x, v.y)) {
        const double unscaledX = (double)v.x - ((double)rect.x/2.0);
        const double unscaledY = (double)v.y - ((double)rect.y/2.0);
        const double scaleX = (double)rect.w / ((double)letterbox.w);
        const double scaleY = (double)rect.h / ((double)letterbox.h);

        SDL_Point point = {
            .x = (unscaledX * 1) - this->viewport.x,
            .y = (unscaledY * 1) + this->viewport.y
        };
        printf("%d %d %d %d - ", rect.x, rect.y, rect.w, rect.h);
        printf("%d %d - ", windowW, windowH);
        //printf("%f %f - %f %f - ", unscaledX, scaleX, unscaledY, scaleY);
        printf("%d %d | %d %d\n", rect.x, rect.y, rect.w, rect.h);
        return point;
    }
*/

    // TODO return Maybe SDL_Point
}
