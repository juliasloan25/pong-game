#include "text.h"


TTF_Font *load_font(int font_size) {
    TTF_Font *font;
    font = TTF_OpenFont("ostrich-regular.ttf", font_size);
    if (!font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(1);
    }
    return font;
}


void display_text(SDL_Renderer *renderer, char *text, int font_size, int score_x,
                int score_y, int width, int height) {
    TTF_Init();
    if (!TTF_WasInit() && TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(1);
    }

    TTF_Font *font = load_font(font_size);
    SDL_Color color = {0, 0, 0}; //black

    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FreeSurface(text_surface);

    //rectangle to position text
    SDL_Rect *message_rect = malloc(sizeof(SDL_Rect));
    message_rect->x = score_x;
    message_rect->y = score_y;
    message_rect->w = width;
    message_rect->h = height;

    if (!(text_surface = TTF_RenderText_Solid(font, text, color))) {
        printf("TTF_Render: %s\n", TTF_GetError());
        exit(1);
    }

    SDL_RenderCopy(renderer, texture, NULL, message_rect);
    SDL_RenderPresent(renderer);

    TTF_CloseFont(font);
    SDL_DestroyTexture(texture);
}
