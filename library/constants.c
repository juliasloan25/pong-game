#include "constants.h"

const double WIDTH = 800; //screen width
const double HEIGHT = 600; //screen height
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
const int FONT_SIZE = 25;
const double G = 6500.0; //gravity for obstacle

const int TITLE_WIDTH = WIDTH/3;
const int TITLE_HEIGHT = HEIGHT/2;
const int TITLE_X = WIDTH/2 - TITLE_WIDTH/2;
const int TITLE_Y = 40;

const int TEXT_WIDTH = WIDTH/4;
const int TEXT_HEIGHT = HEIGHT/8;
const int TEXT_X = WIDTH/2 - TEXT_WIDTH/2;
const int TEXT_Y_START = TITLE_HEIGHT;
const int SCORE_WIDTH = TEXT_WIDTH/4;

const int SCORE_Y = 5;
const int LEFT_SCORE_X = TEXT_WIDTH/2;
const int RIGHT_SCORE_X = WIDTH - TEXT_WIDTH;

const SDL_Color BLACK = {0, 0, 0, 255};
const SDL_Color WHITE = {255, 255, 255, 255};

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
const Vector PADDLE_ONE_CENTER = {
    .x = PADDLE_WIDTH / 2.0,
    .y = HEIGHT/2
}; //initial and reset center of paddle one
const Vector PADDLE_TWO_CENTER = {
    .x = WIDTH - (PADDLE_WIDTH / 2.0),
    .y  = HEIGHT/2
}; //initial and reset center of paddle two
const Vector BALL_CENTER = {
    .x = WIDTH / 2.0,
    .y = HEIGHT/ 2.0
}; //initial and reset center of the ball
