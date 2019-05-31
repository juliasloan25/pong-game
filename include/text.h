#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>

/*
* Set the font to be used in displaying text. Currently there is only one font,
* but the font size can be changed.
* @param font_size the pixel height of the text
* @return the complete font
*/
TTF_Font *load_font(int font_size);

/*
* Displays the text with the given font size on the scene.
* @param text the string to be displayed
* @param font_size the pixel height of the text
*/
void display_text(SDL_Renderer *renderer, char *text, int font_size, int score_x,
                    int score_y, int width, int height);
