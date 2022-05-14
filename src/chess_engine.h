#ifndef _CHESS_ENGINE_H_
#define _CHESS_ENGINE_H_

#include "game.h"

#define PLAYER_SIDE 8
#define WHITE       8
#define BLACK      16
#define AUTO       24
#define PLAYER      0
#define COMPUTER    1

/***************************************************************************/
/*                               micro-Max,                                */
/* A chess program smaller than 2KB (of non-blank source), by H.G. Muller  */
/***************************************************************************/
/* version 3.2 (2000 characters) features:                                 */
/* - recursive negamax search                                              */
/* - quiescence search with recaptures                                     */
/* - recapture extensions                                                  */
/* - (internal) iterative deepening                                        */
/* - best-move-first 'sorting'                                             */
/* - a hash table storing score and best move                              */
/* - full FIDE rules (expt under-promotion) and move-legality checking     */

/* This version is for reference only, with all variable names changed to  */
/* be more indicative of their meaning, and better layout & more comments. */
/* There is no guarantee, though, that it still compiles or runs.          */

// #include <stdio.h>
// #include <math.h>

#include "Adafruit_GFX.h"
#include "chess_engine.h"
#include "game.h"
#include "game_gfx.h"
#include "system.h"


char * get_current_board(void);
void init_chess_engine(game_t * game);
int make_move(game_t * game);

#endif	// _CHESS_ENGINE_H_