#include "forces.h"
#include <time.h>

typedef struct grav_param{
    double G;
    List *bodies;
    bool force_on_body2;
}GravParam;

typedef struct spring_param{
    double k;
    List *bodies;
}SpringParam;

typedef struct drag_param{
    double gamma;
    List *bodies;
}DragParam;

typedef struct coll_param{
    List *bodies;
    CollisionHandler coll_func;
    void *aux;
    FreeFunc freef;
    bool collided_bef;
}CollParam;

typedef struct ai_param{
  List *bodies;
  double check_time;
  double time_since_check;
  Vector axis;
  FreeFunc freef;
}AiParam;

/**
* @brief ForceCreator function that adds a newtonian gravity force between two bodies
*
* @param aux an object that holds parameters necessary to create gravitational force
*/
void newtonian_gravity(void *aux);

/**
* @brief ForceCreator function that adds a spring force between two bodies
*
* @param aux an object that holds parameters necessary to create spring force
*/
void spring(void *aux);

/**
* @brief ForceCreator function that adds a drag force to a body
*
* @param aux an object that holds parameters necessary to create drag force
*/
void drag(void *aux);

/**
* @brief ForceCreator function that detects and handles collisions between two
* bodies
*
* @param aux an object that holds parameters necessary to add collision
* detecting and handling between the objects
*/
void collision(void *aux);

void pong_ai(void *aux);

/**
* @brief Frees CollParam object
*
* @param coll CollParam to be freed
*/
void free_coll_param(CollParam *coll);

/**
* @brief CollisionHandler function that sets two bodies to be rmoved if they
* collide
*/
void destructive_collision(Body *body1, Body *body2, Vector axis, void *aux);

/**
* @brief CollisionHandler function that causes two bodies to bounce off
* eachother if they collide along collision axis calculated in find_collision
*/
void physics_collision(Body *body1, Body *body2, Vector axis, void *aux);

void create_newtonian_gravity(Scene *scene, double G, Body *body1, Body *body2,
                                    bool force_on_body2){
    GravParam* grav = malloc(sizeof(GravParam));
    grav->G = G;
    List *grav_bodies = list_init(2, (FreeFunc)free, NULL);

    Body **body1_point = malloc(sizeof(Body *));
    Body **body2_point = malloc(sizeof(Body *));

    *(body1_point) = body1;
    *(body2_point) = body2;

    list_add(grav_bodies, (void *)body1_point);
    list_add(grav_bodies, (void *)body2_point);
    grav->bodies = grav_bodies;

    grav->force_on_body2 = force_on_body2;

    scene_add_bodies_force_creator(scene, (ForceCreator)newtonian_gravity, grav,
                                            (FreeFunc)free, grav_bodies);
}

void create_spring(Scene *scene, double k, Body *body1, Body *anchor){
    SpringParam* new_spring = malloc(sizeof(SpringParam));
    List *spring_bodies = list_init(2, (FreeFunc)free, NULL);

    Body **body1_point = malloc(sizeof(Body *));
    Body **anchor_point = malloc(sizeof(Body *));

    *(body1_point) = body1;
    *(anchor_point) = anchor;

    list_add(spring_bodies, (void *)body1_point);
    list_add(spring_bodies, (void *)anchor_point);

    new_spring->k = k;
    new_spring->bodies = spring_bodies;

    scene_add_bodies_force_creator(scene, (ForceCreator)spring, new_spring,
                                            (FreeFunc)free, spring_bodies);
}/* expression */

void create_drag(Scene *scene, double gamma, Body *body){
    DragParam* new_drag = malloc(sizeof(DragParam));
    List *drag_body = list_init(1, (FreeFunc)free, NULL);

    Body **body_point = malloc(sizeof(Body *));

    *(body_point) = body;

    list_add(drag_body, (void *)body_point);

    new_drag->gamma = gamma;
    new_drag->bodies = drag_body;

    scene_add_bodies_force_creator(scene, (ForceCreator)drag, new_drag,
                                                (FreeFunc)free, drag_body);
}

void create_collision(
    Scene *scene,
    Body *body1,
    Body *body2,
    CollisionHandler handler,
    void *aux,
    FreeFunc freer
){
    CollParam* new_coll = malloc(sizeof(CollParam));
    List *coll_bodies = list_init(2, (FreeFunc)free, NULL);

    Body **body1_point = malloc(sizeof(Body *));
    Body **body2_point = malloc(sizeof(Body *));

    *(body1_point) = body1;
    *(body2_point) = body2;

    list_add(coll_bodies, (void *)body1_point);
    list_add(coll_bodies, (void *)body2_point);
    new_coll->bodies = coll_bodies;
    new_coll->aux = aux;
    new_coll->coll_func = handler;
    new_coll->freef = freer;
    new_coll->collided_bef = false;

    scene_add_bodies_force_creator(scene, (ForceCreator) collision, new_coll,
                                    (FreeFunc)free_coll_param, coll_bodies);
}

void create_destructive_collision(Scene *scene, Body *body1, Body *body2){
    create_collision(scene, body1, body2, (CollisionHandler)destructive_collision, NULL, NULL);
}

void create_physics_collision(Scene *scene, double elasticity, Body *body1, Body *body2){
    double *elas = malloc(sizeof(double));
    *(elas) = elasticity;
    create_collision(scene, body1, body2, (CollisionHandler)physics_collision, elas, free);
}

