#include "pong.h"
#include <SDL2/SDL.h>


const double WIDTH = 800; //screen width
const double HEIGHT = 800; //screen height
const double BALL_RADIUS = 10.0; //radius of pong ball
const double GRAV_RADIUS = 15.0;
const double PADDLE_HEIGHT = 100.0; //width of the pong paddle
const double PADDLE_WIDTH = 30.0; // height of the pong paddle
const double BOUNCE_HEIGHT = 10.0;
const double BOUNCE_WIDTH = 80.0;
const double COLOR_INTERVAL = 2.0;
const double OBSTACLE_INTERVAL = 7.0;
const double ELASTICITY = 1; //elasticity of collisions
const double PADDLE_VEL = 600.0; //velocity that the paddle can go
const double MASS = 50.0; //mass of non-infinity mass objects
const double BALL_VEL = 700.0; // initial velocity of ball
const RGBColor PADDLE_AI_COLOR = {
    .r = 0,
    .g = 0,
    .b = 0
}; //black paddle color
const RGBColor PADDLE_USER_COLOR = {
    .r = 1,
    .g = 0,
    .b = 0
}; //color of the pong ball
const RGBColor BOUNCE_COLOR = {
    .r = 0,
    .g = 0,
    .b = 1
};
const RGBColor GRAV_COLOR = {
    .r = 0,
    .g = 1,
    .b = 0
};
const RGBColor POLYGON_COLOR = {
    .r = .9,
    .g = .9,
    .b = .9
};
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
const double G = 8000.0; //gravity for obstacle


