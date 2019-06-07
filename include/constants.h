#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <time.h>
#include "constants.h"
#include "vector.h"
#include "color.h"

extern const int WIDTH; //screen width
extern const int HEIGHT; //screen height
extern const double BALL_RADIUS; //radius of pong ball
extern const double PADDLE_HEIGHT; //width of the pong paddle
extern const double PADDLE_WIDTH; // height of the pong paddle
extern const double BALL_MASS; // mass of the pong ball;
extern const double ELASTICITY; //elasticity of collisions
extern const double PADDLE_VEL; //velocity that the paddle can go
extern const double MASS; //mass of all objects
extern const double BALL_VEL; // initial velocity of ball

extern const int TITLE_X;
extern const int TITLE_Y;
extern const int TITLE_WIDTH;
extern const int TITLE_HEIGHT;

extern const int TEXT_X;
extern const int TEXT_Y_START;
extern const int TEXT_WIDTH;
extern const int TEXT_HEIGHT;

extern const int SCORE_Y;
extern const int LEFT_SCORE_X;
extern const int RIGHT_SCORE_X;

extern const RGBColor PADDLE_COLOR;
extern const RGBColor BALL_COLOR;
extern const Vector paddle_one_center;
extern const Vector paddle_two_center;
extern const Vector ball_center;

#endif //#__CONSTANTS_H__
