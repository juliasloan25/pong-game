#include "text.h"

TTF_Font *load_font(int font_size) {
    char *font_path = SDL_GetBasePath();
    char *font_str = "ostrich-regular.ttf";
    int length = strlen(font_path) + strlen(font_str) + 2;
    char path[length];
    snprintf(path, length, "%s%s", font_path, font_str);
    //snprintf(font_path, 100, "%s", font_str);

    TTF_Font *font = TTF_OpenFont(path, font_size);
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
    display_text(renderer, " ", font, bkgrd_rect);

    //sdl_render_scene(scene, renderer, background, NULL, bkgrd_rect, NULL);
    SDL_Delay(100);
    free(bkgrd_rect);
    //need to close window when done?
}

void display_text(SDL_Renderer *renderer, char *text, TTF_Font *font,
                            SDL_Rect *rect) {
    //get vector instead of x and y pos, then use sdl_wrapper scaling
    //or change scene so coordinates match up with sdl pixel coordinates

    if (font == NULL) {
        printf("font null");
        exit(1);
    }
    printf("%s\n", text);

    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, WHITE);
    SDL_LockSurface(text_surface);
    if (text_surface == NULL) {
        printf("TTF_Render: %s\n", TTF_GetError());
        exit(1);
        //return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_RenderCopy(renderer, texture, NULL, rect);
    //SDL_RenderPresent(renderer);
    sdl_show(renderer);
    //SDL_Delay(1000);
    SDL_UnlockSurface(text_surface);

    //sdl_render_text(renderer, text_surface, rect);

    //free(rect); //CHANGE IN NON-SIMPLE
    //SDL_DestroyTexture(texture);
    //SDL_FreeSurface(text_surface);
    return;
}

int start_screen(SDL_Renderer *renderer, TTF_Font *font) {
    char *title = "PONG";
    char *text1 = "Single user";
    char *text2 = "Two users";
    char *text3 = "Demo mode";
    char *text4 = "Networking";
    int num_buttons = 4;

    //set positioning rectangles
    SDL_Rect *rect_title = make_rect(TITLE_X, TITLE_Y, TITLE_WIDTH, TITLE_HEIGHT);
    //SDL_Rect *rect_title = make_rect(0, 0, TITLE_WIDTH, TITLE_HEIGHT);
    SDL_Rect *rect1 = make_rect(TEXT_X, TEXT_Y_START, TEXT_WIDTH, TEXT_HEIGHT);
    SDL_Rect *rect2 = make_rect(TEXT_X, TEXT_Y_START + (TEXT_HEIGHT), TEXT_WIDTH, TEXT_HEIGHT);
    //SDL_Rect *rect2 = make_rect(0, 0, TEXT_WIDTH, TEXT_HEIGHT);
    SDL_Rect *rect3 = make_rect(TEXT_X, TEXT_Y_START + (2 * TEXT_HEIGHT),
                    TEXT_WIDTH, TEXT_HEIGHT);
    SDL_Rect *rect4 = make_rect(TEXT_X, TEXT_Y_START + (3 * TEXT_HEIGHT),
                    TEXT_WIDTH, TEXT_HEIGHT);

    //display title to start screen
    display_text(renderer, title, font, rect_title);
    //display play modes
    display_text(renderer, text1, font, rect1);
    display_text(renderer, text2, font, rect2);
    display_text(renderer, text3, font, rect3);
    display_text(renderer, text4, font, rect4);

    int button_num = handle_buttons(num_buttons);
    while(button_num == 0) {
        SDL_Delay(500);
        button_num = handle_buttons(num_buttons);
    }

    free(rect_title);
    free(rect1);
    free(rect2);
    free(rect3);
    free(rect4);
    //return 0;

    //1 if single player, 2 if demo mode, 0 if no press
    return button_num;
}

//after calling this, call display_text on the scores to overlay them
int end_screen(SDL_Renderer *renderer, TTF_Font *font) {
    char *text0 = "GAME OVER";
    int num_buttons = 0;
    //char *text1 = "CLOSE";
    //int num_buttons = 1;
    //int num_buttons = 0;

    SDL_Rect *rect_title = make_rect(TITLE_X, TITLE_Y, TITLE_WIDTH, TITLE_HEIGHT);
    //SDL_Rect *rect1 = make_rect(TEXT_X, TEXT_Y_START, TEXT_WIDTH, TEXT_HEIGHT);

    //display text to screen
    display_text(renderer, text0, font, rect_title);
    //display_text(renderer, text1, font, rect1);
    int button_num = handle_buttons(num_buttons);
    /*while(button_num == 0) {
        SDL_Delay(500);
        button_num = handle_buttons(num_buttons);
    }*/
    SDL_Delay(3000);
    free(rect_title);
    //  free(rect1);
    return button_num;

    //no buttons
    /*printf("%s\n", );
    free(rect_title);
    return num_buttons;*/
}