int main(int argc, char **argv){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_Music *bgs;
    Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,4096)
    bgs = Mix_LoadMUS("price_is_right.mp3");
    Mix_PlayMusic(bgs, -1);
    if(argc != 3 && argc != 4){
      //if(argc != 2 && argc != 3){
      //printf("usage: %s [1, 2, or 3 for single player, double player, and demo mode respectively] [1, 2, or 3 for easy, medium, or hard AI (single player only)]\n", argv[0]);
      printf("usage: %s [Total number of players (2, 4, 6, or 8)] [Number of users (1 or 2)] [Difficulty of AI (1, 2, or 3 for easy, medium, or hard) if applicable]\n", argv[0]);
      return 1;
    }

    int num_players = *argv[1] - '0';
    int num_users = *argv[2] - '0';
    int num_ai = num_players - num_users;
    AiDifficulty difficulty = MEDIUM;
    if(num_ai != 0){
        switch(*argv[2]){
            case '1':
                difficulty = EASY;
                break;
            case '2':
                difficulty = MEDIUM;
                break;
            case '3':
                difficulty = HARD;
                break;
        }
    }

    //initialize scene and window
    Scene *scene = scene_init();

    BodyType *polygon_type = malloc(sizeof(BodyType));
    double rotation_angle = 0;
    if(num_players <= 4){
        *(polygon_type) = SQUARE;
        rotation_angle = M_PI / 4;
    }
    else if(num_players == 6){
        *(polygon_type) = HEXAGON;
        rotation_angle = M_PI / 6;
    }
    else{
        *(polygon_type) = OCTOGON;
        rotation_angle = M_PI / 8;
    }
    Body *polygon = make_body(polygon_type, (Vector) {WIDTH/2, HEIGHT/2});
    body_set_rotation(polygon, rotation_angle); //to place polygon upright
    scene_add_body(scene, polygon);

    //create ball
    BodyType *ball_type = malloc(sizeof(BodyType));
    *(ball_type) = BALL;
    Body *ball = make_body(ball_type, ball_center);
    scene_add_body(scene, ball);

    //create paddles
    Body **paddles = create_paddles(scene, num_players, num_users, difficulty);

    //create obstacles
    BodyType *bounce_type = malloc(sizeof(BodyType));
    *(bounce_type) = BOUNCE;
    Body *bounce = make_body(bounce_type, VEC_ZERO);
    scene_add_body(scene, bounce);

    BodyType *grav_type = malloc(sizeof(BodyType));
    *(grav_type) = GRAVITY;
    Body *grav = make_body(grav_type, VEC_ZERO);
    scene_add_body(scene, grav);

    reset_obstacles(bounce, grav);

    //create forces between obstacles and ball
    create_physics_collision(scene, ELASTICITY, bounce, ball);
    create_newtonian_gravity(scene, G, ball, grav, false);

     /*if(MOUSE_MOVED){
        Body * paddle_one  = scene_get_body(scene,0);
        body_set_centroid(paddle_one, (Vector){paddle_one_center.x, return_mouse_y_position(scene)});
      }*/

    window_init();
    sdl_on_key(on_key);

    //initialize scores
    int *scores = malloc(sizeof(int) * num_players);
    for(int i = 0; i < num_players; i++){
        scores[i] = 0;
    }
    double ai_timer = 0;
    double obstacle_timer = 0;
    double color_index = 0;
    double color_timer = 0;
    while(!sdl_is_done(scene, num_users)) {
        double wait_time = time_since_last_tick();
        ai_timer += wait_time;
        obstacle_timer += wait_time;
        color_timer += wait_time;
        //checks if paddle or ball has hit walls
        int ball_hit_side = move_if_offscreen(scene_get_body(scene, 2), scene_get_body(scene, 3), ball);
        if(ball_hit_side != -1){
            scores[ball_hit_side]++;
            reset(scene);
        }

        if(obstacle_timer >= OBSTACLE_INTERVAL){
            reset_obstacles(bounce, grav);
            obstacle_timer = 0;
        }

        if(color_timer >= COLOR_INTERVAL){
            color_index++;
            body_set_color(ball, get_color(color_index));
            color_timer = 0;
        }

        //render and update scene at every tick
        scene_tick(scene, wait_time);
        sdl_render_scene(scene);

        // end game if either score reaches 10
        for(int j = 0; j < num_players; j++){
            if(scores[j] >= 10){
                scene_set_end(scene);
            }
        }

        if(scene_get_end(scene)){
            break;
        }
        //display the current scores on-screen
        //display_text(itoa(right_score), 30);
        //display_text(itoa(left_score), 30);
    }
    //free all elements of scene
    scene_free(scene);
    free(scores);
    Mix_FreeMusic(bgm);
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
    double mass = INFINITY;
    double radius;
    double width;
    double height;
    if (*(type) == BALL) {
        mass = MASS;
        points = 2000; //large number of points to create circle
        color = get_color(0);
        velocity.x = BALL_VEL;
        velocity.y = BALL_VEL;
        radius = BALL_RADIUS;
    }
    if(*(type) == PADDLE_USER){
        width = PADDLE_WIDTH;
        height = PADDLE_HEIGHT;
        points = 4; //to form rectangle
        color = PADDLE_USER_COLOR;
    }
    if(*(type) == PADDLE_AI){
        width = PADDLE_WIDTH;
        height = PADDLE_HEIGHT;
        points = 4; //to form rectangle
        color = PADDLE_AI_COLOR;
    }
    if(*(type) == BOUNCE){
        width = BOUNCE_WIDTH;
        height = BOUNCE_HEIGHT;
        points = 4; //to form rectangle
        color = BOUNCE_COLOR;
    }
    if (*(type) == GRAVITY) {
        mass = MASS * 10;
        points = 2000; //large number of points to create circle
        color = GRAV_COLOR;
        radius = GRAV_RADIUS;
    }
    if (*(type) == SQUARE) {
        points = 4;
        color = POLYGON_COLOR;
        radius = HEIGHT * sqrt(2.0) / 2.0;
    }
    if (*(type) == HEXAGON) {
        points = 6;
        color = POLYGON_COLOR;
        radius = HEIGHT / 2.0;
    }
    if (*(type) == OCTOGON) {
        points = 8;
        color = POLYGON_COLOR;
        radius = HEIGHT / 2.0;
    }
    List *shape = list_init(points, free, (EqualFunc)vec_equal_v);

    //construct paddle or bounce obstacle
    if (*(type) == PADDLE_USER || *(type) == PADDLE_AI || *(type) == BOUNCE){
        //add four points
        Vector translate_up_left = {
            .x = -1.0 * width / 2.0,
            .y = height / 2.0
        };
        Vector *vec1 = malloc(sizeof(Vector));
        *vec1 = vec_add(translate_up_left, center);
        list_add(shape, vec1);

        Vector translate_down_left = {
            .x = -1.0 * width / 2.0,
            .y = -1.0 * height / 2.0
        };
        Vector *vec2 = malloc(sizeof(Vector));
        *vec2 = vec_add(translate_down_left, center);
        list_add(shape, vec2);

        Vector translate_down_right = {
            .x = width / 2.0,
            .y = -1.0 * height / 2.0
        };
        Vector *vec3 = malloc(sizeof(Vector));
        *vec3 = vec_add(translate_down_right, center);
        list_add(shape, vec3);

        Vector translate_up_right = {
            .x = width / 2.0,
            .y = height / 2.0
        };
        Vector *vec4 = malloc(sizeof(Vector));
        *vec4 = vec_add(translate_up_right, center);
        list_add(shape, vec4);
    }
    else { //construct the ball or gravity obstacle
        double angle = (2 * M_PI / points);
        for (int i =0; i < points; i++){
            Vector *vec = malloc(sizeof(Vector));
            vec->x = radius *cos(angle*i);
            vec->y = radius *sin(angle*i);
            list_add(shape, (void*)vec);
        }
    }

    body = body_init_with_info(shape, mass, color, (void*)type, (FreeFunc)free);
    body_set_centroid(body, center);
    body_set_velocity(body, velocity);

    return body;
}

