#ifndef _PIECES_H_
#define _PIECES_H_

#include "util.h"

typedef struct piece_gfx_t{
	unsigned char * fill;
	unsigned char * border;
} piece_gfx_t;

struct pieces_t{
	piece_gfx_t  king;
	piece_gfx_t  queen;
	piece_gfx_t  rook;
	piece_gfx_t  bishop;
	piece_gfx_t  knight;
	piece_gfx_t  pawn;
};



struct pieces_t get_pieces();

#endif // _PIECES_H_