#include "text_simple.h"

TTF_Font *test_load_font(int font_size) {
    char *font_path = SDL_GetBasePath();
    const char *font_str = "ostrich-regular.ttf";
    snprintf(font_path, 100, "%s", font_str);
    printf("path: %s\n", font_path);

    TTF_Font *font = TTF_OpenFont(font_path, font_size);
    if (font == NULL) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(1);
    }
    return font;
}

void test_display_text(SDL_Renderer *renderer, char *text, TTF_Font *font,
                            SDL_Rect *rect, SDL_Color color) {
    //get vector instead of x and y pos, then use sdl_wrapper scaling
    //or change scene so coordinates match up with sdl pixel coordinates
    //SDL_Color color = {0, 0, 0, 0}; //white

    if (font == NULL) {
        printf("font null");
        exit(1);
    }

    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, color);
    if (text_surface == NULL) {
        printf("TTF_Render: %s\n", TTF_GetError());
        exit(1);
        //return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_RenderCopy(renderer, texture, NULL, rect);
    // SDL_RenderPresent(renderer);
    sdl_show(renderer);
    sdl_render_text(renderer, text_surface, rect);
    //SDL_Delay(10000);

    //free(rect); //CHANGE IN NON-SIMPLE
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(text_surface);
    //return text_surface;
}
