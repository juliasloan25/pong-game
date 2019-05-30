#include "ai.h"
#include "pong.h"

void set_paddle_vel(Body *paddle, Body *ball){
  Vector ball_vel = body_get_velocity(ball);
  Vector paddle_vel = body_get_velocity(paddle);
  Vector ball_cent = body_get_centroid(ball);
  Vector paddle_cent = body_get_centroid(paddle);

  if(ball_vel.x < 0){
    body_set_velocity(paddle, VEC_ZERO);
    return;
  }

  double x_pos_diff = paddle_cent.x - ball_cent.x;
  if(x_pos_diff > 5){
    double t = x_pos_diff / ball_vel.x;

    double final_y_pos = ball_vel.y *(t) + ball_cent.y;

    double y_diff = final_y_pos - paddle_cent.y;

    double new_y_vel = y_diff/t;

    body_set_velocity(paddle, (Vector){0, new_y_vel/2});
  }

}
