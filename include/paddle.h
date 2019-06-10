#ifndef __PADDLE_H__
#define __PADDLE_H__

#include "body.h"

typedef struct paddle Paddle;

Paddle *paddle_init(Body *body, Vector inital_center, Vector base_velocity);

Body *paddle_get_body(Paddle *paddle);

Vector paddle_get_initial_center(Paddle *paddle);

Vector paddle_get_base_velocity(Paddle *paddle);

#endif // #ifndef __PADDLE_H__