void create_ai(Scene *scene, Paddle *paddle, Body *target, AiDifficulty diff){
  AiParam *new_ai = malloc(sizeof(AiParam));
  double ai_time;
  Body *smart = paddle_get_body(paddle);

  switch (diff) {
   case EASY:
    ai_time = 0.3;
    break;

   case MEDIUM:
    ai_time = 0.05;
    break;

   case HARD:
    ai_time = 0.01;
    break;
  }

  List *ai_bodies = list_init(2, NULL, NULL);
  list_add(ai_bodies, smart);
  list_add(ai_bodies, target);
  new_ai->bodies = ai_bodies;
  new_ai->check_time = ai_time;
  new_ai->time_since_check = 0;
  new_ai->axis = paddle_get_axis(paddle);
  scene_add_bodies_force_creator(scene, (ForceCreator) pong_ai, new_ai, (FreeFunc)free, ai_bodies);
}

void pong_ai(void *aux){
  AiParam *ai = (AiParam *)aux;
  List *ai_bodies = ai->bodies;
  Body *paddle = (Body *)list_get(ai_bodies, 0);
  Body *ball = (Body *)list_get(ai_bodies, 1);
  Vector axis = ai->axis;
  if(ai->time_since_check > ai->check_time){
    set_paddle_vel(paddle, ball, axis);
    ai->time_since_check = 0;
    return;
  }
  ai->time_since_check += time_since_last_ai_tick();
}

void collision(void *aux){
    CollParam *coll = (CollParam *)aux;
    List* coll_bodies = coll->bodies;
    Body *body1 = *((Body **)list_get(coll_bodies, 0));
    Body *body2 = *((Body **)list_get(coll_bodies, 1));
    Vector axis = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    axis = vec_multiply(1/vec_magnitude(axis), axis);
    if(!(coll->collided_bef)){
      coll->coll_func(body1, body2, axis, coll->aux);
      coll->collided_bef = true;
    }
    else{
      coll->collided_bef = false;
    }
}

void newtonian_gravity(void *aux){
    GravParam* grav = (GravParam *)aux;
    Body *Body1 = (Body *) *((Body **)list_get(grav->bodies, 0));
    Body *Body2 = (Body *) *((Body **)list_get(grav->bodies, 1));
    Vector distance = vec_subtract(body_get_centroid(Body2),
                                    body_get_centroid(Body1));

    if(vec_magnitude(distance)>10){
        double force_mag = (grav->G * body_get_mass(Body1) *
                            body_get_mass(Body2))/pow(vec_magnitude(distance),2);
        Vector unit_dist = vec_multiply(1/vec_magnitude(distance), distance);
        Vector force = vec_multiply(force_mag, unit_dist);

        body_add_force(Body1, force);
        if(grav->force_on_body2){
            body_add_force(Body2, vec_negate(force));
        }
    }
}

void spring(void *aux){
    SpringParam* spring_force = (SpringParam*)aux;
    Body *Body1 = (Body *) *((Body **)list_get(spring_force->bodies, 0));
    Body *Body2 = (Body *) *((Body **)list_get(spring_force->bodies, 1));

    Vector distance = vec_subtract(body_get_centroid(Body2),
                                    body_get_centroid(Body1));
    double force_mag = spring_force->k * vec_magnitude(distance);
    Vector unit_dist = vec_multiply(1/vec_magnitude(distance), distance);
    Vector force = vec_multiply(force_mag, unit_dist);

    body_add_force(Body1, force);
    body_add_force(Body2, vec_negate(force));
}

void drag(void *aux){
    DragParam* drag_force = (DragParam*)aux;
    Body *body = (Body *) *((Body **)list_get(drag_force->bodies, 0));
    Vector force = vec_multiply(drag_force->gamma,
                                        vec_negate(body_get_velocity(body)));
    body_add_force(body, force);
}

void destructive_collision(Body *body1, Body *body2, Vector axis, void *aux){
    List *shape1 = body_get_shape(body1);
    List *shape2 = body_get_shape(body2);
    CollisionInfo coll = find_collision(shape1, shape2);
    if(coll.collided){
        body_remove(body1);
        body_remove(body2);
    }
    list_free(shape1);
    list_free(shape2);
}

void physics_collision(Body *body1, Body *body2, Vector axis, void *aux){
    List *shape1 = body_get_shape(body1);
    List *shape2 = body_get_shape(body2);
    double elasticity = *((double *)aux);
    CollisionInfo new_coll = find_collision(shape1, shape2);
    if(new_coll.collided){
        double impulse_mag;
        double reduced_mass;
        if(body_get_mass(body1) == INFINITY){
            reduced_mass = body_get_mass(body2);
        }
        else if(body_get_mass(body2) == INFINITY){
            reduced_mass = body_get_mass(body1);
        }
        else{
            reduced_mass = (body_get_mass(body1) * body_get_mass(body2)) /
                                (body_get_mass(body2) + body_get_mass(body1));
        }

        impulse_mag = reduced_mass * (1 + elasticity)*
                            (vec_dot(new_coll.axis, body_get_velocity(body2)) -
                            vec_dot(new_coll.axis, body_get_velocity(body1)));

        Vector impulse = vec_multiply(impulse_mag, new_coll.axis);

        body_add_impulse(body1, impulse);
        body_add_impulse(body2, vec_negate(impulse));
        body_add_collision(body1);
        body_add_collision(body2);
    }
    list_free(shape1);
    list_free(shape2);
}



void free_coll_param(CollParam *coll){
    if(coll->aux != NULL && coll->freef != NULL){
        coll->freef(coll->aux);
    }
    free(coll);
}
