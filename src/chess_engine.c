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
#include "game_gfx.h"
#include "system.h"

#define F(I, S, N) for (I = S; I < N; I++)
#define W(A) while (A)
#define K(A, B) * (int * )(Zobrist + A + (B & 8) + S * (B & 7))
#define J(A) K(ToSqr + A, Board[ToSqr]) - K(FromSqr + A, Piece) - K(CaptSqr + A, Victim)

// #define HASHSIZE (1 * 1024)
#define HASHSIZE (1024 * 1024)
// #define STRENGTH 1000
// #define MOVE 10

// #define HASHSIZE 7340040
#define STRENGTH 1000
// #define MOVE 50

struct HashTab_t 
{ int  Key, Score;
  char From, To, Draft;
} HashTab[HASHSIZE];                                   /* hash table, 16M+8 entries*/

int V=112, M=136, S=128, INF=8e3, C=799,               /* V=0x70=rank mask, M=0x88 */
    Rootep, Nodes, i; 
    
char RootEval, InputFrom, InputTo,
PieceVal[] = {0,1,1,3,-1,3,5,9},                       /* relative piece values    */
StepVecs[] =                                           /* step-vector lists        */
         {-16,-15,-17,0,                               /* white Pawn               */
           1,16,0,                                     /* Rook                     */
           1,16,15,17,0,                               /* Q,K, also used for B, bP */
          14,18,31,33,0,                               /* Knight                   */
          7,-1,11,6,8,3,6,                             /* first direction per piece*/
          6,3,5,7,4,5,3,6},                            /* initial piece setup      */
Board[129],                                            /* board: half of 16x8+dummy*/
Zobrist[1035],                                         /* hash translation table   */
  
Sym[] = ".?+nkbrq?*?NKBRQ";                            /* piece symbols on printout*/

