#include "pong.h"

const double WIDTH = 800; //screen width
const double HEIGHT = 800; //screen height
const double BALL_RADIUS = 10.0; //radius of pong ball
const double GRAV_RADIUS = 15.0;
const double PADDLE_HEIGHT = 100.0; //width of the pong paddle
const double PADDLE_WIDTH = 20.0; // height of the pong paddle
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
const double G = 7000.0; //gravity for obstacle


int main(int argc, char **argv){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_Music *bgs;
    Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,4096);
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
        rotation_angle = 3 * M_PI / 4;
    }
    else if(num_players == 6){
        *(polygon_type) = HEXAGON;
        rotation_angle = 5 * M_PI / 6;
    }
    else{
        *(polygon_type) = OCTOGON;
        rotation_angle = 7 * M_PI / 8;
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
    Paddle **paddles = create_paddles(scene, num_players, num_users, difficulty, polygon);

    //create obstacles
    BodyType *bounce_type = malloc(sizeof(BodyType));
    *(bounce_type) = BOUNCE;
    Body *bounce = make_body(bounce_type, VEC_ZERO);
    scene_add_body(scene, bounce);

    BodyType *grav_type = malloc(sizeof(BodyType));
    *(grav_type) = GRAVITY;
    Body *grav = make_body(grav_type, VEC_ZERO);
    scene_add_body(scene, grav);

    reset_obstacles(bounce, grav, ball);

    //create forces between obstacles and ball
    create_physics_collision(scene, ELASTICITY, bounce, ball);
    create_newtonian_gravity(scene, G, ball, grav, false);

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
    while(!sdl_is_done(scene, num_users, num_players)) {
        double wait_time = time_since_last_tick();
        ai_timer += wait_time;
        obstacle_timer += wait_time;
        color_timer += wait_time;
        //checks if paddle or ball has hit walls
        int ball_hit = ball_hit_side(ball, polygon, num_players);
        if(ball_hit != -1){
            scores[ball_hit]++;
            reset(scene, paddles, num_players);
            reset_obstacles(bounce, grav, ball);
        }

        paddle_hit_side(paddles, num_players);

        if(obstacle_timer >= OBSTACLE_INTERVAL){
            reset_obstacles(bounce, grav, ball);
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
    free(paddles);
    Mix_FreeMusic(bgs);
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

Paddle **create_paddles(Scene *scene, int num_players, int num_users, AiDifficulty difficulty, Body *polygon){
    Paddle **paddles = malloc(sizeof(Paddle *) * num_players);
    Body *ball = scene_get_body(scene, 1);
    List *polygon_shape = body_get_shape(polygon);
    size_t polygon_size = list_size(polygon_shape);
    double angle = 2 * M_PI / polygon_size;
    for(size_t i = 0; i < polygon_size; i++){
        if(num_players != 2 || i % num_players == 0){
            BodyType *paddle_type = malloc(sizeof(BodyType));
            if((i == 0 && num_users >= 1) || (i == polygon_size / 2 && num_users == 2)){
                *(paddle_type) = PADDLE_USER;
            }
            else{
                *(paddle_type) = PADDLE_AI;
            }

            Vector point1 = *(Vector *)list_get(polygon_shape, i);
            size_t j = i + 1;
            if(j == polygon_size){
                j = 0;
            }
            Vector point2 = *(Vector *)list_get(polygon_shape, j);
            Vector paddle_center = vec_multiply(.5, vec_add(point1, point2));
            Vector axis = vec_unit(vec_subtract(point1, point2));
            /*Vector perp = {
                .x = 1,
                .y = axis.x / axis.y
            };
            printf("%f %f\n", perp.x, perp.y);
            perp = vec_multiply(PADDLE_WIDTH / (2 * vec_magnitude(perp)), perp);
            paddle_center = vec_add(paddle_center, perp);
            printf("%f %f\n", paddle_center.x, paddle_center.y);*/

            /*Vector length = vec_multiply(PADDLE_HEIGHT / 2, axis);
            Vector vec = vec_subtract(point1, length);
            Vector vec2 = vec_add(point2, length);

            printf("point1 %f %f\n", point1.x, point1.y);
            printf("point2 %f %f\n", point2.x, point2.y);
            printf("length %f %f\n", length.x, length.y);
            printf("vec %f %f\n", vec.x, vec.y);
            printf("vec2 %f %f\n", vec2.x, vec2.y);
            double distance = vec_dist(point1, point2);
            double pos_check = vec_dist(vec_add(paddle_center, length), point2);
            double neg_check = vec_dist(vec_subtract(paddle_center, length), point1);
            printf("%f %f %f\n", distance, pos_check, neg_check);*/

            Body *paddle = make_body(paddle_type, paddle_center);
            body_set_rotation(paddle, angle * i);
            scene_add_body(scene, paddle);
            create_physics_collision(scene, ELASTICITY, paddle, ball);
            /*if(i >= num_users){
                create_ai(scene, paddle, ball, difficulty);
            }*/
            if(num_players == 2){
                paddles[i / 2] = paddle_init(paddle, paddle_center, axis, point1, point2);
            }
            else{
                paddles[i] = paddle_init(paddle, paddle_center, axis, point1, point2);
            }
        }
    }
    free(polygon_shape);
    return paddles;
}

void on_key(char key, KeyEventType type, double held_time, Scene *scene,
                                            int num_users, int num_players) {
    Body *paddle_one = scene_get_body(scene, 2);
    Body *paddle_two = scene_get_body(scene, 2 + num_players / 2);

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

int ball_hit_side(Body *ball, Body *polygon, int num_players){
    Vector ball_cent = body_get_centroid(ball);
    List *polygon_shape = body_get_shape(polygon);
    size_t polygon_size = list_size(polygon_shape);
    for(size_t i = 0; i < polygon_size; i++){
        Vector point1 = *(Vector *)list_get(polygon_shape, i);
        size_t j = i + 1;
        if(j == polygon_size){
            j = 0;
        }
        Vector point2 = *(Vector *)list_get(polygon_shape, j);
        Vector perp = vec_negate(vec_unit((Vector) {point2.y - point1.y,point2.x - point1.x}));
        ball_cent = vec_add(perp, ball_cent);
        double cross = (point2.x - point1.x)*(ball_cent.y - point1.y) -
                                (point2.y - point1.y)*(ball_cent.x - point1.x);
        if(cross < 0){
            if(num_players == 2){
                if(i == 0){
                    return 0;
                }
                if(i == 2){
                    return 1;
                }
                if(i == 1){
                    Vector translate = {
                        .x = 0.0,
                        .y =  BALL_RADIUS - ball_cent.y
                    };
                    body_set_centroid(ball, vec_add(ball_cent, translate));
                    Vector velocity = body_get_velocity(ball);
                    velocity.y *= -1.0;
                    body_set_velocity(ball, velocity);
                }
                if(i == 3){
                    Vector translate = {
                        .x = 0.0,
                        .y =  HEIGHT - BALL_RADIUS - ball_cent.y
                    };
                    body_set_centroid(ball, vec_add(ball_cent, translate));
                    Vector velocity = body_get_velocity(ball);
                    velocity.y *= -1.0;
                    body_set_velocity(ball, velocity);
                }
            }
            else{
                return i;
            }
        }
    }
    free(polygon_shape);
    return -1;
}

void paddle_hit_side(Paddle **paddles, int num_players){
    for(int i = 0; i < num_players; i++){
        Paddle *paddle = paddles[i];
        Body *body = paddle_get_body(paddle);
        Vector center = body_get_centroid(body);
        Vector axis = paddle_get_axis(paddle);
        Vector length = vec_multiply(PADDLE_HEIGHT / 2, axis);
        Vector positive = paddle_get_positive_boundary(paddle);
        Vector negative = paddle_get_negative_boundary(paddle);
        double distance = vec_dist(positive, negative);
        double pos_check = vec_dist(vec_add(center, length), negative);
        double neg_check = vec_dist(vec_subtract(center, length), positive);
        if(pos_check > distance) {
            Vector vec = vec_subtract(positive, length);
            body_set_centroid(body, vec);
            //printf("length %f %f\n", length.x, length.y);
            //printf("axis %f %f\n", axis.x, axis.y);
            //printf("vec1 %f %f\n", vec.x, vec.y);
            //printf("positive %f %f\n", positive.x, positive.y);
        }
        if(neg_check > distance) {
            Vector vec = vec_add(negative, length);
            body_set_centroid(body, vec);
            //printf("vec2 %f %f\n", vec.x, vec.y);
            //printf("negative %f %f\n", negative.x, negative.y);
        }
    }
}

void reset(Scene *scene, Paddle **paddles, int num_players){
    Body *ball = scene_get_body(scene, 1);
    body_set_centroid(ball, ball_center);
    body_set_velocity(ball, (Vector){BALL_VEL, 0});

    for(int i = 0; i < num_players; i++){
        Paddle *paddle = paddles[i];
        body_set_centroid(paddle_get_body(paddle), paddle_get_initial_center(paddle));
    }
}

void reset_obstacles(Body *bounce, Body *grav, Body *ball){
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
    List *bounce_shape = body_get_shape(bounce);
    List *grav_shape =  body_get_shape(grav);
    List *ball_shape =  body_get_shape(ball);
    CollisionInfo coll1 = find_collision(bounce_shape, grav_shape);
    CollisionInfo coll2 = find_collision(bounce_shape, ball_shape);
    list_free(bounce_shape);
    list_free(grav_shape);
    list_free(ball_shape);
    if(coll1.collided || coll2.collided){
        reset_obstacles(bounce, grav, ball);
    }
}

RGBColor get_color(int shape_index){
    //Determines how quickly colors repeat
    double frequency = .3;

    //Use sin functions with a shift to rotate through colors of the rainbow
    RGBColor color;
    color.r = fabs(sin(frequency * shape_index + (M_PI / 3.0)));
    color.g = fabs(sin(frequency * shape_index));
    color.b = fabs(sin(frequency * shape_index + (2.0 * M_PI / 3.0)));

    return color;
}
