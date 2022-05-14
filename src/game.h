#ifndef GAME_H
#define GAME_H 

#include "types.h"

#define MOVE_BUFFER_SIZE 5
#define BOARD_BUFFER_SIZE 129

typedef enum{
    LOBBY,
    MENU,
    STARTED,
    THINKING,
    SELECTED,
    MOVED,
    WAITING,
    GAME_OVER
} player_state_t;

typedef struct {
  int player_side;
  int current_side;

  int move_count;
  player_state_t state;
  
  bool valid_move;

  u8 current_index;
  char move_buffer[MOVE_BUFFER_SIZE];
  char * board;
  
  int cursor_x;
  int cursor_y;
  int score;

  char key;
  bool key_event;
} game_t;

#endif