int Search(int Side, int Alpha, int Beta, int Eval, int HashKeyLo, int HashKeyHi,
    int epSqr, int LastTo, int Depth) {
    int j, StepVec, BestScore, Score, IterDepth, h, i = 8, SkipSqr, RookSqr;
    char Victim, PieceType, Piece, FromSqr, ToSqr, BestFrom, BestTo, CaptSqr, StartSqr;
    struct HashTab_t * Hash = HashTab;

    /* Hash probe. On miss (Internal) Iterative Deepening starts at IterDepth=0,     */
    /* without move hint. On a hit we start IID from the hashed values, except       */
    /* when the depth is already sufficient. In this case, if the bound type OK,     */
    /* we use the hashed score immediately, and otherwise we just do the last        */
    /* iteration starting with the hash move.                                        */
    /* lookup pos. in hash table*/
    j = (Side * epSqr ^ HashKeyLo) & HASHSIZE - 9; /* try 8 consec. locations  */
    while ((h = HashTab[++j].Key) && h - HashKeyHi && --i); /* first empty or match     */
    Hash += i ? j : 0; /* dummy A[0] if miss & full*/
    if (Hash -> Key) /* hit: pos. is in hash tab */ {
        IterDepth = Hash -> Draft;
        Score = Hash -> Score;
        BestFrom = Hash -> From; /* examine stored data      */
        if (IterDepth >= Depth) /* if depth sufficient:     */ {
            if (Score >= Beta | BestFrom & S &&
                Score <= Alpha | BestFrom & 8
            ) return Score; /* use if window compatible */
            IterDepth = Depth - 1; /* or use as iter. start    */
        }
        BestFrom &= ~0x88;
        BestTo = Hash -> To; /*      with best-move hint */
        BestTo = IterDepth ? BestTo : 0; /* don't try best at d=0    */
    } else IterDepth = BestFrom = BestTo = 0; /* start iter., no best yet */

    Nodes++; /* node count (for timing)  */

    /* We have to search the current node; deepen the search in steps of one         */
    /* starting from the hashed data as if it were the previous iteration.           */
    /* In the root (LastTo==8) we deepen until node count or max. depth reached.     */

    while (IterDepth++ < Depth | /* iterative deepening loop */
        LastTo == 8 & Nodes < STRENGTH & IterDepth < 98) /* at root until node limit */ {
        /* Each iteration consist of a move-generation run, immediately searching      */
        /* all moves as they are generated. The best move from the previous iter-      */
        /* ation (still contained in BestFrom, BestTo) is slipped in front, though.    */
        /* To make this easier, move generation starts at StartSqr = BestFrom.         */
        /* The S-bit of BestTo indicates if there is a valid best move to try.         */
        FromSqr = StartSqr = BestFrom; /* start scan at prev. best  */
        ToSqr |= 8 & ToSqr >> 4; /* request try noncastl. 1st*/
        BestScore = IterDepth > 1 ? -INF : Eval; /* unconsidered:static eval */
        do {
            Piece = Board[FromSqr]; /* scan board looking for   */
            if (Piece & Side) /*  own piece (inefficient!)*/ {
                StepVec = PieceType = Piece & 7; /* set StepVec > 0          */
                j = StepVecs[PieceType + 16]; /* first step vector Piece  */
                while (StepVec = PieceType > 2 & StepVec < 0 ? -StepVec : -StepVecs[++j])
                /* loop over directions o[] */
                {
                    replay: /* resume normal after best */
                        /* For each direction we scan ToSqr along the ray startig at FromSqr.    */
                        ToSqr = FromSqr;
                    SkipSqr = RookSqr = S; /* S = 0x80 = dummy square  */
                    do {
                        CaptSqr = ToSqr += StepVec; /* ToSqr traverses ray      */

                        /* FromSqr, ToSqr here scan through all tentative moves. If there      */
                        /* is an old best move to try first, this is indicated in the 8-bit    */
                        /* of BestTo (which was copied from the S-bit), and we overrule the    */
                        /* generated ToSqr (which might ly at other distance or in direction)  */
                        /* We then test if ToSqr is on the board, if we have an e.p. capture,  */
                        /* are blocked by an own piece, and if Pawn moves are valid.           */
                        if (BestTo & 8)
                            CaptSqr = ToSqr = BestTo & ~0x88; /* sneak-in prev. best move */
                        if (ToSqr & 0x88) break; /* board edge hit (M=0x88)  */
                        if (PieceType < 3 & ToSqr == epSqr)
                            CaptSqr = ToSqr ^ 16; /* shift CaptSqr if e.p.    */
                        Victim = Board[CaptSqr];
                        if (Victim & Side | /* capture own              */
                            PieceType < 3 & !(StepVec & 7) != !Victim /*            bad pawn mode */
                        ) break;

                        /* If we get here, we have a pseudo-legal move in (FromSqr, ToSqr).    */
                        /* We check it for King capture (or Rook capture after castling).      */
                        /* This can give a beta cutoff, as can the stand-pat score in QS.      */
                        i = 99 * PieceVal[Victim & 7]; /* value of victim piece    */
                        if (i < 0 || /* K capt. or               */
                            epSqr - S && Board[epSqr] && /* non-empty epSqr:castling */
                            ToSqr - epSqr < 2 & epSqr - ToSqr < 2 /*             bad castling */
                        ) BestScore = INF;
                        if (BestScore >= Beta) goto cutoff; /* abort on fail high       */

                        /* We now have a move to search. If there is depth left, we different- */
                        /* ially update the evaluation, Make the move, Search it recursively   */
                        /* UnMake it, and update the best score & move. If not, we ignore it.  */
                        if (h = IterDepth - (ToSqr != LastTo)) /* remaining depth(-recapt.)*/ {
                            Score = PieceType < 6 ? Board[FromSqr + 8] - Board[ToSqr + 8] : 0;
                            /* center positional pts.   */

                            /* Make move & evaluate                                              */
                            Board[RookSqr] = Board[CaptSqr] = Board[FromSqr] = 0;
                            Board[ToSqr] = Piece & 31; /* do move, strip virgin-bit*/
                            if (!(RookSqr & 0x88)) {
                                Board[SkipSqr] = Side + 6; /* castling: put Rook       */
                                Score += 30; /*                 & score  */
                            }

                            if (PieceType < 3) /* pawns:                   */ {
                                Score -= /* structure, undefended    */
                                    9 * (((FromSqr - 2) & M || Board[FromSqr - 2] != Piece) + /* squares plus bias */
                                        ((FromSqr + 2) & M || Board[FromSqr + 2] != Piece) - 1);
                                if (ToSqr + StepVec + 1 & S) {
                                    Board[ToSqr] |= 7; /* promote Pawn to Queen,   */
                                    i += C; /*                add score */
                                }
                            }

                            Score = -Search( /* recursive eval. of reply */
                                24 - Side, /* opponent color           */
                                -Beta - (Beta > Eval), /* new Alpha (delayed gain!)*/
                                BestScore > Alpha ? -BestScore : -Alpha, /* New Beta  */
                                -Eval - Score - i, /* New Eval                 */
                                HashKeyLo + J(0), /* New HashKeys             */
                                HashKeyHi + J(8) + SkipSqr - S, /* SkipSqr-S!=0 on castling */
                                SkipSqr, ToSqr, h); /* New epSqr, LastTo, Depth */

                            Score -= Score > Eval; /* delayed-gain penalty     */

                            /* If the Search routine was called as move-legality checker, we     */
                            /* now return before unmaking the move if it was the input move.     */
                            if (LastTo == 9) /* called as move-legality  */
                            /*   checker                */
                            {
                                if (Score != -INF & FromSqr == InputFrom & ToSqr == InputTo) /* move found */ {
                                    RootEval = -Eval - i; /* update eval, material    */
                                    Rootep = SkipSqr;
                                    return Beta; /*   & not in check, signal */
                                }
                                Score = BestScore; /* (prevent fail-lows on    */
                            } /*   K-capt. replies)       */

                            /* UnMake move                                                       */
                            Board[RookSqr] = Side + 38; /* undo move,RookSqr can be */
                            Board[SkipSqr] = Board[ToSqr] = 0; /*                    dummy */
                            Board[FromSqr] = Piece;
                            Board[CaptSqr] = Victim;

                            /* Process score. If the move just searched was a previous best      */
                            /* move that was tried first, we take its Score and redo the first   */
                            /* ray of this piece. Otherwise update best score and move.          */
                            if (BestTo & 8) /* Move just done was in-    */ {
                                BestScore = Score; /*     serted previous best  */
                                BestTo &= ~8;
                                goto replay;
                            } /* redo original first move  */

                            if (Score > BestScore) {
                                BestScore = Score; /* update max,               */
                                BestFrom = FromSqr; /* best move,                */
                                BestTo = ToSqr | S & RookSqr; /* mark non-castling with S  */
                            }
                        }

                        /* Determine if we have to continue scanning this ray. We must stop    */
                        /* on a capture, or if the piece is a non-slider, with the exceptions  */
                        /* of double moves for Pawns and King (castlings!). Such double moves  */
                        /* cause setting of the SkipSqr, that otherwise is equal to the dummy S*/
                        Victim += PieceType < 5; /* fake capt. for nonsliding */
                        if (PieceType < 3 & 6 * Side + (ToSqr & 0x70) == S /* pawn on 3rd/6th, or       */ ||
                            (Piece & ~24) == 36 & /* virgin K,                 */
                            j == 7 && /* moving sideways,          */
                            RookSqr & 0x88 && /* RookSqr not yet set       */
                            Board[RookSqr = (FromSqr | 7) - (StepVec >> 1 & 7)] & 32 &&
                            /* virgin Rook in corner     */
                            !(Board[RookSqr ^ 1] | Board[RookSqr ^ 2]) /* 2 empty sqrs. next to R */
                        ) {
                            SkipSqr = ToSqr;
                            Victim--;
                        } /* unfake capt., enable e.p. */

                        /* continue ray scan if move was non-capture                           */
                    } while (!Victim); /* if not capt. continue ray */

                }
            }
        } while ((FromSqr = FromSqr + 9 & ~0x88) - StartSqr); /* next sqr. of board, wrap  */

        /* All moves have been searched; wrap up iteration by testing for check- or    */
        /* stalemate, which leave Score at -INF. Call Search with Depth=1 after null   */
        /* move to determine if we are in check. Finally store result in hash.         */
        cutoff:
            if (BestScore > INF / 4 | BestScore < -INF / 4)
                IterDepth = 99; /* mate is indep. of depth   */
        BestScore = BestScore + INF ? BestScore : /* best loses K: (stale)mate */
            -Search(24 - Side, -INF, INF, 0, HashKeyLo, HashKeyHi, S, LastTo, 1) / 2;

        if (!Hash -> Key | (Hash -> From & M) != M | Hash -> Draft <= IterDepth)
        /* if new/better type/depth: */
        {
            Hash -> Key = HashKeyHi; /* store in hash,            */
            Hash -> Score = BestScore;
            Hash -> Draft = IterDepth;
            HashTab -> Key = 0; /* dummy stays empty         */
            Hash -> From = BestFrom | /* From & bound type         */
                8 * (BestScore > Alpha) | /*    encoded in X S,8 bits  */
                S * (BestScore < Beta); /* (8=lower, S=upper bound)  */
            Hash -> To = BestTo;
        }

        /* End of iteration. Start new one if more depth was requested                 */
        /*  if(LastTo==8)printf("%2d ply, %9d searched, %6d by (%2x,%2x)\n",
                                    IterDepth-1,Nodes,BestScore,BestFrom,BestTo&0x77); */
    }

    /* return best score (and in root also best move) after all iterations are done. */
    if (LastTo & 8) {
        InputFrom = BestFrom;
        InputTo = BestTo & ~0x88;
    }
    return BestScore;
}


