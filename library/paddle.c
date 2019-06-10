#include "paddle.h"

struct paddle {
    Body *body;
    Vector initial_center;
    Vector axis;
    Vector positive_boundary;
    Vector negative_boundary;
};

Paddle *paddle_init(Body *body, Vector initial_center, Vector axis, Vector positive_boundary, Vector negative_boundary){
    Paddle *paddle = malloc(sizeof(Paddle));
    assert(paddle != NULL);
    paddle->body = body;
    paddle->initial_center = initial_center;
    paddle->axis = axis;
    paddle->positive_boundary = positive_boundary;
    paddle->negative_boundary = negative_boundary;
    return paddle;
}

Body *paddle_get_body(Paddle *paddle){
    return paddle->body;
}

Vector paddle_get_initial_center(Paddle *paddle){
    return paddle->initial_center;
}

Vector paddle_get_axis(Paddle *paddle){
    return paddle->axis;
}

Vector paddle_get_positive_boundary(Paddle *paddle){
    return paddle->positive_boundary;
}

Vector paddle_get_negative_boundary(Paddle *paddle){
    return paddle->negative_boundary;
}