Body ** create_paddles(Scene *scene, int num_players, int num_users, AiDifficulty difficulty){
    Body **paddles = malloc(sizeof(Body *) * num_players);
    Body *ball = scene_get_body(scene, 1);
    for(int i = 0; i < num_players; i++){
        BodyType *paddle_type = malloc(sizeof(BodyType));
        if(i < num_users){
            *(paddle_type) = PADDLE_USER;
        }
        else{
            *(paddle_type) = PADDLE_AI;
        }
        Vector paddle_center;
        if(i == 0){
            paddle_center = paddle_one_center;
        }
        else{
            paddle_center = paddle_two_center;
        }
        Body *paddle = make_body(paddle_type, paddle_center);
        scene_add_body(scene, paddle);
        create_physics_collision(scene, ELASTICITY, paddle, ball);
        if(i >= num_users){
            create_ai(scene, paddle, ball, difficulty);
        }
        paddles[0] = paddle;
    }
    return paddles;
}

void on_key(char key, KeyEventType type, double held_time, Scene *scene,
                                                        int num_users) {
    Body *paddle_one = scene_get_body(scene, 2);
    Body *paddle_two = scene_get_body(scene, 3);

    //velocity to use if an arrow key was pressed
    Vector new_vel = {
        .x = 0,
        .y = PADDLE_VEL
    };

    if (type == KEY_PRESSED) {
        switch(key) {
            if(num_users >= 1){
                case UP_ARROW:
                    //makes paddle go up if up arrow is pressed
                    body_set_velocity(paddle_one, new_vel);
                    break;

                case DOWN_ARROW:
                    //makes paddle go down if down arrow is pressed
                    new_vel.y = -1.0 * new_vel.y;
                    body_set_velocity(paddle_one, new_vel);
                    break;
            }
            if(num_users == 2){
                case W:
                    //makes paddle go up if up arrow is pressed
                    body_set_velocity(paddle_two, new_vel);
                    break;

                case S:
                    //makes paddle go down if down arrow is pressed
                    new_vel.y = -1.0 * new_vel.y;
                    body_set_velocity(paddle_two, new_vel);
                    break;
            }

            case ESCAPE:
                //ends the scene
                scene_set_end(scene);

            case SPACE:
                //reset the scene
                reset(scene);

        }
    }
    else{
        //sets velocity of shooter back to zero after arrow key is released
        switch (key) {
            if(num_users >= 1){
                case UP_ARROW:
                    body_set_velocity(paddle_one, VEC_ZERO);
                    break;

                case DOWN_ARROW:
                    body_set_velocity(paddle_one, VEC_ZERO);
                    break;
            }
            if(num_users == 2){
                case W:
                    body_set_velocity(paddle_two, VEC_ZERO);
                    break;

                case S:
                    body_set_velocity(paddle_two, VEC_ZERO);
                    break;
            }
        }
    }
}

