#include "scene.h"

struct scene{
    List *BodyList;
    List *ForceCreators;
    bool end;
};

struct Force_Param{
    ForceCreator force_func;
    void *aux;
    FreeFunc freef;
    List *bodies;
};

void force_free(ForceParam *force){
    force->freef(force->aux);
    list_free(force->bodies);
    free(force);
}

Scene *scene_init(void){
    Scene *new_scene = malloc(sizeof(Scene));
    new_scene->BodyList = list_init(100, (FreeFunc) body_free, NULL);
    new_scene->ForceCreators = list_init(100, (FreeFunc) force_free, NULL);
    new_scene->end = false;
    return new_scene;
}

void scene_free(Scene *scene){
    list_free(scene->BodyList);
    list_free(scene->ForceCreators);
    free(scene);
}

size_t scene_bodies(Scene *scene){
    return list_size(scene->BodyList);
}

Body *scene_get_body(Scene *scene, size_t index){
    return (Body *)list_get(scene->BodyList, index);
}

void scene_add_body(Scene *scene, Body *body){
    list_add(scene->BodyList, (void *)body);
}

void scene_add_bodies_force_creator(
    Scene *scene,
    ForceCreator forcer,
    void *aux,
    FreeFunc freer,
    List *bodies
  ){
    ForceParam *new_force = malloc(sizeof(ForceParam));
    new_force->force_func = forcer;
    new_force->aux = aux;
    new_force->freef = freer;
    new_force->bodies = bodies;
    list_add(scene->ForceCreators, new_force);
}


void scene_add_force_creator(
    Scene *scene,
    ForceCreator forcer,
    void *aux,
    FreeFunc freer
){
    scene_add_bodies_force_creator(scene, forcer, aux, freer, NULL);
}

void scene_remove_body(Scene *scene, size_t index){
    Body *body = scene_get_body(scene, index);
    for(int i = 0; i < list_size(scene->ForceCreators); i++){
        ForceParam *curr_force = (ForceParam*)list_get(scene->ForceCreators, i);
        List *bodies = curr_force->bodies;
        for(int j = 0; j < list_size(bodies); j++){
            if((Body *)*((Body **)list_get(bodies, j)) == body){
                list_remove(scene->ForceCreators, i);
                i--;
                break;
            }
        }
    }
    list_remove(scene->BodyList, index);
}

void scene_tick(Scene *scene, double dt){
    for(int i = 0; i < list_size(scene->ForceCreators); i++){
        ForceParam *curr_force = (ForceParam*)list_get(scene->ForceCreators, i);
        curr_force->force_func(curr_force->aux);
    }

    for(int i = 0; i < scene_bodies(scene); i++){
        body_tick(scene_get_body(scene, i), dt);
        if(body_is_removed(scene_get_body(scene, i))){
            scene_remove_body(scene, i);
        }
    }
}

void scene_set_end(Scene *scene){
    scene->end = true;
}

bool scene_get_end(Scene *scene){
    return scene->end;
}
