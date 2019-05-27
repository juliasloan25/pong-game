#ifndef __SPACE_H__
#define __SPACE_H__

#include "sdl_wrapper.h"
#include "body.h"
#include "forces.h"
#include "scene.h"
#include "collision.h"
#include <stdbool.h>
#include <math.h>

/*
* The possible types of bodies to make.
*/
typedef enum {
    SHOOTER,
    ALIEN,
    SHOOTER_LASER,
    ALIEN_LASER
} BodyType;

/*
* @brief Initializes a window with the given height and width
*/
void window_init();

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
*/
void on_key(char key, KeyEventType type, double held_time);

/*
* @brief Handles if ships move offscreen. If the shooter is offscreen, it is
* translated onto screen.  If an alien is offscreen, it is translated downwards
* and the velocity is reversed.
*/
void move_if_offscreen();

#endif // #ifndef __SPACE_H__