/* 8=white, 16=black  */

int Side = WHITE;     // First move always white  
int player_color = PLAYER_SIDE;

int move_count = 0;



char * get_current_board(void){
    return Board;
}



void init_chess_engine(game_t * game) {
    int j = 0;
    game->move_count = 0;
    game->board = Board;
    Side = WHITE;
    memset(game->board, 0, BOARD_BUFFER_SIZE);
    /* Initialize board and piece-square table (actually just a square table).      */
    for (i = 0; i < 8; i++) /* initial board setup*/ {
        game->board[i] = (game->board[i + 0x70] = StepVecs[i + 24] + 40) + 8; /* Pieces             */
        game->board[i + 0x10] = 18;
        game->board[i + 0x60] = 9; /* black, white Pawns */
        for (j = 0; j < 8; j++) game->board[16 * j + i + 8] = (i - 4) * (i - 4) + (j - 3.5) * (j - 3.5);
        /*common pce-sqr table*/
    } /*(in unused half b[])*/
    for (i = 0x88; i < 1035; i++) {
        Zobrist[i] = rand() >> 9;
    }
}

char unmask_file(int n) { 
  u8 file = 0;
  u8 rank = 0;
  rank = 8 - n / 16;
  file = n + 16 * rank - 127;
  return file + 96;
}

