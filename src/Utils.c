#include "Utils.h"

void emptyInit(void* context) {}
void emptyUpdate(void* context, RawTime dt) {}
void emptyDraw(void* context, SDL_Renderer* surface) {}
void emptyResize(void* context, SDL_Point size) {}
void emptyDestroy(void* context) {}
void emptyHandleEvent(void* context, SDL_Event*event) {}

SCM Utils_evaluateSchemeFile(const char* path) {
    withFileContent(content, path, {
        if (NULL == content) { return SCM_BOOL_F; }
        SCM code = scm_from_locale_string(content);
        return scm_eval_string(code);
    });
}

void fwriteSafe(const void *ptr, const size_t size, const size_t nmemb, FILE *stream, bool* success) {
    if (false == *success) { return; }
    *success = nmemb == fwrite(ptr, size, nmemb, stream);
}

char* Utils_readFileContent(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        return NULL;
    }
    fseek (f, 0, SEEK_END);
    size_t length = ftell (f);
    fseek (f, 0, SEEK_SET);
    char* content = malloc(length);
    if (content) {
        fread(content, 1, length, f);
    }
    fclose (f);
    return content;
}

bool SDL_Rect_touches(SDL_Rect* a, SDL_Rect* b) {
    return !(a->x      > b->x+b->w ||
              a->x+a->w < b->x      ||
              a->y      > b->y+b->h ||
              a->y+a->h < b->y);
}

bool SDL_Rect_above(SDL_Rect* a, SDL_Rect* b) {
    return a->y + a->h > b->y;
}

bool SDL_Rect_isInside(const SDL_Rect* a, int x, int y) {
    return x > a->x && x < a->x + a->w &&
            y > a->y && y < a->y + a->h;
}

void SDL_Rect_init(SDL_Rect* rect) {
    rect->x = 0;
    rect->y = 0;
    rect->w = 0;
    rect->h = 0;
}
