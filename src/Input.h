#pragma once
#include "Constants.h"
#include "Vector.h"

#define AXIS_MIN_TRIGGER 260
#define AXIS_MAX 32767.0

// TODO somehow differentiate the player. Store hotkeys in Player.c or something
typedef enum ActionId {
    none,
    // player-entity
    horizontal, vertical, jump, attackSword, attackBow, action,
    // menu
    openMenu,
    // editor
    reloadScene, editorPlace, editorTerrain, editorObjects,
    editorToggleStartSimulation, editorStopSimulation,
    editorSave
} ActionId;

CLASS(KeyHotkey) {
    SDL_Keysym key;
    int axisValue;
};

CLASS(JoystickHotkey) {
    SDL_Joystick* joystick;
    int axisNumber;
    int hatNumber;
    int buttonNumber;
    int trackballNumber;
};

#define HOTKEY_TYPE_KEYBOARD 1
#define HOTKEY_TYPE_JOYSTICK 2
CLASS(InputHotkey) {
    union {
        KeyHotkey key;
        JoystickHotkey joystick;
    } hotkey;
    short hotkeyType;
    ActionId id;
};

struct MouseState {
    int x;
    int y;
    int xDelta;
    int yDelta;
    Uint32 mod;
};

CLASS(Input) {
    const Uint8* keystates;
    SDL_Keymod keymods;
    struct MouseState mouseState;

    Vector* joysticks;
    Vector* hotkeys;
};

Input* Input_create();
void Input_destroy(Input* this);
void Input_update(Input* this);
void Input_addHotkey(Input* this, InputHotkey* hotkey);
void Input_loadDefault_1(Input* this);
void Input_loadDefault_2(Input* this);
/* void Input_loadHotkeys(Input* this, lua_State* l, const char* filePath); */
/* void Input_parseKeyboardHotkeys(Input* this, lua_State* l); */
/* void Input_parseJoystickHotkeys(Input* this, lua_State* l); */

// TODO get list or something and choose the best however that should work
SDL_Keycode Input_getHotkeyForAction(Input* this, ActionId action);


int Input_getAxis(Input* this, ActionId id);
float Input_getAxisMultiplier(Input* this, ActionId hotkeyId);
bool Input_isDown(Input* this, ActionId id);

bool Input_keysymIsDown(Input* this, SDL_Keysym* keysym);


InputHotkey* InputHotkey_create(int type, int actionId);

// Utils
SDL_Keycode Input_stringToKeycode(const char* keyText);
ActionId Input_stringToActionId(const char* actionIdText);
char* Input_keycodeToPrintable(SDL_Keycode key);
const char* Input_keycodeToConstantName(SDL_Keycode keyText);
