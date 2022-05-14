#include "game_gfx.h"
#include "Adafruit_GFX.h"
#include "pieces_gfx.h"
#include "screen.h"
#include "timer.h"
#include "chess_engine.h"


void draw_pallete(int size){
  int j = 0;
  int i = 0;
  for (int color = 0; color < 256; ++color){
    int x = i * size;
    int y = j * size;

    if(x > (SCREEN_WIDTH - size)){
        j++;
        y = j * size;
        i = 0;
        x = 0;
    }
    fillRect(x, y, size, size, color);
    i++;
  }
}

void draw_logo(int length, int x, int y, bool animate){
  bool color = false;
  const u32 now = (u32) timer_get();
  
  if(((now % TIMER_TPS)> TIMER_TPS / 2) && animate){
      color = true;
  }

    for(int i = 0; i < 2; i++ ){
    for (int j = 0; j < 2; ++j){
        fillRect(x + i * (length/2), y + j * (length/2), length/2, length/2, (color) ? (DARK) : (LIGHT));
        drawRect(x + i * (length/2), y + j * (length/2), length/2, length/2, 0);
        color = !color;
    }
    color = !color;
  }
}

void draw_chess_cursor(int square_size, int x, int y, u8 color){
  if((square_size * 8) > ((SCREEN_HEIGHT<SCREEN_WIDTH)?SCREEN_HEIGHT:SCREEN_WIDTH )){

    return;
  }

  drawRect(BOARD_X + x * square_size, BOARD_Y + y * square_size, square_size, square_size, color);
}

void draw_piece(int x, int y, int color, u8 piece, u8 side){  
  struct pieces_t pieces = get_pieces();

  if(side == 16){
    x = 7 - x;
    y = 7 - y;
  }

  if(piece == 'n'){
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.knight.fill, SQUARE_SIZE,  SQUARE_SIZE, DARK_PIECE_FILL, 0);
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.knight.border, SQUARE_SIZE,  SQUARE_SIZE, LIGHT_PIECE_BORDER, 0);
  }

  if(piece == 'N'){
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.knight.fill, SQUARE_SIZE,  SQUARE_SIZE, LIGHT_PIECE_FILL, 0);
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.knight.border, SQUARE_SIZE,  SQUARE_SIZE, DARK_PIECE_BORDER, 0);
  }

  if(piece == 'k'){
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.king.fill, SQUARE_SIZE,  SQUARE_SIZE, DARK_PIECE_FILL, 0);
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.king.border, SQUARE_SIZE,  SQUARE_SIZE, LIGHT_PIECE_BORDER, 0);
  }

  if(piece == 'K'){
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.king.fill, SQUARE_SIZE,  SQUARE_SIZE, LIGHT_PIECE_FILL, 0);
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.king.border, SQUARE_SIZE,  SQUARE_SIZE, DARK_PIECE_BORDER, 0);
  }

  if(piece == 'b'){
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.bishop.fill, SQUARE_SIZE,  SQUARE_SIZE, DARK_PIECE_FILL, 0);
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.bishop.border, SQUARE_SIZE,  SQUARE_SIZE, LIGHT_PIECE_BORDER, 0);
  }

  if(piece == 'B'){
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.bishop.fill, SQUARE_SIZE,  SQUARE_SIZE, LIGHT_PIECE_FILL, 0);
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.bishop.border, SQUARE_SIZE,  SQUARE_SIZE, DARK_PIECE_BORDER, 0);
  }

  if(piece == 'r'){
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.rook.fill, SQUARE_SIZE,  SQUARE_SIZE, DARK_PIECE_FILL, 0);
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.rook.border, SQUARE_SIZE,  SQUARE_SIZE, LIGHT_PIECE_BORDER, 0);
  }

  if(piece == 'R'){
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.rook.fill, SQUARE_SIZE,  SQUARE_SIZE, LIGHT_PIECE_FILL, 0);
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.rook.border, SQUARE_SIZE,  SQUARE_SIZE, DARK_PIECE_BORDER, 0);
  }

  if(piece == '+'){
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.pawn.fill, SQUARE_SIZE,  SQUARE_SIZE, DARK_PIECE_FILL, 0);
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.pawn.border, SQUARE_SIZE,  SQUARE_SIZE, LIGHT_PIECE_BORDER, 0);
  }

  if(piece == '*'){
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.pawn.fill, SQUARE_SIZE,  SQUARE_SIZE, LIGHT_PIECE_FILL, 0);
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.pawn.border, SQUARE_SIZE,  SQUARE_SIZE, DARK_PIECE_BORDER, 0);
  }

  if(piece == 'q'){
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.queen.fill, SQUARE_SIZE,  SQUARE_SIZE, DARK_PIECE_FILL, 0);
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.queen.border, SQUARE_SIZE,  SQUARE_SIZE, LIGHT_PIECE_BORDER, 0);
  }

  if(piece == 'Q'){
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.queen.fill, SQUARE_SIZE,  SQUARE_SIZE, LIGHT_PIECE_FILL, 0);
    drawBitmap(BOARD_X + x * SQUARE_SIZE, BOARD_Y + y * SQUARE_SIZE,pieces.queen.border, SQUARE_SIZE,  SQUARE_SIZE, DARK_PIECE_BORDER, 0);
  }
}   