char unmask_rank(int n) { 
  u8 rank = 0;
  rank = 8 - n / 16;
  return rank + 48;
}

int make_move(game_t * game) {

    Nodes = 0;
    int score = 0;

    // if my side then take input
    // otherwise search

    if(game->player_side == game->current_side){
        InputFrom   =   game->move_buffer[0]-16*game->move_buffer[1]+C;                       /* parse entered move */
        InputTo     =   game->move_buffer[2]-16*game->move_buffer[3]+C;
    }
    else {
       // Search(Side, -INF, INF, RootEval, 1, 1, Rootep, 8, 0); /* or think up one    */
        score = Search(game->current_side, -INF, INF, RootEval, 1, 1, Rootep, 8, 0);
    }   

    for (i = 0; i < HASHSIZE; i++) {
        HashTab[i].Key = 0; /* clear hash table   */
    }

    if (Search(game->current_side, -INF, INF, RootEval, 1, 1, Rootep, 9, 2) == INF) { /* check legality & do*/
        game->current_side ^= 24;     // Flip side
        // move_count++;
        game->move_buffer[0] = unmask_file(InputFrom);
        game->move_buffer[1] = unmask_rank(InputFrom);
        game->move_buffer[2] = unmask_file(InputTo);
        game->move_buffer[3] = unmask_rank(InputTo);
        game->valid_move = true;
        game->move_count++;
        // display_scoreboard(score, InputFrom, InputTo, Side, move_count);
    } else{
        game->valid_move = false;
    }

    game->score = score;
    return score;
}

int test_engine(void) {

    return 0;
}