#include "text.h"

TTF_Font *load_font(int font_size) {
    char *font_path = SDL_GetBasePath();
    const char *font_str = "ostrich-regular.ttf";
    snprintf(font_path, 100, "%s", font_str);

    TTF_Font *font = TTF_OpenFont(font_path, font_size);
    if (font == NULL) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(1);
    }
    return font;
}

SDL_Rect *make_rect(int x_pos, int y_pos, int width, int height) {
    //create rectangle to position text
    SDL_Rect *rect = malloc(sizeof(SDL_Rect));
    rect->x = x_pos;
    rect->y = y_pos;
    rect->w = width;
    rect->h = height;

    //sdl_scale_rect(rect, renderer);
    return rect;
}

void set_background(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Rect *bkgrd_rect = make_rect(0, 0, WIDTH, HEIGHT);
    //SDL_Surface *background = test_display_text(renderer, " ", font, bkgrd_rect);
    display_text(renderer, " ", font, bkgrd_rect, BLACK);

    //sdl_render_scene(scene, renderer, background, NULL, bkgrd_rect, NULL);
    SDL_Delay(100);
    free(bkgrd_rect);
    //need to close window when done?
}

void display_text(SDL_Renderer *renderer, char *text, TTF_Font *font,
                            SDL_Rect *rect, SDL_Color color) {
    //get vector instead of x and y pos, then use sdl_wrapper scaling
    //or change scene so coordinates match up with sdl pixel coordinates

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
    sdl_show(renderer);
    sdl_render_text(renderer, text_surface, rect);

    //free(rect); //CHANGE IN NON-SIMPLE
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(text_surface);
}

int start_screen(SDL_Renderer *renderer, TTF_Font *font) {
    char *title = "PONG";
    char *text1 = "Single player";
    char *text2 = "Two player";
    char *text3 = "Demo mode";
    int num_buttons = 3;

    //set positioning rectangles
    SDL_Rect *rect_title = make_rect(TITLE_X, TITLE_Y, TITLE_WIDTH, TITLE_HEIGHT);
    SDL_Rect *rect1 = make_rect(TEXT_X, TEXT_Y_START, TEXT_WIDTH, TEXT_HEIGHT);
    SDL_Rect *rect2 = make_rect(TEXT_X, TEXT_Y_START + (TEXT_HEIGHT),
                    TEXT_WIDTH, TEXT_HEIGHT);
    SDL_Rect *rect3 = make_rect(TEXT_X, TEXT_Y_START + (2 * TEXT_HEIGHT),
                    TEXT_WIDTH, TEXT_HEIGHT);

    //display title to start screen
    display_text(renderer, title, font, rect_title, WHITE);
    //display play modes
    display_text(renderer, text1, font, rect1, WHITE);
    display_text(renderer, text2, font, rect2, WHITE);
    display_text(renderer, text3, font, rect3, WHITE);
    //sdl_render_scene(scene, renderer, surface, NULL, rect, NULL);

    int button_num = handle_buttons(num_buttons);
    while(button_num == 0) {
        SDL_Delay(500);
        button_num = handle_buttons(num_buttons);
    }

    free(rect_title);
    free(rect1);
    free(rect2);
    free(rect3);

    //1 if single player, 2 if two-player, 3 if demo mode, 0 if no press
    return button_num;
}

//after calling this, call display_text on the scores to overlay them
int end_screen(SDL_Renderer *renderer, TTF_Font *font) {
    char *text0 = "GAME OVER";
    char *text1 = "Play again?"; //If selected, return to start screen
    int num_buttons = 1;

    SDL_Rect *rect_title = make_rect(TITLE_X, TITLE_Y, TITLE_WIDTH, TITLE_HEIGHT);
    SDL_Rect *rect1 = make_rect(TEXT_X, TEXT_Y_START, TEXT_WIDTH, TEXT_HEIGHT);

    //display text to screen
    display_text(renderer, text0, font, rect_title, WHITE);
    display_text(renderer, text1, font, rect1, WHITE);

    int button_num = handle_buttons(num_buttons);
    while(button_num == 0) {
        SDL_Delay(500);
        button_num = handle_buttons(num_buttons);
    }

    free(rect_title);
    free(rect1);
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
                  switch(e.button.button){
                    case SDL_BUTTON_LEFT:
                      if ((mbe.x >= TEXT_X && mbe.x <= TEXT_X + TEXT_WIDTH) &&
                          (mbe.y >= i * TEXT_Y_START && mbe.y < i * TEXT_Y_START + TEXT_HEIGHT)) {
                              return i;
                        }
                        break;
                    case SDL_BUTTON_RIGHT:
                       continue;
                      }
                      break;
            }
        }
    }
    //no button press - would this just be infinite loop?
    //keep track of time and exit after certain amount?
    return 0;
}
