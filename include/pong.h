#ifndef __PONG_H__
#define __PONG_H__

#include "sdl_wrapper.h"
#include "body.h"
#include "forces.h"
#include "scene.h"
#include "collision.h"
#include "text.h"
#include <stdbool.h>
#include <math.h>

/*
* The possible types of bodies to make.
*/
typedef enum {
    PADDLE,
    BALL,
} BodyType;

/*
* @brief Initializes a window with the given height and width
*
* @return the renderer associated with this window
*/
SDL_Renderer *window_init();

/*
* @brief Creates and returns a body object based off its BodyType
*
* @param type The type of body to be created
* @param center The center of the created body
* @return The created body
*/
Body *make_body(BodyType *type, Vector center);

/*
* @brief Handles key events. Left and right arrows move the shooter and the
* space bar creates lasers from the shooter
*
* @param key A character indicating which key was pressed
* @param type The type of key event (KEY_PRESSED or KEY_RELEASED)
* @param held_time The amount of time the key is held down for
* @param scene The scene that the key presses will impact
*/
void on_key(char key, KeyEventType type, double held_time, Scene *scene);

/*
* @brief Handles if ships move offscreen. If one of the paddles are offscreen,
* it is translated onto screen.  If an ball is offscreen, it bounces of the
* ceiling or floor and would reset the game if it hits the side.
*
* @return bool Whether game needs to be reset because  ball hit the side
*/
char move_if_offscreen(Body *paddle_one, Body *paddle_two, Body *ball);

/*
* @brief Resets the scene after point is made
*/
void reset(Scene *scene);

#endif // #ifndef __PONG_H__
