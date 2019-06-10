#include "ai.h"

clock_t ai_clock = 0;

void set_paddle_vel(Body *paddle, Body *ball, Vector axis){
    Vector ball_cent = body_get_centroid(ball);
    Vector ball_vel = body_get_velocity(ball);
    Vector paddle_cent = body_get_centroid(paddle);
    Vector base_velocity = vec_multiply(PADDLE_VEL, axis);

    double rel_paddle_x = paddle_cent.x - (WIDTH/2);
    if((rel_paddle_x / ball_vel.x) < 0){
        body_set_velocity(paddle, VEC_ZERO);
        return;
    }

    double x_pos_diff = paddle_cent.x - ball_cent.x;
    if(fabs(x_pos_diff) > 20){
        double t = x_pos_diff / ball_vel.x;

        double final_y_pos = ball_vel.y *(t) + ball_cent.y;

        double y_diff = final_y_pos - paddle_cent.y;

        if(fabs(y_diff - 0) < 5){
            body_set_velocity(paddle, VEC_ZERO);
        }
        else if(y_diff > 0){
            body_set_velocity(paddle, (Vector){0, PADDLE_VEL});
        }
        else if(y_diff < 0){
            body_set_velocity(paddle, (Vector){0, -1 * PADDLE_VEL});
        }
    }
}

double time_since_last_ai_tick(void) {
    clock_t now = clock();
    double difference = ai_clock
        ? (double) (now - ai_clock) / CLOCKS_PER_SEC
        : 0.0; // return 0 the first time this is called
    ai_clock = now;
    return difference;
}
