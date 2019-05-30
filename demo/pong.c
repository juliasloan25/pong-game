#include "pong.h"

const double WIDTH = 800; //screen width
const double HEIGHT = 600; //screen height
const double BALL_RADIUS = 10.0; //radius of pong ball
const double PADDLE_HEIGHT = 100.0; //width of the pong paddle
const double PADDLE_WIDTH = 30.0; // height of the pong paddle
const double BALL_MASS = 100; // mass of the pong ball;
const double ELASTICITY = 1; //elasticity of collisions
const double PADDLE_VEL = 600.0; //velocity that the paddle can go
const double MASS = 50.0; //mass of all objects
const double BALL_VEL = 700.0; // initial velocity of ball
const RGBColor PADDLE_COLOR = {
    .r = 0,
    .g = 0,
    .b = 0
}; //black paddle color
const RGBColor BALL_COLOR = {
    .r = 1,
    .g = 0,
    .b = 0
}; //color of the pong ball
const Vector paddle_one_center = {
    .x = PADDLE_WIDTH / 2.0,
    .y = HEIGHT/2
}; //initial and reset center of paddle one
const Vector paddle_two_center = {
    .x = WIDTH - (PADDLE_WIDTH / 2.0),
    .y  = HEIGHT/2
}; //initial and reset center of paddle two
const Vector ball_center = {
    .x = WIDTH / 2.0,
    .y = HEIGHT/ 2.0
}; //initial and reset center of the ball


int main(int argc, char **argv){
    //initialize scene and window
    Scene *scene = scene_init();
    window_init();

    //creates two paddles and initializes them on either side of the screen
    BodyType *paddle_one_type = malloc(sizeof(BodyType));
    *(paddle_one_type) = PADDLE;
    Body *paddle_one = make_body(paddle_one_type, paddle_one_center);
    scene_add_body(scene, paddle_one);

    BodyType *paddle_two_type = malloc(sizeof(BodyType));
    *(paddle_two_type) = PADDLE;
    Body *paddle_two = make_body(paddle_two_type, paddle_two_center);
    scene_add_body(scene, paddle_two);

    //create ball
    BodyType *ball_type = malloc(sizeof(BodyType));
    *(ball_type) = BALL;
    Body *ball = make_body(ball_type, ball_center);
    scene_add_body(scene, ball);

    //create bouncing collision between paddles and ball
    create_physics_collision(scene, ELASTICITY, paddle_one, ball);
    create_physics_collision(scene, ELASTICITY, paddle_two, ball);

    sdl_on_key(on_key); //handles key inputs

    //initialize scores
    int left_score = 0; //score of player with left paddle
    int right_score = 0; //score of player with right paddle
    double ai_timer = 0;
    while(!sdl_is_done(scene)) {
        double wait_time = time_since_last_tick();
        ai_timer += wait_time;
        //checks if paddle or ball has hit walls
        char ball_hit_side = move_if_offscreen(paddle_one, paddle_two, ball);
        if(ball_hit_side == 'l'){ //add point to right player
            right_score++;
            reset(scene);
        }
        else if (ball_hit_side == 'r') { //add point to left player
            left_score++;
            reset(scene);
        }
        if(ai_timer > 0.01){
          ai_timer = 0;
          set_paddle_vel(paddle_two, ball, PADDLE_VEL);
        }

        //render and update scene at every tick
        scene_tick(scene, wait_time);
        sdl_render_scene(scene);

        // end game if either score reaches 10
        if (right_score >= 10 || left_score >= 10 || scene_get_end(scene)){
            break;
        }

        //display the current scores on-screen
        //display_text(itoa(right_score), 30);
        //display_text(itoa(left_score), 30);
    }

    //free all elements of scene
    scene_free(scene);
    //TTF_Quit();
    return 1;
}

void window_init(){
    Vector vec_min = VEC_ZERO;
    Vector vec_max = {
        .x = WIDTH,
        .y = HEIGHT
    };
    sdl_init(vec_min, vec_max);
}

Body *make_body(BodyType *type, Vector center){
    Body *body;
    Vector velocity = VEC_ZERO;
    size_t points;
    RGBColor color;
    double mass;
    if (*(type) == BALL) {
        mass = 50;
        points = 2000; //large number of points to create circle
        color = BALL_COLOR;
        velocity.x = BALL_VEL;
        velocity.y = BALL_VEL;
    }
    if(*(type) == PADDLE){
        mass = INFINITY;
        points = 4; //to form rectangle
        color = PADDLE_COLOR;
    }
    List *shape = list_init(points, free, (EqualFunc)vec_equal_v);

    //construct the ball
    if (*(type)==BALL){
        double angle = (2 * M_PI / points);
        for (int i =0; i<points; i++){
            Vector *vec = malloc(sizeof(Vector));
            vec->x = BALL_RADIUS *cos(angle*i);
            vec->y = BALL_RADIUS *sin(angle*i);
            list_add(shape, (void*)vec);
        }

    }
    //construct paddle
    else{
        //add four points
        Vector translate_up_left = {
            .x = -1.0 * PADDLE_WIDTH / 2.0,
            .y = PADDLE_HEIGHT / 2.0
        };
        Vector *vec1 = malloc(sizeof(Vector));
        *vec1 = vec_add(translate_up_left, center);
        list_add(shape, vec1);

        Vector translate_down_left = {
            .x = -1.0 * PADDLE_WIDTH / 2.0,
            .y = -1.0 * PADDLE_HEIGHT / 2.0
        };
        Vector *vec2 = malloc(sizeof(Vector));
        *vec2 = vec_add(translate_down_left, center);
        list_add(shape, vec2);

        Vector translate_down_right = {
            .x = PADDLE_WIDTH / 2.0,
            .y = -1.0 * PADDLE_HEIGHT / 2.0
        };
        Vector *vec3 = malloc(sizeof(Vector));
        *vec3 = vec_add(translate_down_right, center);
        list_add(shape, vec3);

        Vector translate_up_right = {
            .x = PADDLE_WIDTH / 2.0,
            .y = PADDLE_HEIGHT / 2.0
        };
        Vector *vec4 = malloc(sizeof(Vector));
        *vec4 = vec_add(translate_up_right, center);
        list_add(shape, vec4);
    }

    body = body_init_with_info(shape, mass, color, (void*)type, (FreeFunc)free);
    body_set_centroid(body,center);
    body_set_velocity(body,velocity);

    return body;
}

