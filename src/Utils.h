#pragma once

#include "Constants.h"
#include <stdio.h>

// SDL utils
void SDL_Rect_init(SDL_Rect* rect);
bool SDL_Rect_touches(SDL_Rect* a, SDL_Rect* b);
bool SDL_Rect_above(SDL_Rect* a, SDL_Rect* b);
bool SDL_Rect_isInside(const SDL_Rect* a, int x, int y);

SCM Utils_evaluateSchemeFile(const char* path);

char* Utils_readFileContent(const char* path);

void fwriteSafe(const void *ptr, size_t size, size_t nmemb, FILE *stream, bool* success);

// useful to check not null and then chain something after a struct pointer
// example:
// nnIf(pointer, ->value = 9;);
#define nnIf(caseExpr, expression) \
    if (caseExpr) caseExpr expression


#define withFileContent(symbolName, path, block)                \
    withAllocOfType(char*, symbolName, Utils_readFileContent(path), block)


// check for NULL before freeing
#define safeFree(sign)                          \
    if (NULL != sign) { free(sign); }

// does your stuff + NULL checked free
#define withAllocOfType(type, sign, allocationExpression, block)    \
    { type sign = allocationExpression; block; safeFree(sign); }


#define anyArgs(...) __VA_ARGS__
// allocate symbol & set it to sprintf, to use in block, free it after block
#define withSprintf(symbol, formatStr, formatArgs, block)    \
    {                                                        \
        char* symbol = NULL;                                 \
        asprintf(&symbol,                                    \
                 formatStr,                                  \
                 formatArgs);                                \
        block;                                               \
        free(symbol);                                        \
    }

#define allocType(type) \
    malloc(sizeof(type))

#define allocTypeN(type, n) \
    calloc(n, sizeof(type))

#define allocStruct(type) \
    malloc(sizeof(struct type))

#define allocStructN(type, n) \
    calloc(n, sizeof(struct type))

#define VECTOR_EACH(vector, symbolName, block) \
    for (int _i=0; _i < vector->usedElements; ++_i) { \
        void* symbolName = vector->elements[_i]; \
        block; \
    }


#define FREE_VECTOR_WITH_ELMENTS(vector, destructor) \
    for (int _i=0; _i < vector->usedElements; ++_i) { \
        void* it = vector->elements[_i]; \
        if (it != NULL) { \
            destructor(it); \
        } \
    } \
    Vector_Destroy(vector);

#define FREE_LIST_WITH_ELMENTS(list, destructor) \
    { \
        ListNode* util_freeingNode = list->first; \
        while (util_freeingNode) { \
            void* util_data = util_freeingNode->data; \
            if (util_data != NULL) { \
                destructor(util_data); \
            } \
            util_freeingNode = util_freeingNode->next; \
        } \
        List_destroy(list); \
    }

#define DEFAULT_CREATE_DESTROY_H(prefix)        \
    struct prefix * prefix ## _create();        \
    void prefix ## _destroy(struct prefix * this);

#define DEFAULT_CREATE_DESTROY(prefix)          \
    struct prefix * prefix ## _create() {       \
    const int size = sizeof(struct prefix);     \
    struct prefix * newValue = malloc(size);    \
    prefix ## _init(newValue);          \
    return newValue;                \
    }                           \
    void prefix ## _destroy(struct prefix * this) { \
    prefix ## _destroyMembers(this);        \
    free(this);                 \
    }

#define STRING_TO_ENUM(e, s) if (strcmp(STR(e), s) == 0) { return e; }
#define ENUM_TO_STRING(e, v) if (e == v) { return STR(e); }

void emptyInit(void* context);
void emptyUpdate(void* context, RawTime dt);
void emptyDraw(void* context, SDL_Renderer* surface);
void emptyResize(void* context, SDL_Point size);
void emptyDestroy(void* context);
void emptyHandleEvent(void* context, SDL_Event*);
