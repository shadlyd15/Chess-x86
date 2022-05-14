#include "game_gfx.h"
#include "chess_engine.h"
#include "keyboard.h"
#include "game_manager.h"

typedef game_t * state_change_cb(game_t * game);

state_change_cb * cb = manage_rendering;


inline u8 get_source_file(game_t * game){return (game->current_side == WHITE)?(game->move_buffer[0] - 'a'):(7 - game->move_buffer[0] + 'a');}
inline u8 get_source_rank(game_t * game){return (game->current_side == WHITE)?(7 - game->move_buffer[1] + 1 + '0'):(game->move_buffer[1] - 1 - '0' );}

inline u8 get_dest_file(game_t * game){return (game->current_side == WHITE)?(game->move_buffer[2] - 'a'):(7 - game->move_buffer[2] + 'a');}
inline u8 get_dest_rank(game_t * game){return (game->current_side == WHITE)?(7 - game->move_buffer[3] + 1 + '0'):(game->move_buffer[3] - 1 - '0' );}


game_t * change_state(game_t * game, player_state_t state){
    if(!game) return NULL;
    game->state = state;
    return cb(game);
}

void write_move_buffer(game_t * game, int file, int rank){  
    if(game->state == THINKING){
        game->move_buffer[0] = (game->current_side == WHITE)?
        (file + 'a') : (7 - file + 'a');
        game->move_buffer[1] = (game->current_side == WHITE)?
        (7 - rank + 1 + '0'):(rank + 1 + '0');
    } 
    else if(game->state == SELECTED){
        game->move_buffer[2] = (game->current_side == WHITE)?
        (file + 'a'):(7 - file + 'a');
        game->move_buffer[3] = (game->current_side == WHITE)?
        (7 - rank + 1 + '0'):(rank + 1 + '0');
    }
} 
  
game_t * init_game_data(game_t * game, u8 player_side){
    if(game){
        game->player_side = player_side;
        game->current_side = WHITE;
        game->move_count = 0;
        game->state = LOBBY; 
        game->current_index = 0;
        game->board = get_current_board();
        memset(game->move_buffer, 0, MOVE_BUFFER_SIZE);
        memset(game->board, 0, BOARD_BUFFER_SIZE);
        game->cursor_x = 4;
        game->cursor_y = 4;
        game->score = 0;
    }
    return game;
}

game_t * init_game(game_t * game, u8 player_side){
  if(game){
    init_game_data(game, player_side);
    init_chess_engine(game);
  }
  return game;
}

game_t * navigation_key_handler(game_t * game){
    if(!game) return NULL;

    if(game->key_event){

        if(game->key == KEY_UP){
            game->cursor_y--;
            if(game->cursor_y < 0){
                game->cursor_y = 7;
            }
            game->key_event = false;
        }

        if(game->key == KEY_DOWN ){
            game->cursor_y++;
            game->cursor_y = game->cursor_y % 8;
            game->key_event = false;
        }

        if(game->key == KEY_LEFT ){
            game->cursor_x--;
            if(game->cursor_x < 0){
                game->cursor_x = 7;
            }
            game->key_event = false;
        }

        if(game->key == KEY_RIGHT ){
            game->cursor_x++;
            game->cursor_x = game->cursor_x % 8;
            game->key_event = false;
        }
    }
    return game;
}

game_t * reset_key_handler(game_t * game){
    if(!game) return NULL;
    if(game->key_event){
        if(game->key == 'r' ){
            change_state(game, LOBBY);
            init_game(game, game->player_side);
            // game->current_side = WHITE;
            game->key_event = false;
        }
    }
    return game;
}


game_t * manage_lobby_state(game_t * game){
    if(!game) return NULL;
    if(game->key_event){
        if(game->key == KEY_ENTER){
            change_state(game, MENU);
            game->key_event = false;
        }
    }
    return game;
}

game_t * manage_menu_state(game_t * game){
    if(!game) return NULL;
    if(game->key_event){
        if((game->key > '0') && (game->key < ('4'))){ 
            // 1. White 8 | 2. Black 16 | 3. Auto 32
            init_game(game, (game->key - '0') * WHITE); 
            change_state(game, STARTED);
            game->key_event = false;
        }
    }
    return game;
}

