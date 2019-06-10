#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <sdl_wrapper.h>
#include "constants.h"



/*
* Set the font to be used in displaying text. Currently there is only one font,
* but the font size can be changed.
* @param font_size the pixel height of the text
* @return the complete font
*/
TTF_Font *load_font(int font_size);

/*
* Create an SDL Rect to position text on the window.
* @param x_pos the x coordinate of the left side of the text box
* @param y_pos the y coordinate of the top side of the text box
* @param width the width of the text box
* @param height the height of the text box
*/

SDL_Rect *make_rect(int x_pos, int y_pos, int width, int height);

/*
* Displays the text with the given font size on the scene.
* @param renderer the SDL renderer that will draw the text
* @param text the string to be displayed
* @param font_size the pixel height of the text
* @param rect the positioning rectangle for the text
*/
void display_text(SDL_Renderer *renderer, char *text, TTF_Font *font,
                    SDL_Rect *rect, SDL_Color color);

/*
* Sets the entire background of the window to the given color.
* @param renderer the SDL renderer to set as the background
* @param r the red value of the color
* @param g the green value of the color
* @param b the blue value of the color
*/
void set_background(SDL_Renderer *renderer, TTF_Font *font);

/*
* Initializes the start screen with buttons.
* @param renderer the SDL renderer that will draw text
* @font the font to draw text in
* @return an integer representing which text was clicked
*/
int start_screen(SDL_Renderer *renderer, TTF_Font *font);

/*
* Initializes the end game screen with buttons.
* @param renderer the SDL renderer that will draw text
* @font the font to draw text in
* @return an integer representing which text was clicked
*/
int end_screen(SDL_Renderer *renderer, TTF_Font *font);

/*
* Handles buttons and returns an integer corresponding to the number of the
* button pressed, with 1 at the top and n at the bottom.
* @param num_buttons the number of buttons currently on-screen
* @return an integer representing which text was clicked
*/
int handle_buttons(int num_buttons);
