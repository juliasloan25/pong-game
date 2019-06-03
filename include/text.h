#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>

const int TITLE_X = width/2 - 100;
const int TITLE_Y = 40;
const int TITLE_WIDTH = width / 2;
const int TITLE_HEIGHT = height / 4;

const int TEXT_X = width / 2 - 50;
const int TEXT_Y_START = 200;
const int TEXT_WIDTH = width / 4;
const int TEXT_HEIGHT = height / 8;

/*
* Set the font to be used in displaying text. Currently there is only one font,
* but the font size can be changed.
* @param font_size the pixel height of the text
* @return the complete font
*/
TTF_Font *load_font(int font_size);

/*
* Displays the text with the given font size on the scene.
* @param renderer the SDL renderer that will draw the text
* @param text the string to be displayed
* @param font_size the pixel height of the text
* @param x_pos the x-coordinate of the left side of the text
* @param y_pos the y-coordinate of the top side of the text
* @param width the width of the text box
* @param height the height of the text box
*/
void display_text(SDL_Renderer *renderer, char *text, int font_size, int x_pos,
                    int y_pos, int width, int height);

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
int start_screen(SDL_Renderer *renderer, int width, int height);

/*
* Initializes the end game screen with buttons.
* @param renderer the SDL renderer that will draw text
* @param width the width of the window
* @param height the height of the window
* @return an integer representing which text was clicked
*/
int end_screen(SDL_Renderer *renderer, int width, int height);

/*
* Handles buttons and returns an integer corresponding to the number of the
* button pressed, with 1 at the top and n at the bottom.
* @param num_buttons the number of buttons currently on-screen
* @return an integer representing which text was clicked
*/
int handle_buttons(int num_buttons);
