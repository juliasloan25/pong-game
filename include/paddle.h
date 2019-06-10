#ifndef __PADDLE_H__
#define __PADDLE_H__

#include "body.h"

typedef struct paddle Paddle;

Paddle *paddle_init(Body *body, Vector inital_center, Vector axis, Vector positive_boundary, Vector negative_bondary);

Body *paddle_get_body(Paddle *paddle);

Vector paddle_get_initial_center(Paddle *paddle);

Vector paddle_get_axis(Paddle *paddle);

Vector paddle_get_positive_boundary(Paddle *paddle);

Vector paddle_get_negative_boundary(Paddle *paddle);

#endif // #ifndef __PADDLE_H__