game_t * manage_started_state(game_t * game){
    if(!game) return NULL;
    if(game->player_side == WHITE){
        change_state(game, THINKING);
    } else {
        change_state(game, WAITING);
    }
    return game;
}

game_t * manage_thinking_state(game_t * game){
    if(!game) return NULL;
    
    navigation_key_handler(game);

    if(game->key_event){
        if(game->key == KEY_SPACE ){
            write_move_buffer(game, game->cursor_x, game->cursor_y);
            change_state(game, SELECTED);
            game->key_event = false;

        }
    }
    reset_key_handler(game);
    return game;
}

game_t * manage_selected_state(game_t * game){
    if(!game) return NULL;
    // display_status_bar("Selected");

    navigation_key_handler(game);

    if(game->key_event){
        if(game->key == KEY_SPACE ){
            write_move_buffer(game, game->cursor_x, game->cursor_y);
            change_state(game, MOVED);
            game->key_event = false;
        }
    }
    reset_key_handler(game);
    return game;
}

game_t * manage_moved_state(game_t * game){
    if(!game) return NULL;
    
    navigation_key_handler(game);

    game->score = make_move(game);

    if(game->score != -4000){
        // display_status_bar("Played");
        game->current_index = 0;
        // game->move_count++;
        change_state(game, WAITING);
    }
    
    reset_key_handler(game);
    return game;
}

game_t * manage_waiting_state(game_t * game){
    if(!game) return NULL;

    // navigation_key_handler(game);

    game->score = make_move(game);

    if(game->player_side == AUTO){
        change_state(game, WAITING);
        game->current_index = 0;
        // game->move_count++;
    } else{
        change_state(game, THINKING);
        game->current_index = 0;
        // game->move_count++;
        if(game->score >= 4000 - 1){
            change_state(game, GAME_OVER);
        } 
    }

    reset_key_handler(game);
    return game;
}

game_t * manage_game_over_state(game_t * game){
    if(!game) return NULL;

    reset_key_handler(game);
    return game;
}

game_t * game_send_key(game_t * game, char key){
    if(game){
        game->key = key;
        game->key_event = true;
    }
    return game;
}

void render_chess_cursor(game_t * game){

}

game_t * manage_rendering(game_t * game){ 
    if(!game) return NULL;

    if(game){
        if(game->state == LOBBY){
            render_splash_screen();
        } 
        else if(game->state == MENU){
            render_menu();
        }
        else if(game->state == STARTED){
            fillScreen(0);
            render_scoreboard(game);
        }
        else{
            render_board(game);
            if(game->state == SELECTED){               
                draw_chess_cursor(SQUARE_SIZE, get_source_file(game), get_source_rank(game), 23);
                if(game->valid_move)
                draw_chess_cursor(SQUARE_SIZE, get_dest_file(game), get_dest_rank(game), 200);
                draw_chess_cursor(SQUARE_SIZE, game->cursor_x, game->cursor_y, 25); 
                render_status_bar("Selected");
            } else if(game->state == THINKING){
                //15
                if(game->valid_move){
                    draw_chess_cursor(SQUARE_SIZE, get_source_file(game), get_source_rank(game), 23);
                    draw_chess_cursor(SQUARE_SIZE, get_dest_file(game), get_dest_rank(game), 200);
                    // render_scoreboard(game);
                }
                draw_chess_cursor(SQUARE_SIZE, game->cursor_x, game->cursor_y, 255);
                render_status_bar("Your Turn");
            } else if(game->state == WAITING){
                render_scoreboard(game);
                render_status_bar("Thinking ...");
            } else if(game->state == GAME_OVER){
                render_status_bar("Game Over");
            }
        }
    }
    return game;
}



game_t * manage_game_states(game_t * game){

    if(!game) return NULL;

    switch(game->state){
        case LOBBY:
            manage_lobby_state(game);
            break;

        case MENU:
            manage_menu_state(game);
            break;

        case STARTED:
            manage_started_state(game);
            break;

        case THINKING:
            manage_thinking_state(game);
            break;

        case SELECTED:
            manage_selected_state(game);
            break;

        case MOVED:
            manage_moved_state(game);
            break;
        
        case WAITING:
            manage_waiting_state(game);
            break;

        case GAME_OVER:
            manage_game_over_state(game);
            break;

        default :
            break;
    }

    return game;
}



