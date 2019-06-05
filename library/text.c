#include "text.h"

TTF_Font *load_font(int font_size) {
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

void display_text(SDL_Renderer *renderer, char *text, TTF_Font *font, int x_pos,
                int y_pos, int width, int height) {
    //get vector instead of x and y pos, then use sdl_wrapper scaling
    //or change scene so coordinates match up with sdl pixel coordinates
    SDL_Color color = {0, 0, 0}; //white

    if (font == NULL) {
        printf("font null");
        exit(1);
    }
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, color);
    if (text_surface == NULL) {
        printf("TTF_Render: %s\n", TTF_GetError());
        exit(1);
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    //create rectangle to position text
    SDL_Rect *rect = malloc(sizeof(SDL_Rect));
    rect->x = 400;
    rect->y = 300;
    rect->w = 30;
    rect->h = 40;
    //sdl_scale_rect(rect, renderer);

    SDL_RenderCopy(renderer, texture, NULL, rect);
    // SDL_RenderPresent(renderer);
    sdl_show(renderer);
    //SDL_Delay(10000);

    free(rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(texture);
}

void set_background(SDL_Renderer *renderer, int r, int g, int b) {
    int opacity = 255; //maximum value
    //set render color to given color
    SDL_SetRenderDrawColor(renderer, r, g, b, opacity);
    //clear the window to the current color
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
    SDL_Delay(50); //delay time in milliseconds
}

int start_screen(SDL_Renderer *renderer, int width, int height, TTF_Font *font) {
    //set background to black
    set_background(renderer, 0, 0, 0);

    char *title = "PONG";
    char *text1 = "Single player";
    char *text2 = "Demo mode";
    int num_buttons = 2;

    //display title to start screen
    display_text(renderer, title, font, TITLE_X, TITLE_Y, TITLE_WIDTH, TITLE_HEIGHT);
    //display play modes
    display_text(renderer, text1, font, TEXT_X, TEXT_Y_START, TEXT_WIDTH, TEXT_HEIGHT);
    display_text(renderer, text2, font, TEXT_X, TEXT_Y_START + (2 * TEXT_HEIGHT),
                    TEXT_WIDTH, TEXT_HEIGHT);

    //1 if single player, 2 if demo mode, 0 if no press
    return handle_buttons(num_buttons);
}

//after calling this, call display_text on the scores to overlay them
int end_screen(SDL_Renderer *renderer, int width, int height, TTF_Font *font) {
    //set background to black
    set_background(renderer, 0, 0, 0);

    char *text0 = "GAME OVER";
    char *text1 = "Play again?"; //If selected, return to start screen
    int num_buttons = 1;

    //display text to screen
    display_text(renderer, text0, font, TITLE_X, TITLE_Y, TITLE_WIDTH, TITLE_HEIGHT);
    display_text(renderer, text1, font, TEXT_X, TEXT_Y_START, TEXT_WIDTH, TEXT_HEIGHT);

    return handle_buttons(num_buttons);
}

int handle_buttons(int num_buttons) {
    SDL_Event e;

    for (int i = 1; i <= num_buttons; i++) {
        while(SDL_PollEvent(&e)) {
            SDL_MouseButtonEvent mbe = e.button;
            switch(mbe.type) {
                case SDL_QUIT:
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    //int x = mbe.x;
                    //int y = mbe.y;
                    if ((mbe.x >= TEXT_X && mbe.x <= TEXT_X + TEXT_WIDTH) &&
                        (mbe.y >= i * TEXT_Y_START && mbe.y < i * TEXT_Y_START + TEXT_HEIGHT)) {
                            return i;
                        }
            }
        }
    }

    //no button press - would this just be infinite loop?
    //keep track of time and exit after certain amount?
    return 0;
}
