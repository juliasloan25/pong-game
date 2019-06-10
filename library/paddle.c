#include "paddle.h"

struct paddle {
    Body *body;
    Vector inital_center;
    Vector base_velocity;
}

Paddle *paddle_init(Body *body, Vector inital_center, Vector base_velocity){
    Paddle *paddle = malloc(sizeof(Paddle));
    assert(paddle != NULL);
    paddle->body = body;
    paddle->inital_center = initial_center;
    paddle->base_velocity = base_velocity;
    return paddle;
}

Body *paddle_get_body(Paddle *paddle){
    return paddle->body;
}

Vector paddle_get_initial_center(Paddle *paddle){
    return paddle->initial_center;
}

Vector paddle_get_base_velocity(Paddle *paddle){
    return paddle->base_veloctiy;
}