void draw_board(int square_size, int x, int y, u8 fg_color, u8 bg_color, u8 side){
  bool color = (side == 16) ? true : false;

  if((square_size * 8) > ((SCREEN_HEIGHT<SCREEN_WIDTH)?SCREEN_HEIGHT:SCREEN_WIDTH )){
    return;
  }

  for(int i = 0; i < 8; i++ ){
    for (int j = 0; j < 8; ++j){
        fillRect(x + i * square_size, y + j * square_size, square_size, square_size, (color) ? (fg_color) : (bg_color));
        drawRect(x + i * square_size, y + j * square_size, square_size, square_size, GRID_COLOR);
        color = !color;
    }
    color = !color;
  }
}

void draw_scorboard(){
    int start = (4 * 2) + (8 * SQUARE_SIZE);

    fillRect( start, BOARD_Y/2, SCREEN_WIDTH - start - BOARD_X / 2, SCREEN_HEIGHT - BOARD_Y, DARK);
    drawRect(start, BOARD_Y/2, SCREEN_WIDTH - start - BOARD_X / 2, SCREEN_HEIGHT - BOARD_Y, GRID_COLOR);
    
    setCursor(220, 10);
    setTextColor(LIGHT);
    print("Chess X86");
}

void render_splash_screen(){

  int logo_y = 85;
  fillScreen(0);
  setTextSize(2);
  setCursor(105, 65);
  setTextColor(255);
  print("Chess X86");

  draw_logo(16, 150, 40, true);

  struct pieces_t pieces = get_pieces();
  drawBitmap(98 , logo_y, pieces.knight.fill, SQUARE_SIZE,  SQUARE_SIZE, LIGHT, 0);
  drawBitmap(98 , logo_y, pieces.knight.border, SQUARE_SIZE,  SQUARE_SIZE, 0, 0);

  drawBitmap(98 + 24 , logo_y, pieces.king.fill, SQUARE_SIZE,  SQUARE_SIZE, LIGHT, 0);
  drawBitmap(98 + 24 , logo_y, pieces.king.border, SQUARE_SIZE,  SQUARE_SIZE, 0, 0);


  drawBitmap(98 + 24 + 24 , logo_y, pieces.queen.fill, SQUARE_SIZE,  SQUARE_SIZE, LIGHT, 0);
  drawBitmap(98 + 24 + 24 , logo_y, pieces.queen.border, SQUARE_SIZE,  SQUARE_SIZE, 0, 0);


  drawBitmap(98 + 24 + 24 + 24 , logo_y, pieces.bishop.fill, SQUARE_SIZE,  SQUARE_SIZE, LIGHT, 0);
  drawBitmap(98 + 24 + 24 + 24 , logo_y, pieces.bishop.border, SQUARE_SIZE,  SQUARE_SIZE, 0, 0);


  drawBitmap(98 + 24 + 24 + 24 + 24 , logo_y, pieces.rook.fill, SQUARE_SIZE,  SQUARE_SIZE, LIGHT, 0);
  drawBitmap(98 + 24 + 24 + 24 + 24 , logo_y, pieces.rook.border, SQUARE_SIZE,  SQUARE_SIZE, 0, 0);


  setTextSize(1);
  setCursor(90, 117);

  if(((u32)timer_get() % TIMER_TPS)> TIMER_TPS / 2){
      setTextColor(LIGHT);
  }

  print("Press ENTER to continue");
  writeLine(110, 135, 205, 135, 25);
}