int difficulty_screen(SDL_Renderer *renderer, TTF_Font *font) {
    char *text0 = "CHOOSE A DIFFICULTY";
    char *text1 = "EASY";
    char *text2 = "MEDIUM";
    char *text3 = "HARD";
    int num_buttons = 3;

    SDL_Rect *rect_title = make_rect(TITLE_X/2, TITLE_Y, TITLE_WIDTH*2, TITLE_HEIGHT);
    SDL_Rect *rect1 = make_rect(TEXT_X, TEXT_Y_START, TEXT_WIDTH, TEXT_HEIGHT);
    SDL_Rect *rect2 = make_rect(TEXT_X, TEXT_Y_START + (TEXT_HEIGHT),
                    TEXT_WIDTH, TEXT_HEIGHT);
    SDL_Rect *rect3 = make_rect(TEXT_X, TEXT_Y_START + (2 * TEXT_HEIGHT),
                    TEXT_WIDTH, TEXT_HEIGHT);

    //display text to screen
    display_text(renderer, text0, font, rect_title);
    display_text(renderer, text1, font, rect1);
    display_text(renderer, text2, font, rect2);
    display_text(renderer, text3, font, rect3);

    int button_num = handle_buttons(num_buttons);
    while(button_num == 0) {
        SDL_Delay(500);
        button_num = handle_buttons(num_buttons);
    }

    free(rect_title);
    free(rect1);
    free(rect2);
    free(rect3);
    return button_num;
}

int players_screen(SDL_Renderer *renderer, TTF_Font *font) {
    char *text0 = "HOW MANY PLAYERS?";
    char *text1 = "2";
    char *text2 = "4";
    char *text3 = "6";
    char *text4 = "8";
    int num_buttons = 4;

    SDL_Rect *rect_title = make_rect(TITLE_X - TITLE_WIDTH/2, TITLE_Y, TITLE_WIDTH * 2, TITLE_HEIGHT - 60);
    SDL_Rect *rect1 = make_rect(TEXT_X + TEXT_WIDTH/4, TEXT_Y_START,
                                  TEXT_WIDTH/2, TEXT_HEIGHT);
    SDL_Rect *rect2 = make_rect(TEXT_X + TEXT_WIDTH/4, TEXT_Y_START + (TEXT_HEIGHT),
                    TEXT_WIDTH/2, TEXT_HEIGHT);
    SDL_Rect *rect3 = make_rect(TEXT_X + TEXT_WIDTH/4, TEXT_Y_START +
                                  (2 * TEXT_HEIGHT), TEXT_WIDTH/2, TEXT_HEIGHT);
    SDL_Rect *rect4 = make_rect(TEXT_X + TEXT_WIDTH/4, TEXT_Y_START +
                                  (3 * TEXT_HEIGHT), TEXT_WIDTH/2, TEXT_HEIGHT);

    //display text to screen
    display_text(renderer, text0, font, rect_title);
    display_text(renderer, text1, font, rect1);
    display_text(renderer, text2, font, rect2);
    display_text(renderer, text3, font, rect3);
    display_text(renderer, text4, font, rect4);

    int button_num = handle_buttons(num_buttons);
    while(button_num == 0) {
        SDL_Delay(500);
        button_num = handle_buttons(num_buttons);
    }

    free(rect_title);
    free(rect1);
    free(rect2);
    free(rect3);
    free(rect4);
    return button_num * 2;
}

int handle_buttons(int num_buttons) {
    SDL_Event e;
    if (num_buttons > 0) {
    while(SDL_PollEvent(&e)) {
      for (int i = 1; i <= num_buttons; i++) {
          SDL_MouseButtonEvent mbe = e.button;
          switch(mbe.type) {
              case SDL_QUIT:
                  break;
              case SDL_MOUSEBUTTONDOWN:
                  switch(e.button.button){
                      case SDL_BUTTON_LEFT:
                      if(i <= num_buttons){
                          if ((mbe.x >= TEXT_X && mbe.x <= TEXT_X + TEXT_WIDTH) &&
                              (mbe.y >= TEXT_Y_START + TEXT_HEIGHT * (i - 1) && mbe.y <
                                  TEXT_Y_START + TEXT_HEIGHT * i)) {
                          return i;
                      }
                      case SDL_BUTTON_RIGHT:
                          continue;
                      }
                  }
              }
            }
        }
    }
    return 0;
}
