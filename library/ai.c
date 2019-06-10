#include "ai.h"

clock_t ai_clock = 0;

void set_paddle_vel(Body *paddle, Body *ball, Vector axis){
    Vector ball_cent = body_get_centroid(ball);
    Vector ball_vel = body_get_velocity(ball);
    Vector paddle_cent = body_get_centroid(paddle);
    Vector base_velocity = vec_multiply(PADDLE_VEL, axis);
    //printf("paddle_cent %f %f\n", paddle_cent.x, paddle_cent.y);
    //printf("ball_cent %f %f\n", ball_cent.x, ball_cent.y);
    //printf("ball_vel %f %f\n", ball_vel.x, ball_vel.y);

    Vector difference = vec_subtract(ball_cent, paddle_cent);
    if(vec_magnitude(difference) > 20)
    {
        Vector perp = vec_rotate(axis, 3 * M_PI / 2);
        double dot = vec_dot(perp, difference);
        Vector proj_diff = vec_multiply(dot/(pow(vec_magnitude(perp),2)), perp);
        //printf("proj_diff %f %f\n", proj_diff.x, proj_diff.y);

        double dot2 = vec_dot(proj_diff, ball_vel);
        Vector proj_vel = vec_multiply(dot2/(pow(vec_magnitude(proj_diff),2)), proj_diff);
        //printf("proj_vel %f %f\n", proj_vel.x, proj_vel.y);
        double dot3 = vec_dot(proj_vel, perp);

        if(dot3 < 0){
            double proj_diff_mag = vec_magnitude(proj_diff);
            double proj_vel_mag = vec_magnitude(proj_vel);

            double t = proj_diff_mag / proj_vel_mag;
            Vector intersect = vec_add(vec_multiply(t, ball_vel), ball_cent);
            //printf("time %f\n", t);
            //printf("intersect %f %f\n", intersect.x, intersect.y);

            Vector diff_intersect = vec_subtract(intersect, paddle_cent);
            double dot_final = vec_dot(diff_intersect, axis);
            //printf("diff_intersect %f %f\n", diff_intersect.x, diff_intersect.y);
            //printf("axis %f %f\n", axis.x, axis.y);
            //printf("dot_final %f\n", dot_final);
            if(vec_magnitude(diff_intersect) < 40){
                body_set_velocity(paddle, VEC_ZERO);
                return;
            }
            if(dot_final > 0){
                body_set_velocity(paddle, base_velocity);
                return;
            }
            else if(dot_final < 0){
                body_set_velocity(paddle, vec_negate(base_velocity));
                return;
            }
            else{
                body_set_velocity(paddle, VEC_ZERO);
                return;
            }
        }
        else{
            body_set_velocity(paddle, VEC_ZERO);
            return;
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
