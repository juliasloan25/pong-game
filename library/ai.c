#include "ai.h"

clock_t ai_clock = 0;

void set_paddle_vel(Paddle *paddle, Body *ball){
    Vector ball_cent = body_get_centroid(ball);
    Vector ball_vel = body_get_velocity(ball);
    Vector paddle_cent = body_get_centroid(paddle);
    Vector base_velocity = vec_multiply(PADDLE_VEL, axis);
    Vector axis = paddle_get_axis(paddle);
    Vector cent_diff = vec_subtract(ball_cent, paddle_cent);
    Vector axis_perp = {axis.y, -1 * axis.x};

    Vector proj_ball_cent_par = vec_multiply(vec_dot(ball_cent, axis)/vec_dot(axis, axis), axis);

    Vector proj_cent_perp = vec_multiply(vec_dot(cent_diff, axis_perp)/vec_dot(axis_perp, axis_perp), axis_perp);
    Vector proj_cent_par = vec_multiply(vec_dot(cent_diff, axis)/vec_dot(axis, axis), axis);

    Vector proj_vel_perp = vec_multiply(vec_dot(ball_vel, axis_perp)/vec_dot(axis_perp, axis_perp), axis_perp);
    Vector proj_vel_par = vec_multiply(vec_dot(ball_vel, axis)/vec_dot(axis, axis), axis);

    Body *paddle_body = paddle_get_body(paddle);


    double t = vec_magnitude(proj_cent_perp)/vec_magnitude(proj_vel_perp);

    Vector final_y_pos = vec_add(vec_multiply(t, proj_vel_par), proj_ball_cent_par);

    Vector pos_inter_diff = vec_subtract(final_y_pos, paddle_cent);

    if(vec_dot(pos_inter_diff, axis) < 0){
      body_set_velocity(paddle_body, vec_multiply(PADDLE_VEL,vec_negate(axis)));
      return;
    }
    else if(vec_dot(pos_inter_diff, axis) > 0){
      body_set_velocity(paddle_body, vec_multiply(PADDLE_VEL, axis));
    }
    else{
      body_set_velocity(paddle_body, VEC_ZERO);
    }
    /*double rel_paddle_x = paddle_cent.x - (WIDTH/2);
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
    }*/
}

double time_since_last_ai_tick(void) {
    clock_t now = clock();
    double difference = ai_clock
        ? (double) (now - ai_clock) / CLOCKS_PER_SEC
        : 0.0; // return 0 the first time this is called
    ai_clock = now;
    return difference;
}
