#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <sdl_wrapper.h>
#include "constants.h"

const int TITLE_X = WIDTH / 2 - 100;
const int TITLE_Y = 40;
const int TITLE_WIDTH = WIDTH / 2;
const int TITLE_HEIGHT = HEIGHT / 4;

const int TEXT_X = WIDTH / 2 - 50;
const int TEXT_Y_START = 200;


/*
* Set the font to be used in displaying text. Currently there is only one font,
* but the font size can be changed.
* @param font_size the pixel height of the text
* @return the complete font
*/
TTF_Font *load_font(int font_size);

SDL_Rect *make_rect(int x_pos, int y_pos, int width, int height);

/*
* Displays the text with the given font size on the scene.
* @param renderer the SDL renderer that will draw the text
* @param text the string to be displayed
* @param font_size the pixel height of the text
* @param rect the positioning rectangle for the text
*/
SDL_Surface *display_text(SDL_Renderer *renderer, char *text, TTF_Font *font,
                    SDL_Rect *rect);

/*
* Sets the entire background of the window to the given color.
* @param renderer the SDL renderer to set as the background
* @param r the red value of the color
* @param g the green value of the color
* @param b the blue value of the color
*/
void set_background(SDL_Renderer *renderer, int r, int g, int b);

/*
* Initializes the start screen with buttons.
* @param renderer the SDL renderer that will draw text
* @param width the width of the window
* @param height the height of the window
* @return an integer representing which text was clicked
*/
int start_screen(SDL_Renderer *renderer, int width, int height, TTF_Font *font);

/*
* Initializes the end game screen with buttons.
* @param renderer the SDL renderer that will draw text
* @param width the width of the window
* @param height the height of the window
* @return an integer representing which text was clicked
*/
int end_screen(SDL_Renderer *renderer, int width, int height, TTF_Font *font);

/*
* Handles buttons and returns an integer corresponding to the number of the
* button pressed, with 1 at the top and n at the bottom.
* @param num_buttons the number of buttons currently on-screen
* @return an integer representing which text was clicked
*/
int handle_buttons(int num_buttons);
