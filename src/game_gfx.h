#ifndef _GAME_GFX_H_
#define _GAME_GFX_H_

#include "game.h"
#include "util.h"

#define SQUARE_SIZE 24
#define BOARD_SIZE (8 * SQUARE_SIZE)
#define BOARD_X 4
#define BOARD_Y 4

#define DARK    4
#define LIGHT   245


// Shundor Rong : 23 200 25 15
// #define DARK_PIECE    250
// #define LIGHT_PIECE   40

#define SHOW_MOVE_MAX 20

#define DARK_PIECE_FILL    40
#define LIGHT_PIECE_FILL   250

#define STATUS_BAR_WIDTH
#define STATUS_BAR_HEIGHT

#define TITLE_BAR_WIDTH
#define TITLE_BAR_HEIGHT

#define TEXTBOX_WIDTH
#define TEXTBOX_HEIGHT

#define DARK_PIECE_BORDER    0
#define LIGHT_PIECE_BORDER   0

#define GRID_COLOR 	  41
#define CURSOR_COLOR  255

void draw_pallete(int size);

void draw_logo(int length, int x, int y, bool animate);
void draw_board(int square_size, int x, int y, u8 fg_color, u8 bg_color, u8 side);
void draw_chess_cursor(int square_size, int x, int y, u8 color);

void render_menu();
void render_splash_screen();

void render_game();
void render_board(game_t * game);
void render_scoreboard(game_t * game);
void render_status_bar(char * status);


#endif // _GAME_GFX_H_