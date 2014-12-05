#include "Scene.h"
#include "Utils.h"
#include "Entity.h"
#include "Trigger.h"
#include "Tile.h"
#include "Collectable.h"
#include "Utils.h"
#include "CollisionHandler.h"
#include <stdio.h>

Scene* Scene_create(GameEngine* engine, Scene* lastScene) {
    Scene* this = allocStruct(Scene);

    this->engine = engine;
    SDL_Point screenSize;
    SDL_RenderGetLogicalSize(engine->renderer, &screenSize.x, &screenSize.y);
    this->camera = Camera_create(screenSize.x, screenSize.y);
    this->walkableBounds.x = 0;
    this->walkableBounds.w = 0;
    this->walkableBounds.y = 295 * PHYSICS_SCALE;
    this->walkableBounds.h = 185 * PHYSICS_SCALE;

    this->entities = Vector_Create();
    this->triggers = Vector_Create();
    this->bodies = Vector_Create();
    this->shapes = Vector_Create();

    this->sky = Sky_create(engine->textureCache);

    withSprintf(path, "images/%s/bg.png", anyArgs("red"), {
            SDL_Texture* texture = TextureCache_getForUnconstantPath(engine->textureCache, path);
            this->background = Sprite_create(texture);
        });
    this->background->scrollX = 0.8;

    //Scene_makeGround(this, cpv(0, 50), cpv(400, 400));

    this->gravity = PIXEL_TO_PHYSICS(32*5); // flall 5 tiles per second
    return this;
}

cpShape* Scene_makeGround(Scene* this, cpVect a, cpVect b) {
    cpShape* shape = cpSegmentShapeNew(cpSpaceGetStaticBody(this->engine->space), a, b, 1);
    cpShapeSetFriction(shape, 0.7);
    cpShapeSetCollisionType(shape, COLLISION_TYPE_GROUND);

    cpSpaceAddShape(this->engine->space, shape);
    return shape;
}

SCM Scene_guile_addSegment(SCM scene_smob, SCM ax, SCM ay, SCM bx, SCM by) {
    scm_assert_smob_type (Scene_guile_tag, scene_smob);

    struct Scene *this = (struct Scene*) SCM_SMOB_DATA(scene_smob);
    Scene_makeGround(this, cpv(scm_to_int(ax), scm_to_int(ay)), cpv(scm_to_int(bx), scm_to_int(by)));
    return SCM_BOOL_T;
}

void Scene_makeBox(Scene* this)  {
    cpFloat mass = 1;

    // The moment of inertia is like mass for rotation
    // Use the cpMomentFor*() functions to help you approximate it.
    cpFloat moment = cpMomentForBox(mass, 30, 30);

    cpBody *ballBody = cpSpaceAddBody(this->engine->space, cpBodyNew(mass, moment));
    cpBodySetPosition(ballBody, cpv(30, 15));

    cpShape *ballShape = cpSpaceAddShape(this->engine->space, cpBoxShapeNew(ballBody, 30, 30, 0));
    cpShapeSetFriction(ballShape, 0.7);
}

// TODO macro for this
void Scene_guile_registerType() {
    Scene_guile_tag = scm_make_smob_type("scene", sizeof(struct Scene));
    //scm_set_smob_mark(Scene_guile_tag, Scene_guile_mark);
    //scm_set_smob_free(Scene_guile_tag, Scene_guile_free);
    //scm_set_smob_print(Scene_guile_tag, Scene_guile_print);

    scm_c_define_gsubr ("scene-add-segment", 5, 0, 0, Scene_guile_addSegment);
}

void Scene_createEditorEnvironment(Scene* this) {
    SCM sceneObject = scm_new_smob(Scene_guile_tag, (scm_t_bits)this);
    //SCM module = scm_c_resolve_module("current-scene");
    scm_c_define("*scene*", sceneObject);
    //return module;
}

struct Scene_guile_evalFileData {
    Scene* scene;
    const char* filePath;
};

void* Scene_evaluate(void* voidData) {
    struct Scene_guile_evalFileData* data = (struct Scene_guile_evalFileData*)voidData;
    Scene* this = data->scene;
    const char* path = data->filePath;
    Scene_createEditorEnvironment(this);
    //scm_set_current_module(module);
    scm_c_primitive_load(path);
    /*
    withFileContent(content, path, {
            //SCM code = scm_from_locale_string("(display \"hello\")");
        //scm_eval_string_in_module(code, module);
        //scm_eval_string(code);
    });
    */
    return this;
}

