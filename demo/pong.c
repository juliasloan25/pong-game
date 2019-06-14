  #include "pong.h"

int conn;
Paddle *me;
Paddle *them;
bool networked = false;

int main(int argc, char **argv){
    Mix_Music *bgs;
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,4096);
    bgs = Mix_LoadMUS("price_is_right.mp3");
    Mix_PlayMusic(bgs, -1);

    //initialize scene and window
    int num_players;
    int num_users = 2;
    AiDifficulty difficulty = MEDIUM;
    NetType net_type;
    TTF_Init();
    Scene *scene = scene_init();
    SDL_Renderer *renderer = window_init();
    TTF_Font *font = load_font(FONT_SIZE);

    //using command line arguments for setup
    /*if(argc != 3 && argc != 4){
      //if(argc != 2 && argc != 3){
      //printf("usage: %s [1, 2, or 3 for single player, double player, and demo mode respectively] [1, 2, or 3 for easy, medium, or hard AI (single player only)]\n", argv[0]);
      printf("usage: %s [Total number of players (2, 4, 6, or 8)] [Number of users (1 or 2)] [Difficulty of AI (1, 2, or 3 for easy, medium, or hard) if applicable]\n", argv[0]);
      return 1;
    }

    int num_players = *argv[1] - '0';
    int num_users = *argv[2] - '0';*/

    num_users = start_screen(renderer, font);

    if (num_users == 4) { //networking mode
        if (strcmp(argv[1], "server") == 0) {
            printf("Waiting for a client to connect...\n");
            conn = nu_wait_client(atoi(argv[2]));
            printf("Client connected!\n");
            num_players = 2;
            net_type = SERVER;
            networked = true;
        }

        else if(strcmp(argv[1], "client") == 0){
            conn = nu_connect_server(argv[2], atoi(argv[3]));
            printf("Connected to server!\n");
            net_type = CLIENT;
            num_players = 2;
            networked = true;
        }

        if (conn < 0) {
            fprintf(stderr, "Connection Failed\n");
            exit(1);
        }
      }



    if(!networked){
        //reset window and scene
        SDL_DestroyRenderer(renderer);
        close_window();
        renderer = window_init();

        num_players = players_screen(renderer, font);

        //reset window and scene
        SDL_DestroyRenderer(renderer);
        close_window();
        renderer = window_init();

        //int num_ai = num_players - num_users;
        AiDifficulty difficulty = MEDIUM;

        //using command line arguments for setup
        /*if(num_ai != 0){
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
        }*/

        if (num_users == 3) { // demo mode
           num_users = 0;
           difficulty = MEDIUM;
       }

       if (num_users == 1) { // single player
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
       }

   }

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
    Body *ball = make_body(ball_type, BALL_CENTER);
    scene_add_body(scene, ball);

    //create paddles
    Paddle **paddles = create_paddles(scene, num_players, num_users, difficulty, polygon);

    if (networked) {
        if(net_type == SERVER){
            me = *(paddles);
            them = *(paddles + 1);
        }
        else if(net_type == CLIENT){
            me = *(paddles + 1);
            them = *(paddles);
        }
    }

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
    if(!networked){
        sdl_on_key(on_key);
    }
    else{
        sdl_on_key(net_on_key);
    }


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
            reset(scene, paddles, num_players, bounce, grav);
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
        if(networked){
            net_update();
        }
        scene_tick(scene, wait_time);
        sdl_render_scene(scene, renderer);

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
    //reset window and scene
    //scene_free(scene);
    SDL_DestroyRenderer(renderer);
    close_window();
    //scene = scene_init();
    renderer = window_init();
    end_screen(renderer, font);


    //free all elements of scene
    scene_free(scene);
    free(scores);
    for(int i = 0; i < num_players; i++){
        free(*(paddles+i));
    }
    if(networked){
        nu_close_connection(conn);
    }
    free(paddles);

    Mix_FreeMusic(bgs);
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

