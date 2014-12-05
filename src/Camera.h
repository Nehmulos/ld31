#pragma once
#include "Constants.h"


typedef struct Camera {
    SDL_Rect viewport;
    SDL_Rect bounds;
    SDL_Point translation;
    void* trackedEntity; // TODO get proper forwarddeclaration
} Camera;

Camera* Camera_create(int w, int h);
void Camera_destroy(Camera* this);
void Camera_update(Camera* this);

bool Camera_setTo(Camera* this, int newX, int newY);
bool Camera_moveBy(Camera* this, int dx, int dy);

cpVect Camera_translatePhysics(const Camera* this, const cpVect v);
SDL_Point Camera_translateViewport(const Camera* this, const SDL_Point v);
SDL_Point Camera_translateWindow(const Camera* this, SDL_Renderer* renderer, SDL_Window* window, const SDL_Point v);