void render_menu(){
  setTextSize(1); 
  render_splash_screen();
  fillRect( 92, 112, SCREEN_WIDTH, 50, 0); // Remove prev texts
  
  setTextColor(255);
  setCursor(80, 112 + 26);
  print("1. White  2. Black  3. Auto");

  if(((u32) timer_get() % TIMER_TPS)> TIMER_TPS / 2){
      setTextColor(LIGHT);
  }

  setCursor(92 + 33, 120);
  print("Select Side\n");
}


// Scoreboard 
// - Header
// - Text
// - Status

void render_scoreboard(game_t * game){
  if(game){
    if((game->move_count < 1) || !((game->move_count ) % SHOW_MOVE_MAX)){
      draw_scorboard();
    }

    int text_start_x = 220;
    int text_start_y = 25;
    char buff[100];
    bool flag = (game->move_count % 2) ? false : true;
    
    // - Header
    setCursor(text_start_x, 10);
    setTextColor(LIGHT);
    print("Chess x86");
    
    // - Text
    if(game->valid_move){
      if(!flag){
        setCursor(text_start_x, text_start_y + (game->move_count % SHOW_MOVE_MAX * 8));
        print(itoa((game->move_count + 1) / 2, buff, 10 ));
        print(". ");
        print(game->move_buffer);
      }
      //   else{
      //   setCursor(text_start_x + 3 * 6 + 35, text_start_y + ((game->move_count - 1) 
      //     % SHOW_MOVE_MAX * 8));
      //   print(game->move_buffer);
      // }
      // game->valid_move = false;
    }
     else{
      // 0 = invalid move
      render_status_bar(itoa(game->score, buff, 10 )); // - Status
    }

    if(game->score >= 4000 - 1){
      // setCursor(0, 180);
      render_status_bar("Game Over"); // - Status
    }
  }
}

void render_board(game_t * game){
  if(game){
    int row = 0;
    int column = 0;
    char Sym[] = ".?+nkbrq?*?NKBRQ";
    
    u8 fg_color = DARK;
    u8 bg_color = LIGHT;
    
    draw_board(SQUARE_SIZE, BOARD_X, BOARD_Y, fg_color, bg_color, game->player_side);
    if(game->board){
      u8 i = 0;
      for (i = 0; i < 121; i++) {
        if (i & 8) {
            i = i + 7;
            row++;
            column = 0;
        } else {
            draw_piece(column, row, 0, Sym[game->board[i] & 15], game->player_side);
            column++;
        }
      }
    }
  }
}

void render_status_bar(char * status){
  int start = (4 * 2) + (8 * SQUARE_SIZE) + BOARD_X / 2;
  fillRect( start, 185, SCREEN_WIDTH - start - BOARD_X, 11, LIGHT);
  setCursor(start + 10, 185 + 2);
  setTextColor(0);
  print(status);
}

void render_game(game_t * game){
  if(game){
      render_board(game);
      render_scoreboard(game);
  }
}

