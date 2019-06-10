#include "pong.h"


int main(int argc, char **argv){

    //initialize scene and window
    Scene *scene = scene_init();
    SDL_Renderer *renderer = window_init();
    TTF_Font *font = load_font(FONT_SIZE);

    // TESTING TEXT
    start_screen(renderer, font);
    scene_free(scene);
    SDL_DestroyRenderer(renderer);

    SDL_Delay(2000);
    close_window(); //destroy window
    scene = scene_init();
    renderer = window_init();

    // TESTING TEXT
    end_screen(renderer, font);

    SDL_Delay(2000);
    close_window(); //destroy window

    //free(rect);
    scene_free(scene);
    SDL_DestroyRenderer(renderer);
    TTF_CloseFont(font); //CHANGE IN NON-EXAMPLE
    TTF_Quit();
    SDL_Quit();
    return 1;
}

SDL_Renderer *window_init(){
    Vector vec_min = VEC_ZERO;
    Vector vec_max = {
        .x = WIDTH,
        .y = HEIGHT
    };
    return sdl_init(vec_min, vec_max);
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
    Vector paddle_one_loc = body_get_centroid(paddle_one);
    Vector paddle_two_loc = body_get_centroid(paddle_two);
    Vector ball_center_loc = body_get_centroid(ball);

    if(paddle_one_loc.y <  PADDLE_HEIGHT / 2.0){
        Vector new_center = {
            .x = PADDLE_WIDTH / 2.0,
            .y = PADDLE_HEIGHT / 2.0
        };
        body_set_centroid(paddle_one, new_center);
    }
    if(paddle_one_loc.y > (HEIGHT - PADDLE_HEIGHT / 2.0)){
        Vector new_center = {
            .x = PADDLE_WIDTH / 2.0,
            .y = HEIGHT - PADDLE_HEIGHT / 2.0
        };
        body_set_centroid(paddle_one, new_center);
    }

    if(paddle_two_loc.y <  PADDLE_HEIGHT / 2.0){
        Vector new_center = {
            .x = WIDTH - PADDLE_WIDTH / 2.0,
            .y = PADDLE_HEIGHT / 2.0
        };
        body_set_centroid(paddle_two, new_center);
    }
    if(paddle_two_loc.y > (HEIGHT - PADDLE_HEIGHT / 2.0)){
        Vector new_center = {
            .x = WIDTH - PADDLE_WIDTH / 2.0,
            .y = HEIGHT - PADDLE_HEIGHT / 2.0
        };
        body_set_centroid(paddle_two, new_center);
    }

    if(ball_center_loc.x < BALL_RADIUS){ //ball goes off left side (return 'l')
        Vector translate = {
            .x = BALL_RADIUS - ball_center_loc.x,
            .y = 0.0
        };
        body_set_centroid(ball, vec_add(ball_center_loc, translate));
        Vector velocity = body_get_velocity(ball);
        velocity.x *= -1.0;
        body_set_velocity(ball, velocity);
        return 'l';
    }
    if(ball_center_loc.x > WIDTH - BALL_RADIUS){ //ball goes off right side (return 'r')
        Vector translate = {
            .x = WIDTH - BALL_RADIUS - ball_center_loc.x,
            .y = 0.0
        };
        body_set_centroid(ball, vec_add(ball_center_loc, translate));
        Vector velocity = body_get_velocity(ball);
        velocity.x *= -1.0;
        body_set_velocity(ball, velocity);
        return 'r';
    }
    if(ball_center_loc.y > HEIGHT - BALL_RADIUS){ //ball goes off top (bounce)
        Vector translate = {
            .x = 0.0,
            .y =  HEIGHT - BALL_RADIUS - ball_center_loc.y
        };
        body_set_centroid(ball, vec_add(ball_center_loc, translate));
        Vector velocity = body_get_velocity(ball);
        velocity.y *= -1.0;
        body_set_velocity(ball, velocity);
    }
    if(ball_center_loc.y < BALL_RADIUS){ //ball goes off bottom (bounce)
        Vector translate = {
            .x = 0.0,
            .y =  BALL_RADIUS - ball_center_loc.y
        };
        body_set_centroid(ball, vec_add(ball_center_loc, translate));
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
