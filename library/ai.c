#include "ai.h"

clock_t ai_clock = 0;

void set_paddle_vel(Body *paddle, Body *ball, Vector axis){
    Vector ball_cent = body_get_centroid(ball);
    Vector ball_vel = body_get_velocity(ball);
    Vector paddle_cent = body_get_centroid(paddle);
    Vector base_velocity = vec_multiply(PADDLE_VEL, axis);

    Vector difference = vec_subtract(ball_cent, paddle_cent);
    Vector perp = vec_rotate(axis, 3 * M_PI / 2);
    double dot = vec_dot(perp, difference);
    Vector proj_diff = vec_multiply(dot/(pow(vec_magnitude(perp),2)), perp);

    double dot2 = vec_dot(proj_diff, ball_vel);
    Vector proj_vel = vec_multiply(dot2/(pow(vec_magnitude(proj_diff),2)), proj_diff);

    double proj_diff_mag = vec_magnitude(proj_diff);
    double proj_vel_mag = vec_magnitude(proj_vel);

    double t = proj_diff_mag / proj_vel_mag;
    Vector intersect = vec_add(vec_multiply(t, ball_vel), ball_cent);

    Vector diff_intersect = vec_subtract(intersect, ball_cent);
    double dot_final = vec_dot(diff_intersect, axis);

    if(dot_final > 0){
        body_set_velocity(paddle, base_velocity);
    }
    else{
        body_set_velocity(paddle, vec_negate(base_velocity));
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
