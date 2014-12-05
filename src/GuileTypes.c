#include "GuileTypes.h"

#include "Scene.h"

void* GuileTypes_guile_registerAll(void* data) {
    Scene_guile_registerType();
    return NULL;
}

void GuileTypes_registerAll() {
    scm_with_guile(GuileTypes_guile_registerAll, NULL);
}
