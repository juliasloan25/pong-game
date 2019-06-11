#include "sdl_wrapper.h"

#define WINDOW_TITLE "CS 3"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MS_PER_S 1e3

/**
 * The coordinate at the center of the screen. scene coordinates
 * don't make global bc want to change if window resizes
 */
Vector center;
/**
 * The coordinate difference from the center to the top right corner.
 */
Vector max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
// SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
KeyHandler key_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;
/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key) {
    switch (key) {
        case SDLK_LEFT: return LEFT_ARROW;
        case SDLK_UP: return UP_ARROW;
        case SDLK_RIGHT: return RIGHT_ARROW;
        case SDLK_DOWN: return DOWN_ARROW;
        case SDLK_SPACE: return SPACE;
        case SDLK_ESCAPE: return ESCAPE;
        //case SDLK_W: return W;
        //case SDLK_S: return S;
        default:
            // Only process 7-bit ASCII characters
            return key == (SDL_Keycode) (char) key ? key : '\0';
    }
}

SDL_Renderer *sdl_init(Vector min, Vector max) {
    // Check parameters
    assert(min.x < max.x);
    assert(min.y < max.y);

    center = vec_multiply(0.5, vec_add(min, max)),
    max_diff = vec_subtract(max, center);
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );

    if (TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(1);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    return renderer;
}

bool sdl_is_done(Scene *scene, int num_players, int num_users) {
    SDL_Event *event = malloc(sizeof(*event));
    //int x_pos;
    //int y_pos;
    assert(event);
    while (SDL_PollEvent(event)) {
        switch (event->type) {
            case SDL_QUIT:
                free(event);
                return true;
            /* case SDL_MOUSEMOTION:
              x_pos = (int) event->motion.x;
              y_pos = (int) event->motion.y;
                break;
              */
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                // Skip the keypress if no handler is configured
                // or an unrecognized key was pressed
                if (!key_handler) break;
                char key = get_keycode(event->key.keysym.sym);
                if (!key) break;

                double timestamp = event->key.timestamp;
                if (!event->key.repeat) {
                    key_start_timestamp = timestamp;
                }
                KeyEventType type =
                    event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
                double held_time =
                    (timestamp - key_start_timestamp) / MS_PER_S;
                key_handler(key, type, held_time, scene, num_players, num_users);
                break;
        }
    }
    free(event);
    return false;
}


void sdl_clear(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

Vector sdl_get_window_center() {
    int *width = malloc(sizeof(*width)),
        *height = malloc(sizeof(*height));
    assert(width);
    assert(height);

    // Fill width and height with appropriate values
    SDL_GetWindowSize(window, width, height);
    Vector window_center = {
        .x = *width / 2.0,
        .y = *height / 2.0
    };

    free(width);
    free(height);

    return window_center;
}

double sdl_get_scale(double x, double y) {
    double x_scale = x / max_diff.x, //center should be pixel coords (window)
           y_scale = y / max_diff.y;

    return x_scale < y_scale ? x_scale : y_scale; //take min of scales to find constraining coordinate
}

void sdl_scale_rect(SDL_Rect *rect, SDL_Renderer *renderer) {
    Vector window_center = sdl_get_window_center();
    double scale = sdl_get_scale(window_center.x, window_center.y);
    Vector rect_pos = {
        rect->x,
        rect->y
    };
    Vector pos_from_center = vec_multiply(scale, vec_subtract(rect_pos, center));

    rect->x = window_center.x + pos_from_center.x; //wrong
    rect->y = (window_center.y + pos_from_center.y) * -1.0; //wrong
    rect->w *= scale;
    rect->h *= scale;
}

void sdl_draw_polygon(List *points, RGBColor color, SDL_Renderer *renderer) {
    // Check parameters
    size_t n = list_size(points);
    assert(n >= 3);
    assert(0 <= color.r && color.r <= 1);
    assert(0 <= color.g && color.g <= 1);
    assert(0 <= color.b && color.b <= 1);

    Vector window_center = sdl_get_window_center();
    double scale = sdl_get_scale(window_center.x, window_center.y);

    // Convert each vertex to a point on screen
    short *x_points = malloc(sizeof(*x_points) * n),
          *y_points = malloc(sizeof(*y_points) * n);
    assert(x_points);
    assert(y_points);
    for (size_t i = 0; i < n; i++) {
        Vector *vertex = list_get(points, i);
        Vector pos_from_center =
            vec_multiply(scale, vec_subtract(*vertex, center)); //converting scene to pixel coords
        // Flip y axis since positive y is down on the screen
        x_points[i] = round(window_center.x + pos_from_center.x); //pixel coordinates
        y_points[i] = round(window_center.y - pos_from_center.y);
    }

    // Draw polygon with the given color
    filledPolygonRGBA(
        renderer,
        x_points, y_points, n,
        color.r * 255, color.g * 255, color.b * 255, 255
    );
    free(x_points);
    free(y_points);
}

void sdl_show(SDL_Renderer *renderer) {
    SDL_RenderPresent(renderer);
}

void sdl_render_scene(Scene *scene, SDL_Renderer *renderer) {
    sdl_clear(renderer);
    size_t body_count = scene_bodies(scene);
    for (size_t i = 0; i < body_count; i++) {
        Body *body = scene_get_body(scene, i);
        List *shape = body_get_shape(body);
        sdl_draw_polygon(shape, body_get_color(body), renderer);
        list_free(shape);
        //body_free(body);
    }
    /*if (surface1) {
        printf("surface1\n");
        SDL_Texture *texture1 = SDL_CreateTextureFromSurface(renderer, surface1);
        SDL_RenderCopy(renderer, texture1, NULL, rect1);
        SDL_DestroyTexture(texture1);
    }
    if (surface2) {
        printf("surface2\n");
        SDL_Texture *texture2 = SDL_CreateTextureFromSurface(renderer, surface2);
        SDL_RenderCopy(renderer, texture2, NULL, rect2);
        SDL_DestroyTexture(texture2);
    }*/
    sdl_show(renderer);
}

void sdl_render_text(SDL_Renderer *renderer, SDL_Surface *surface, SDL_Rect *rect) {
    if (surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, NULL, rect);
        SDL_DestroyTexture(texture);
    }
    //free(rect);
    sdl_show(renderer);
}

void sdl_on_key(KeyHandler handler) {
    key_handler = handler;
}

void close_window() {
    SDL_DestroyWindow(window);
}
/*
double time_since_last_tick(void) {
    clock_t now = clock();
    double difference = last_clock
        ? (double) (now - last_clock) / CLOCKS_PER_SEC
        : 0.0; // return 0 the first time this is called
    last_clock = now;
    return difference;
}*/
