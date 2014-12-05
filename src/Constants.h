/*
 * Constants.h
 *
 *  Created on: 24.12.2012
 *      Author: nehmulos
 */

#pragma once
// for gnu extensions
#define _GNU_SOURCE


#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>

#include <guile/2.0/libguile.h>

#include "Chipmunk.h"


typedef Uint32 RawTime; // in ms (1s == 1000)

#define UPDATES_PER_SECOND 60.0
#define UPDATE_TIME 1000.0 / UPDATES_PER_SECOND
#define MAX_UPDATE_TIME UPDATE_TIME * UPDATES_PER_SECOND * 5.0

#define PHYSICS_SCALE 1.0
#define PIXEL_TO_PHYSICS(value) (value) / PHYSICS_SCALE
#define PHYSICS_TO_PIXEL(value) (value) * PHYSICS_SCALE

#define PLAYER_COUNT 1

#define YOM_WINDOW_W 1920.0
#define YOM_WINDOW_H 1080.0

#define SCENE_WIDTH YOM_WINDOW_W
#define SCENE_HEIGHT YOM_WINDOW_H
#define SCENE_SPACER_WIDTH 32
#define SCENE_SPACER_HEIGHT 480

#define TILE_W 32
#define TILE_H 32
#define SCENE_TILES_X 12
#define SCENE_TILES_Y 15

#define DEFAULT_BPP 24
#define DEFAULT_WINDOW_FLAGS SDL_HWSURFACE|SDL_DOUBLEBUF


#define CANCEL_IF_FALSE(value) if (!value) { return; }
#define CLASS(name) typedef struct name name; struct name

// Thanks to Lindydancer for providing prepocess var to string converter code
// http://stackoverflow.com/a/5459929
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

static inline int imax(int a, int b) {
    return a > b ? a : b;
}
static inline int imin(int a, int b) {
    return a < b ? a : b;
}

#define inRange(a, min, max) (a > min && a < max)
#define inRangeOrEqual(a, min, max) (a >= min && a <= max)