void net_update(){
    char *remote = nu_read_str(conn);
    if(remote != NULL){
        if(strcmp(remote, "P")){
            Vector new_paddle_vel = read_vec();
            Body *them_bod = paddle_get_body(them);
            body_set_velocity(them_bod, new_paddle_vel);
        }
    }
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
            Paddle *paddle_obj;
            if(num_players == 2){
                paddle_obj = paddle_init(paddle, paddle_center, axis, point1, point2);
                paddles[i / 2] = paddle_obj;
            }
            else{
                paddle_obj = paddle_init(paddle, paddle_center, axis, point1, point2);
                paddles[i%num_players] = paddle_obj;
            }
            if(!((i == 0 && num_users >= 1) || (i == polygon_size / 2 && num_users == 2))){
                create_ai(scene, paddle_obj, ball, difficulty);
            }
        }
    }
    list_free(polygon_shape);
    return paddles;
}

void send_vec(Vector vec){
  char str_x[100];
  char str_y[100];
  sprintf(str_x, "%f", vec.x);
  sprintf(str_y, "%f", vec.y);
  printf("%s \n", str_x);
  printf("%s \n", str_y);
  nu_send_str(conn, str_x);
  nu_send_str(conn, str_y);

}

Vector read_vec(){
    char* vec_x = nu_read_str(conn);
    char* vec_y = nu_read_str(conn);
    printf("%s \n", vec_x);
    printf("%s \n", vec_y);
    Vector new_vec = {.x = atof(vec_x), .y = atof(vec_y)};
    return new_vec;
}

void net_on_key(char key, KeyEventType type, double held_time, Scene *scene,
                                            int num_users, int num_players) {
    Body *paddle_one = paddle_get_body(me);

    //velocity to use if an arrow key was pressed
    Vector new_vel = {
        .x = 0,
        .y = PADDLE_VEL
    };

    if (type == KEY_PRESSED) {
        switch(key) {
            case W:
                //makes paddle go up if up arrow is pressed
                body_set_velocity(paddle_one, new_vel);
                nu_send_str(conn, "P");
                send_vec(new_vel);
                break;

            case S:
                //makes paddle go down if down arrow is pressed
                new_vel.y = -1.0 * new_vel.y;
                body_set_velocity(paddle_one, new_vel);
                nu_send_str(conn, "P");
                send_vec(new_vel);
                break;

            case ESCAPE:
                //ends the scene
                nu_send_str(conn, "END");
                scene_set_end(scene);
        }
    }
    else{
        //sets velocity of shooter back to zero after arrow key is released
        switch (key) {
            case W:
                body_set_velocity(paddle_one, VEC_ZERO);
                nu_send_str(conn, "P");
                send_vec(VEC_ZERO);
                break;

            case S:
                body_set_velocity(paddle_one, VEC_ZERO);
                nu_send_str(conn, "P");
                send_vec(VEC_ZERO);
                break;
        }
    }
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
                    list_free(polygon_shape);
                    return 0;
                }
                if(i == 2){
                    list_free(polygon_shape);
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
                list_free(polygon_shape);
                return i;
            }
        }
    }
    list_free(polygon_shape);
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

void reset(Scene *scene, Paddle **paddles, int num_players, Body *bounce, Body *grav){
    Body *ball = scene_get_body(scene, 1);
    body_set_centroid(ball, BALL_CENTER);
    body_set_velocity(ball, (Vector){BALL_VEL, 0});
    List *bounce_shape = body_get_shape(bounce);
    List *ball_shape =  body_get_shape(ball);
    CollisionInfo coll = find_collision(bounce_shape, ball_shape);
    list_free(bounce_shape);
    list_free(ball_shape);
    if(bounce != NULL && grav != NULL){
        if(coll.collided){
            reset_obstacles(bounce, grav, ball);
        }
    }
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