int move_if_offscreen(Body *paddle_one, Body *paddle_two, Body *ball){
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

    if(ball_center.x < BALL_RADIUS){ //ball goes off left side (return 0)
        Vector translate = {
            .x = BALL_RADIUS - ball_center.x,
            .y = 0.0
        };
        body_set_centroid(ball, vec_add(ball_center, translate));
        Vector velocity = body_get_velocity(ball);
        velocity.x *= -1.0;
        body_set_velocity(ball, velocity);
        return 0;
    }
    if(ball_center.x > WIDTH - BALL_RADIUS){ //ball goes off right side (return 1)
        Vector translate = {
            .x = WIDTH - BALL_RADIUS - ball_center.x,
            .y = 0.0
        };
        body_set_centroid(ball, vec_add(ball_center, translate));
        Vector velocity = body_get_velocity(ball);
        velocity.x *= -1.0;
        body_set_velocity(ball, velocity);
        return 1;
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
    return -1;
}

void reset(Scene *scene){
    Body *paddle_one = scene_get_body(scene, 2);
    Body *paddle_two = scene_get_body(scene, 3);
    Body *ball = scene_get_body(scene, 1);

    //places the two paddles and the ball in the starting position
    body_set_centroid(paddle_one, paddle_one_center);
    body_set_centroid(paddle_two, paddle_two_center);
    body_set_centroid(ball, ball_center);
    body_set_velocity(ball, (Vector){BALL_VEL, 0});
}

void reset_obstacles(Body *bounce, Body *grav){
    Vector bounce_center = {
        .x = rand() / (RAND_MAX / (WIDTH / 2.0 + 1) + 1) + WIDTH / 4.0,
        .y = rand() / (RAND_MAX / (HEIGHT / 2.0 + 1) + 1) + HEIGHT / 4.0
    };
    body_set_centroid(bounce, bounce_center);

    Vector grav_center = {
        .x = rand() / (RAND_MAX / (WIDTH / 2.0 + 1) + 1) + WIDTH / 4.0,
        .y = rand() / (RAND_MAX / (HEIGHT / 2.0 + 1) + 1) + HEIGHT / 4.0
    };
    body_set_centroid(grav, grav_center);

    int degree = rand() / (RAND_MAX / (360 + 1) + 1);
    double angle = degree * (M_PI/ 180.0);
    body_set_rotation(bounce, angle);

    CollisionInfo coll = find_collision(body_get_shape(bounce), body_get_shape(grav));
    if(coll.collided){
        reset_obstacles(bounce, grav);
    }
}

RGBColor get_color(int shape_index){
    //Determines how quickly colors repeat
    double frequency = .3;

    //Use sin functions with a shift to rotate through colors of the rainbow
    RGBColor color;
    color.r = fabs(sin(frequency * shape_index));
    color.g = fabs(sin(frequency * shape_index + (M_PI / 3.0)));
    color.b = fabs(sin(frequency * shape_index + (2.0 * M_PI / 3.0)));

    return color;
}
