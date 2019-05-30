#include "ai.h"

void set_paddle_vel(Body *paddle, Body *ball, double paddle_vel){
  Vector ball_vel = body_get_velocity(ball);
  Vector ball_cent = body_get_centroid(ball);
  Vector paddle_cent = body_get_centroid(paddle);

  if(ball_vel.x < 0){
    body_set_velocity(paddle, VEC_ZERO);
    return;
  }

  double x_pos_diff = paddle_cent.x - ball_cent.x;
  if(x_pos_diff > 20){
    double t = x_pos_diff / ball_vel.x;

    double final_y_pos = ball_vel.y *(t) + ball_cent.y;

    double y_diff = final_y_pos - paddle_cent.y;

    if(fabs(y_diff - 0) < 5){
      body_set_velocity(paddle, VEC_ZERO);
    }
    else if(y_diff > 0){
      body_set_velocity(paddle, (Vector){0, paddle_vel});
    }
    else if(y_diff < 0){
      body_set_velocity(paddle, (Vector){0, -1 * paddle_vel});
    }

  }

}