void on_key(char key, KeyEventType type, double held_time, Scene *scene) {
    Body *paddle_one = scene_get_body(scene, 0);

    //velocity to use if an arrow key was pressed
    Vector new_vel = {
        .x = 0,
        .y = PADDLE_VEL
    };

    if (type == KEY_PRESSED) {
        switch(key) {
            case UP_ARROW:
                //makes paddle go up if up arrow is pressed
                body_set_velocity(paddle_one, new_vel);
                break;

            case DOWN_ARROW:
                //makes paddle go down if down arrow is pressed
                new_vel.y = -1.0 * new_vel.y;
                body_set_velocity(paddle_one, new_vel);
                break;

            case ESCAPE:
                //ends the scene
                scene_set_end(scene);
        }
    }
    else{
        //sets velocity of shooter back to zero after arrow key is released
        switch (key) {
            case UP_ARROW:
                body_set_velocity(paddle_one, VEC_ZERO);
                break;

            case DOWN_ARROW:
                body_set_velocity(paddle_one, VEC_ZERO);
                break;
        }
    }
}

char move_if_offscreen(Body *paddle_one, Body *paddle_two, Body *ball){
    Vector paddle_one_center = body_get_centroid(paddle_one);
    Vector paddle_two_center = body_get_centroid(paddle_two);
    Vector ball_center = body_get_centroid(ball);

    if(paddle_one_center.y <  PADDLE_HEIGHT / 2.0){
        Vector new_center = {
            .x = PADDLE_WIDTH / 2.0,
            .y = PADDLE_HEIGHT / 2.0
        };
        body_set_centroid(paddle_one, new_center);
    }
    if(paddle_one_center.y > (HEIGHT - PADDLE_HEIGHT / 2.0)){
        Vector new_center = {
            .x = PADDLE_WIDTH / 2.0,
            .y = HEIGHT - PADDLE_HEIGHT / 2.0
        };
        body_set_centroid(paddle_one, new_center);
    }

    if(paddle_two_center.y <  PADDLE_HEIGHT / 2.0){
        Vector new_center = {
            .x = WIDTH - PADDLE_WIDTH / 2.0,
            .y = PADDLE_HEIGHT / 2.0
        };
        body_set_centroid(paddle_two, new_center);
    }
    if(paddle_two_center.y > (HEIGHT - PADDLE_HEIGHT / 2.0)){
        Vector new_center = {
            .x = WIDTH - PADDLE_WIDTH / 2.0,
            .y = HEIGHT - PADDLE_HEIGHT / 2.0
        };
        body_set_centroid(paddle_two, new_center);
    }

    if(ball_center.x < BALL_RADIUS){ //ball goes off left side (return 'l')
        Vector translate = {
            .x = BALL_RADIUS - ball_center.x,
            .y = 0.0
        };
        body_set_centroid(ball, vec_add(ball_center, translate));
        Vector velocity = body_get_velocity(ball);
        velocity.x *= -1.0;
        body_set_velocity(ball, velocity);
        return 'l';
    }
    if(ball_center.x > WIDTH - BALL_RADIUS){ //ball goes off right side (return 'r')
        Vector translate = {
            .x = WIDTH - BALL_RADIUS - ball_center.x,
            .y = 0.0
        };
        body_set_centroid(ball, vec_add(ball_center, translate));
        Vector velocity = body_get_velocity(ball);
        velocity.x *= -1.0;
        body_set_velocity(ball, velocity);
        return 'r';
    }
    if(ball_center.y > HEIGHT - BALL_RADIUS){ //ball goes off top (bounce)
        Vector translate = {
            .x = 0.0,
            .y =  HEIGHT - BALL_RADIUS - ball_center.y
        };
        body_set_centroid(ball, vec_add(ball_center, translate));
        Vector velocity = body_get_velocity(ball);
        velocity.y *= -1.0;
        body_set_velocity(ball, velocity);
    }
    if(ball_center.y < BALL_RADIUS){ //ball goes off bottom (bounce)
        Vector translate = {
            .x = 0.0,
            .y =  BALL_RADIUS - ball_center.y
        };
        body_set_centroid(ball, vec_add(ball_center, translate));
        Vector velocity = body_get_velocity(ball);
        velocity.y *= -1.0;
        body_set_velocity(ball, velocity);
    }
    return 'n'; //'n' for "no collision"
}

void reset(Scene *scene){
    Body *paddle_one = scene_get_body(scene, 0);
    Body *paddle_two = scene_get_body(scene, 1);
    Body *ball = scene_get_body(scene, 2);

    //places the two paddles and the ball in the starting position
    body_set_centroid(paddle_one, paddle_one_center);
    body_set_centroid(paddle_two, paddle_two_center);
    body_set_centroid(ball, ball_center);
    body_set_velocity(ball, (Vector){BALL_VEL, 0});
}
