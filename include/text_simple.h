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
TTF_Font *test_load_font(int font_size);

/*
* Displays the text with the given font size on the scene.
* @param renderer the SDL renderer that will draw the text
* @param text the string to be displayed
* @param font_size the pixel height of the text
* @param rect the positioning rectangle for the text
*/
void test_display_text(SDL_Renderer *renderer, char *text, TTF_Font *font,
                            SDL_Rect *rect, SDL_Color color);
