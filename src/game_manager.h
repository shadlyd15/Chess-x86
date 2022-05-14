#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "game.h"
#include "types.h"


game_t * init_game_data(game_t * game, u8 player_side);
game_t * init_game(game_t * game, u8 player_side);

game_t * navigation_key_handler(game_t * game);
game_t * reset_key_handler(game_t * game);

game_t * manage_lobby_state(game_t * game);
game_t * manage_menu_state(game_t * game);
game_t * manage_started_state(game_t * game);
game_t * manage_thinking_state(game_t * game);
game_t * manage_selected_state(game_t * game);
game_t * manage_moved_state(game_t * game);
game_t * manage_waiting_state(game_t * game);

game_t * game_send_key(game_t * game, char key);
game_t * manage_rendering(game_t * game);
game_t * manage_game_states(game_t * game);

#endif