Scene* Scene_createForPath(GameEngine* engine, const char* filePath, Scene* lastScene) {
    struct Scene_guile_evalFileData data = {
        .scene = Scene_create(engine, lastScene),
        .filePath = filePath
    };
    scm_with_guile(Scene_evaluate, &data);
    return data.scene;
}

void Scene_destroy(Scene* this) {
    VECTOR_EACH(this->shapes, shape, {
            cpSpaceRemoveShape(this->engine->space, shape);
        });

    Sprite_destroy(this->background);
    FREE_VECTOR_WITH_ELMENTS(this->entities, Entity_destroy);
    FREE_VECTOR_WITH_ELMENTS(this->triggers, Trigger_destroy);
    FREE_VECTOR_WITH_ELMENTS(this->bodies, cpBodyFree);
    FREE_VECTOR_WITH_ELMENTS(this->shapes, cpShapeFree);
    Camera_destroy(this->camera);
}

void Scene_update(Scene* this, RawTime dt) {
    //CollisionHandler_update(this->collisionHandler, this->entities);
    Sky_updateForDateTime(this->sky);

    for (int i=0; i < this->entities->usedElements; ++i) {
        Entity* it = this->entities->elements[i];
        if (it != NULL) {
//          it->update(it->context, dt);
            if (it->destroyed) {
                Entity_destroy(it);
                Vector_Set(this->entities, i, NULL);
            } else {
                Entity_update(it, dt);
                if (it->physics.groundedStatus == grounded || it->physics.groundedStatus == inAir) {
                    it->physics.dy += this->gravity;
                }
            }
        }
    }
    Camera_update(this->camera);
}

void Scene_draw(Scene* this, SDL_Renderer* renderer) {
    Sprite_drawOnCamera(this->background, renderer, this->camera);
    Sky_draw(this->sky, renderer, this->camera);

    for (int i=0; i < this->entities->usedElements; ++i) {
        Entity* it = this->entities->elements[i];
        if (it != NULL) {
            it->draw(it->context, renderer, this->camera);
        }
    }

    GameEngine_debugDraw(this->engine, this->camera);
}

void Scene_transfereEntity(Scene* this, Entity* entity, Scene* other) {
    Vector_Overwrite(this->entities, entity, NULL);
    Scene_addEntity(other, entity);
}

void Scene_addEntity(Scene* this, Entity* entity) {
    Vector_InsertInFirstFreeSpace(this->entities, entity);
    cpSpaceAddBody(this->engine->space, entity->physics.body);
    //cpBodyEachShape(entity->physics.body, Chipmunk_addEachShapeToSpace, this->engine->space);
    VECTOR_EACH(entity->physics.shapes, shape, {
                cpSpaceAddShape(this->engine->space, shape);
        })
}
void Scene_addShape(Scene* this, cpShape* shape) {
    Vector_InsertInFirstFreeSpace(this->shapes, shape);
    cpSpaceAddShape(this->engine->space, shape);
}

void Scene_setBounds(Scene* this, int x, int y, int w, int h) {
    this->walkableBounds.x = x * PHYSICS_SCALE;
    this->walkableBounds.y = y * PHYSICS_SCALE;
    this->walkableBounds.w = w * PHYSICS_SCALE;
    this->walkableBounds.h = h * PHYSICS_SCALE;
    /* this->camera->bounds.x = x; */
    /* this->camera->bounds.y = y; */
    /* this->camera->bounds.w = w; */
    /* this->camera->bounds.h = h; */
}

void Scene_openDoors(Scene* this, int type) {
    /* TODO remove collision for doors of -type- */
}

void Scene_spawnCollectable(Scene* this, int tileId, int type) {
    /* Collectable* c = NULL; */
    /* if (type == COLLECTABLE_COIN) { */
    /*     c = Collectable_createCoin(this, Scene_positionForTileId(this, tileId), 5); */
    /* } else if (type == COLLECTABLE_COIN_BIG) { */
    /*     c = Collectable_createCoin(this, Scene_positionForTileId(this, tileId), 25); */
    /* } else if (type >= COLLECTABLE_KEY_4 && type <= COLLECTABLE_KEY_6) { */
    /*     c = Collectable_createKey(this, Scene_positionForTileId(this, tileId), type); */
    /* } else if (type == COLLECTABLE_POTION) { */
    /*     c = Collectable_createPotion(this, Scene_positionForTileId(this, tileId), 25); */
    /* } */

    /* if (c) { */
    /*     Scene_addEntity(this, c->entity); */
    /* } */
}
