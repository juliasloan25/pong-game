#include "constants.h"

const int WIDTH = 800; //screen width
const int HEIGHT = 600; //screen height
const double BALL_RADIUS = 10.0; //radius of pong ball
const double PADDLE_HEIGHT = 100.0; //width of the pong paddle
const double PADDLE_WIDTH = 30.0; //height of the pong paddle
const double BALL_MASS = 100; //mass of the pong ball;
const double ELASTICITY = 1; //elasticity of collisions
const double PADDLE_VEL = 600.0; //velocity that the paddle can go
const double MASS = 50.0; //mass of all objects
const double BALL_VEL = 700.0; //initial velocity of ball
const int FONT_SIZE = 20; //standard font size

const int TITLE_WIDTH = WIDTH / 2;
const int TITLE_HEIGHT = HEIGHT / 4;
const int TITLE_X = WIDTH / 2 - TITLE_WIDTH / 2;
const int TITLE_Y = 40;

const int TEXT_WIDTH = WIDTH / 4;
const int TEXT_HEIGHT = HEIGHT / 8;
const int TEXT_X = WIDTH / 2 - TEXT_WIDTH / 2;
const int TEXT_Y_START = 250;
const int SCORE_WIDTH = TEXT_WIDTH / 4;

const int SCORE_Y = 5; //top side of scores
const int LEFT_SCORE_X = TEXT_WIDTH / 2; //left side of player 1 score
const int RIGHT_SCORE_X = WIDTH - TEXT_WIDTH; //left side of player 2 score

const SDL_Color BLACK = {0, 0, 0, 255}; //used for text display
const SDL_Color WHITE = {255, 255, 255, 255}; //used for text display
const RGBColor PADDLE_COLOR = {
    .r = 0,
    .g = 0,
    .b = 0
}; //black paddle color
const RGBColor BALL_COLOR = {
    .r = 1,
    .g = 0,
    .b = 0
}; //color of the pong ball
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
