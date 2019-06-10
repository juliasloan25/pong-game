#include "pong.h"


int main(int argc, char **argv){

    //initialize scene and window
    Scene *scene = scene_init();
    SDL_Renderer *renderer = window_init();
    TTF_Font *font = load_font(FONT_SIZE);

    int button_num = start_screen(renderer, font);

    //reset window and scene
    close_window();
    scene = scene_init();
    renderer = window_init();

    AiDifficulty difficulty = MEDIUM;
    int num_players = 2;
    int num_users = button_num;

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


    if (button_num == 3) { // demo mode
        num_users = 0;
        create_ai(scene, paddle_two, ball, MEDIUM);
        create_ai(scene, paddle_one, ball, MEDIUM);
    }

    if (button_num == 1) { // single player
        int ai_difficulty = difficulty_screen(renderer, font);
        if (ai_difficulty == 1) {
            difficulty = EASY;
        }
        else if (ai_difficulty == 2) {
            difficulty = MEDIUM;
        }
        else if (ai_difficulty == 3) {
            difficulty = HARD;
        }
        create_ai(scene, paddle_one, ball, difficulty);
    }

    //create bouncing collision between paddles and ball
    create_physics_collision(scene, ELASTICITY, paddle_one, ball);
    create_physics_collision(scene, ELASTICITY, paddle_two, ball);

    sdl_on_key(on_key); //handles key inputs

    //initialize scores and AI timer
    int left_score = 0,  //score of player with left paddle
        right_score = 0; //score of player with right paddle
    char left_score_str[4];
    char right_score_str[4];
    snprintf(left_score_str, 10, "%d", left_score);
    snprintf(right_score_str, 10, "%d", right_score);
    double ai_timer = 0;


    SDL_Rect *rect_right = make_rect(RIGHT_SCORE_X, SCORE_Y, SCORE_WIDTH,
                                TEXT_HEIGHT);
    SDL_Rect *rect_left = make_rect(LEFT_SCORE_X, SCORE_Y, SCORE_WIDTH,
                                TEXT_HEIGHT);

    while(!sdl_is_done(scene, num_players, num_users)) {
        double wait_time = time_since_last_tick();
        ai_timer += wait_time;
        //checks if paddle or ball has hit walls
        char ball_hit_side = move_if_offscreen(paddle_one, paddle_two, ball);

        if(ball_hit_side == 'l'){ //add point to right player
            right_score++;
            right_score_str[0] = '\0';
            snprintf(right_score_str, 10, "%d", right_score);

            reset(scene);
            //display the updated right score
            display_text(renderer, right_score_str, font,
                            rect_right, BLACK);
            sdl_render_scene(scene, renderer);
        }
        else if (ball_hit_side == 'r') { //add point to left player
            left_score++;
            left_score_str[0] = '\0';
            snprintf(left_score_str, 10, "%d", left_score);

            reset(scene);
            //display the updated right score
            display_text(renderer, left_score_str, font,
                            rect_left, BLACK);
            sdl_render_scene(scene, renderer);
        }

        display_text(renderer, right_score_str, font,
                        rect_right, BLACK);
        display_text(renderer, left_score_str, font,
                        rect_left, BLACK);

        //render and update scene at every tick
        scene_tick(scene, wait_time);
        sdl_render_scene(scene, renderer);

        // end game if either score reaches 10
        if (right_score >= 10 || left_score >= 10 || scene_get_end(scene)){
            //reset window and scene
            close_window();
            scene = scene_init();
            renderer = window_init();

            button_num = end_screen(renderer, font);
            /*if (button_num == 0) {
                break;
            }
            else {
                right_score = 0;
                left_score = 0;
                right_score_str[0] = '\0';
                left_score_str[0] = '\0';
                //reset window and scene
                close_window();
                reset(scene);
                scene = scene_init();
                renderer = window_init();

                button_num = start_screen(renderer, font);
            }*/
            SDL_Delay(2000);
            close_window();
            break;
        }
    }

    //SDL_Delay(2000);

    //free all elements of scene and the renderer
    free(rect_left);
    free(rect_right);
    scene_free(scene);
    SDL_DestroyRenderer(renderer);
    TTF_CloseFont(font); //CHANGE IN NON-SIMPLE
    close_window();
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

    //free(type);
    return body;
}

void on_key(char key, KeyEventType type, double held_time, Scene *scene,
                                            int num_users, int num_players) {
    Body *paddle_one = scene_get_body(scene, 0);
    Body *paddle_two = scene_get_body(scene, 1);

    //velocity to use if an arrow key was pressed
    Vector new_vel = {
        .x = 0,
        .y = PADDLE_VEL
    };

    if (type == KEY_PRESSED) {
        switch(key) {
            case W:
                //makes paddle go up if up arrow is pressed
                if(num_users >= 1){
                    body_set_velocity(paddle_one, new_vel);
                }
                break;

            case S:
                //makes paddle go down if down arrow is pressed
                new_vel.y = -1.0 * new_vel.y;
                if(num_users >= 1){
                    body_set_velocity(paddle_one, new_vel);
                }
                break;

            case UP_ARROW:
                //makes paddle go up if up arrow is pressed
                if(num_users == 2){
                    body_set_velocity(paddle_two, new_vel);
                }
                break;

            case DOWN_ARROW:
                //makes paddle go down if down arrow is pressed
                new_vel.y = -1.0 * new_vel.y;
                if(num_users == 2){
                    body_set_velocity(paddle_two, new_vel);
                }
                break;

            case ESCAPE:
                //ends the scene
                scene_set_end(scene);
        }
    }
    else{
        //sets velocity of shooter back to zero after arrow key is released
        switch (key) {
            case W:
                if(num_users >= 1){
                    body_set_velocity(paddle_one, VEC_ZERO);
                }
                break;

            case S:
                if(num_users >= 1){
                    body_set_velocity(paddle_one, VEC_ZERO);
                }
                break;
            case UP_ARROW:
                if(num_users == 2){
                    body_set_velocity(paddle_two, VEC_ZERO);
                }
                break;

            case DOWN_ARROW:
                if(num_users == 2){
                    body_set_velocity(paddle_two, VEC_ZERO);
                }
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
