#include "text.h"


TTF_Font *load_font(int font_size) {
    TTF_Font *font;
    font = TTF_OpenFont("ostrich-regular.ttf", font_size);
    if (!font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit();
    }
    return font;
}


//should this have capability to choose where to display text?
void display_text(char *text, int font_size) {
    TTF_Init();
    if (!TTF_WasInit() && TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit();
    }

    TTF_Font *font = load_font(font_size);
    SDL_Color color = {0, 0, 0}; //black
    SDL_Surface *screen; //scene?
    SDL_Surface *text_surface;

    TTF_RenderText_Solid(font, text, color);

    if (!(text_wurface = TTF_RenderText_Solid(font, text, color))) {
        printf("TTF_Render: %s\n", TTF_GetError());
        exit();
    }
    SDL_Blit_Surface(text_surface, NULL, screen, NULL;
    SDL_FreeSurface(text_surface);

    TTF_CloseFont(font);
}
