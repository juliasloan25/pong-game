#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include <stdbool.h>
#include "color.h"
#include "list.h"
#include "scene.h"
#include "vector.h"

// Values passed to a key handler when the given arrow key or space is pressed
#define LEFT_ARROW 1
#define UP_ARROW 2
#define RIGHT_ARROW 3
#define DOWN_ARROW 4
#define SPACE 5
#define ESCAPE 6

/**
 * The possible types of key events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * typedef unsigned int KeyEventType;
 * #define KEY_PRESSED 0
 * #define KEY_RELEASED 1
 */
typedef enum {
    KEY_PRESSED,
    MOUSE_MOVED,
    KEY_RELEASED
} KeyEventType;

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of key event (KEY_PRESSED or KEY_RELEASED)
 * @param held_time if a press event, the time the key has been held in seconds
 * @param scene The scene that the key presses will impact
 */
typedef void (*KeyHandler)(char key, KeyEventType type, double held_time,
                                                                Scene *scene);

//int return_mouse_y_position(Scene *scene);
/**
 * Initializes the SDL window and renderer.
 * Must be called once before any of the other SDL functions.
 *
 * @param min the x and y coordinates of the bottom left of the scene
 * @param max the x and y coordinates of the top right of the scene
 */
void sdl_init(Vector min, Vector max);

/**
 * Processes all SDL events and returns whether the window has been closed.
 * This function must be called in order to handle keypresses.
 * @param scene The scene that the sdl impacts
 *
 * @return true if the window was closed, false otherwise
 */
bool sdl_is_done(Scene *scene);

/**
 * Clears the screen. Should be called before drawing polygons in each frame.
 */
void sdl_clear(void);

/**
 * Draws a polygon from the given list of vertices and a color.
 *
 * @param points the list of vertices of the polygon
 * @param color the color used to fill in the polygon
 */
void sdl_draw_polygon(List *points, RGBColor color);

/**
 * Displays the rendered frame on the SDL window.
 * Must be called after drawing the polygons in order to show them.
 */
void sdl_show(void);

/**
 * Draws all bodies in a scene.
 * This internally calls sdl_clear(), sdl_draw_polygon(), and sdl_show(),
 * so those functions should not be called directly.
 *
 * @param scene the scene to draw
 */
void sdl_render_scene(Scene *scene);

/**
 * Registers a function to be called every time a key is pressed.
 * Overwrites any existing handler.
 *
 * Example:
 * ```
 * void on_key(char key, KeyEventType type, double held_time) {
 *     if (type == KEY_PRESSED) {
 *         switch (key) {
 *             case 'a':
 *                 puts("A pressed");
 *                 break;
 *             case UP_ARROW:
 *                 puts("UP pressed");
 *                 break;
 *         }
 *     }
 * }
 * int main(int argc, char **argv) {
 *     sdl_on_key(on_key);
 *     while (!sdl_is_done());
 * }
 * ```
 *
 * @param handler the function to call with each key press
 */
void sdl_on_key(KeyHandler handler);

/**
 * Gets the amount of time that has passed since the last time
 * this function was called, in seconds.
 *
 * @return the number of seconds that have elapsed
 */
double time_since_last_tick(void);

#endif // #ifndef __SDL_WRAPPER_H__
