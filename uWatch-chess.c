/*
 *	DEFS.H
 *	Tom Kerrigan's Simple Chess Program (TSCP)
 *
 *	Copyright 1997 Tom Kerrigan
 */


#include <setjmp.h>

#pragma pack(push, 1)

#define USE_BOOKxx
#define USE_HISTORYxx

#define TRUE			1
#define FALSE			0

#define HIST_STACK		96
#define MAX_PLY			12
#define GEN_STACK		512

#define LIGHT			0
#define DARK			1

#define PAWN			0
#define KNIGHT			1
#define BISHOP			2
#define ROOK			3
#define QUEEN			4
#define KING			5

#define EMPTY			6

#define CAPTURE_SCORE           1000
#define HIGHEST_SCORE           10000

/* useful squares */
#define A1				56
#define B1				57
#define C1				58
#define D1				59
#define E1				60
#define F1				61
#define G1				62
#define H1				63
#define A8				0
#define B8				1
#define C8				2
#define D8				3
#define E8				4
#define F8				5
#define G8				6
#define H8				7

#define ROW(x)			(x >> 3)
#define COL(x)			(x & 7)

/* This is the basic description of a move. promote is what
   piece to promote the pawn to, if the move is a pawn
   promotion. bits is a bitfield that describes the move,
   with the following bits:

   1	capture
   2	castle
   4	en passant capture
   8	pushing a pawn 2 squares
   16	pawn move
   32	promote

   It's union'ed with an integer so two moves can easily
   be compared with each other. */

typedef long int4;  /* 32 bit int */

typedef struct 
{
    char from;
    char to;
    char promote;
    char bits;
} move_bytes;

typedef union
{
	move_bytes b;
	int4 u;
} move;

/* an element of the move stack. it's just a move with a
   score, so it can be sorted by the search functions. */
typedef struct 
{
    move m;
    short score;
} gen_t;

/* an element of the history stack, with the information
   necessary to take a move back. */
typedef struct 
{
    move m;
    unsigned char capture;
    unsigned char fifty;
    short ep;
    unsigned char castle;
} hist_t;


/* the board representation */
unsigned char color[64];  /* LIGHT, DARK, or EMPTY */
unsigned char piece[64];  /* PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, or EMPTY */
int side;  /* the side to move */
int xside;  /* the side not to move */
unsigned char castle;  /* a bitfield with the castle permissions. if 1 is set,
                white can still castle kingside. 2 is white queenside.
				4 is black kingside. 8 is black queenside. */
short ep;  /* the en passant square. if white moves e2e4, the en passant
            square is set to e3, because that's where a pawn would move
			in an en passant capture */
int fifty;  /* the number of moves since a capture or pawn move, used
               to handle the fifty-move-draw rule */

int ply;  /* the number of half-moves (ply) since the
             root of the search tree */
int hply;  /* h for history; the number of ply since the beginning
              of the game */

/* gen_dat is some memory for move lists that are created by the move
   generators. The move list for ply n starts at first_move[n] and ends
   at first_move[n + 1]. */
gen_t gen_dat[GEN_STACK];
unsigned short first_move[MAX_PLY];

#ifdef USE_HISTORY
/* the history heuristic array (used for move ordering) */
int history[64][64];
#endif

/* we need an array of hist_t's so we can take back the
   moves we make */
hist_t hist_dat[HIST_STACK];

/* the engine will search for max_time milliseconds or until it finishes
   searching max_depth ply. */
int4 max_time;
int max_depth;

/* the time when the engine starts searching, and when it should stop */
int4 start_time;
int4 stop_time;

int nodes;  /* the number of nodes we've searched */

/* a "triangular" PV array; for a good explanation of why a triangular
   array is needed, see "How Computers Play Chess" by Levy and Newborn. */
move pv[MAX_PLY][MAX_PLY];
unsigned short pv_length[MAX_PLY];
BOOL follow_pv;

/* Now we have the mailbox array, so called because it looks like a
   mailbox, at least according to Bob Hyatt. This is useful when we
   need to figure out what pieces can go where. Let's say we have a
   rook on square a4 (32) and we want to know if it can move one
   square to the left. We subtract 1, and we get 31 (h5). The rook
   obviously can't move to h5, but we don't know that without doing
   a lot of annoying work. Sooooo, what we do is figure out a4's
   mailbox number, which is 61. Then we subtract 1 from 61 (60) and
   see what mailbox[60] is. In this case, it's -1, so it's out of
   bounds and we can forget it. You can see how mailbox[] is used
   in attack() in board.c. */

const signed char mailbox[120] = 
{
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
        -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
        -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
        -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
        -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
        -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
        -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
        -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

const unsigned char mailbox64[64] = 
{
	21, 22, 23, 24, 25, 26, 27, 28,
	31, 32, 33, 34, 35, 36, 37, 38,
	41, 42, 43, 44, 45, 46, 47, 48,
	51, 52, 53, 54, 55, 56, 57, 58,
	61, 62, 63, 64, 65, 66, 67, 68,
	71, 72, 73, 74, 75, 76, 77, 78,
	81, 82, 83, 84, 85, 86, 87, 88,
	91, 92, 93, 94, 95, 96, 97, 98
};


/* slide, offsets, and offset are basically the vectors that
   pieces can move in. If slide for the piece is FALSE, it can
   only move one square in any one direction. offsets is the
   number of directions it can move in, and offset is an array
   of the actual directions. */

const BOOL slide[6] = 
{
	FALSE, FALSE, TRUE, TRUE, TRUE, FALSE
};

const unsigned char offsets[6] = 
{
	0, 8, 4, 4, 8, 8
};

const signed char offset[6][8] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -21, -19, -12, -8, 8, 12, 19, 21 },
	{ -11, -9, 9, 11, 0, 0, 0, 0 },
	{ -10, -1, 1, 10, 0, 0, 0, 0 },
	{ -11, -10, -9, -1, 1, 9, 10, 11 },
	{ -11, -10, -9, -1, 1, 9, 10, 11 }
};


/* This is the castle_mask array. We can use it to determine
   the castling permissions after a move. What we do is
   logical-AND the castle bits with the castle_mask bits for
   both of the move's squares. Let's say castle is 1, meaning
   that white can still castle kingside. Now we play a move
   where the rook on h1 gets captured. We AND castle with
   castle_mask[63], so we have 1&14, and castle becomes 0 and
   white can't castle kingside anymore. */

const unsigned char castle_mask[64] = 
{
	 7, 15, 15, 15,  3, 15, 15, 11,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	13, 15, 15, 15, 12, 15, 15, 14
};


/* the piece letters, for print_board() */
const char piece_char[6] = 
{
	'P', 'N', 'B', 'R', 'Q', 'K'
};


/* the initial board state */

const unsigned char init_color[64] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

const unsigned char init_piece[64] = {
	3, 1, 2, 4, 5, 2, 1, 3,
	0, 0, 0, 0, 0, 0, 0, 0,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	0, 0, 0, 0, 0, 0, 0, 0,
	3, 1, 2, 4, 5, 2, 1, 3
};


/*************** BOARD FUNCTIONS ****************************************/

/* board.c */
void init_board();
BOOL in_check(int s);
BOOL attack(int sq, int s);
void gen();
void gen_caps();
void gen_push(int from, int to, int bits);
void gen_promote(int from, int to, int bits);
BOOL makemove(move_bytes m);
void takeback();
char *move_str(int4);


/* init_board() sets the board to the initial game state. */
void init_board()
{
    int i;

    for (i = 0; i < 64; ++i) {
        color[i] = init_color[i];
        piece[i] = init_piece[i];
    }
    side = LIGHT;
    xside = DARK;
    castle = 15;
    ep = -1;
    fifty = 0;
    ply = 0;
    hply = 0;
    first_move[0] = 0;
}

/* in_check() returns TRUE if side s is in check and FALSE
   otherwise. It just scans the board to find side s's king
   and calls attack() to see if it's being attacked. */

BOOL in_check(int s)
{
    int i;
    for (i = 0; i < 64; ++i)
        if (piece[i] == KING && color[i] == s)
            return attack(i, s ^ 1);
    return TRUE;  /* shouldn't get here */
}


/* attack() returns TRUE if square sq is being attacked by side
   s and FALSE otherwise. */

BOOL attack(int sq, int s)
{
    int i, j, n;

    for (i = 0; i < 64; ++i)
        if (color[i] == s) {
            if (piece[i] == PAWN) {
                if (s == LIGHT) {
                    if (COL(i) != 0 && i - 9 == sq)
                        return TRUE;
                    if (COL(i) != 7 && i - 7 == sq)
                        return TRUE;
                }
                else {
                    if (COL(i) != 0 && i + 7 == sq)
                        return TRUE;
                    if (COL(i) != 7 && i + 9 == sq)
                        return TRUE;
                }
            }
            else
                for (j = 0; j < offsets[piece[i]]; ++j)
                    for (n = i;;) {
                        n = mailbox[mailbox64[n] + offset[piece[i]][j]];
                        if (n == -1)
                            break;
                        if (n == sq)
                            return TRUE;
                        if (color[n] != EMPTY)
                            break;
                        if (!slide[piece[i]])
                            break;
                    }
        }
    return FALSE;
}


/* gen() generates pseudo-legal moves for the current position.
   It scans the board to find friendly pieces and then determines
   what squares they attack. When it finds a piece/square
   combination, it calls gen_push to put the move on the "move
   stack." */

void gen()
{
    int i, j, n;

    /* so far, we have no moves for the current ply */
    first_move[ply + 1] = first_move[ply];

    for (i = 0; i < 64; ++i)
        if (color[i] == side) {
            if (piece[i] == PAWN) {
                if (side == LIGHT) {
                    if (COL(i) != 0 && color[i - 9] == DARK)
                        gen_push(i, i - 9, 17);
                    if (COL(i) != 7 && color[i - 7] == DARK)
                        gen_push(i, i - 7, 17);
                    if (color[i - 8] == EMPTY) {
                        gen_push(i, i - 8, 16);
                        if (i >= 48 && color[i - 16] == EMPTY)
                            gen_push(i, i - 16, 24);
                    }
                }
                else {
                    if (COL(i) != 0 && color[i + 7] == LIGHT)
                        gen_push(i, i + 7, 17);
                    if (COL(i) != 7 && color[i + 9] == LIGHT)
                        gen_push(i, i + 9, 17);
                    if (color[i + 8] == EMPTY) {
                        gen_push(i, i + 8, 16);
                        if (i <= 15 && color[i + 16] == EMPTY)
                            gen_push(i, i + 16, 24);
                    }
                }
            }
            else
                for (j = 0; j < offsets[piece[i]]; ++j)
                    for (n = i;;) {
                        n = mailbox[mailbox64[n] + offset[piece[i]][j]];
                        if (n == -1)
                            break;
                        if (color[n] != EMPTY) {
                            if (color[n] == xside)
                                gen_push(i, n, 1);
                            break;
                        }
                        gen_push(i, n, 0);
                        if (!slide[piece[i]])
                            break;
                    }
        }

    /* generate castle moves */
    if (side == LIGHT) {
        if (castle & 1)
            gen_push(E1, G1, 2);
        if (castle & 2)
            gen_push(E1, C1, 2);
    }
    else {
        if (castle & 4)
            gen_push(E8, G8, 2);
        if (castle & 8)
            gen_push(E8, C8, 2);
    }
	
    /* generate en passant moves */
    if (ep != -1) {
        if (side == LIGHT) {
            if (COL(ep) != 0 && color[ep + 7] == LIGHT && piece[ep + 7] == PAWN)
                gen_push(ep + 7, ep, 21);
            if (COL(ep) != 7 && color[ep + 9] == LIGHT && piece[ep + 9] == PAWN)
                gen_push(ep + 9, ep, 21);
        }
        else {
            if (COL(ep) != 0 && color[ep - 9] == DARK && piece[ep - 9] == PAWN)
                gen_push(ep - 9, ep, 21);
            if (COL(ep) != 7 && color[ep - 7] == DARK && piece[ep - 7] == PAWN)
                gen_push(ep - 7, ep, 21);
        }
    }
}


/* gen_caps() is basically a copy of gen() that's modified to
   only generate capture and promote moves. It's used by the
   quiescence search. */

void gen_caps()
{
    int i, j, n;

    first_move[ply + 1] = first_move[ply];
    for (i = 0; i < 64; ++i)
        if (color[i] == side) {
            if (piece[i]==PAWN) {
                if (side == LIGHT) {
                    if (COL(i) != 0 && color[i - 9] == DARK)
                        gen_push(i, i - 9, 17);
                    if (COL(i) != 7 && color[i - 7] == DARK)
                        gen_push(i, i - 7, 17);
                    if (i <= 15 && color[i - 8] == EMPTY)
                        gen_push(i, i - 8, 16);
                }
                if (side == DARK) {
                    if (COL(i) != 0 && color[i + 7] == LIGHT)
                        gen_push(i, i + 7, 17);
                    if (COL(i) != 7 && color[i + 9] == LIGHT)
                        gen_push(i, i + 9, 17);
                    if (i >= 48 && color[i + 8] == EMPTY)
                        gen_push(i, i + 8, 16);
                }
            }
            else
                for (j = 0; j < offsets[piece[i]]; ++j)
                    for (n = i;;) {
                        n = mailbox[mailbox64[n] + offset[piece[i]][j]];
                        if (n == -1)
                            break;
                        if (color[n] != EMPTY) {
                            if (color[n] == xside)
                                gen_push(i, n, 1);
                            break;
                        }
                        if (!slide[piece[i]])
                            break;
                    }
        }
    if (ep != -1) {
        if (side == LIGHT) {
            if (COL(ep) != 0 && color[ep + 7] == LIGHT && piece[ep + 7] == PAWN)
                gen_push(ep + 7, ep, 21);
            if (COL(ep) != 7 && color[ep + 9] == LIGHT && piece[ep + 9] == PAWN)
                gen_push(ep + 9, ep, 21);
        }
        else {
            if (COL(ep) != 0 && color[ep - 9] == DARK && piece[ep - 9] == PAWN)
                gen_push(ep - 9, ep, 21);
            if (COL(ep) != 7 && color[ep - 7] == DARK && piece[ep - 7] == PAWN)
                gen_push(ep - 7, ep, 21);
        }
    }
}


/* gen_push() puts a move on the move stack, unless it's a
   pawn promotion that needs to be handled by gen_promote().
   It also assigns a score to the move for alpha-beta move
   ordering. If the move is a capture, it uses MVV/LVA
   (Most Valuable Victim/Least Valuable Attacker). Otherwise,
   it uses the move's history heuristic value. Note that
   1,000,000 is added to a capture move's score, so it
   always gets ordered above a "normal" move. */

void gen_push(int from, int to, int bits)
{
    gen_t *g;
    unsigned short* gi;
	
    if (bits & 16) {
        if (side == LIGHT) {
            if (to <= H8) {
                gen_promote(from, to, bits);
                return;
            }
        }
        else {
            if (to >= A1) {
                gen_promote(from, to, bits);
                return;
            }
        }
    }

    gi = &first_move[ply + 1];
    
#if 0
    {
        if (*gi >= GEN_STACK)
        {
            printf("gen move max hit!\n");
        }
    }
#endif

    if (*gi < GEN_STACK)
    {
        g = &gen_dat[(*gi)++];
        g->m.b.from = (char)from;
        g->m.b.to = (char)to;
        g->m.b.promote = 0;
        g->m.b.bits = (char)bits;
        if (color[to] != EMPTY)
            g->score = CAPTURE_SCORE + (piece[to] * 10) - piece[from];
        else
        {
#ifdef USE_HISTORY
            g->score = history[from][to];
#else
            g->score = 0;
#endif
        }
    }
}


/* gen_promote() is just like gen_push(), only it puts 4 moves
   on the move stack, one for each possible promotion piece */

void gen_promote(int from, int to, int bits)
{
    int i;
    gen_t *g;
	
    for (i = KNIGHT; i <= QUEEN; ++i) {
        g = &gen_dat[first_move[ply + 1]++];
        g->m.b.from = (char)from;
        g->m.b.to = (char)to;
        g->m.b.promote = (char)i;
        g->m.b.bits = (char)(bits | 32);
        g->score = CAPTURE_SCORE + (i * 10);
    }
}


/* makemove() makes a move. If the move is illegal, it
   undoes whatever it did and returns FALSE. Otherwise, it
   returns TRUE. */

BOOL makemove(move_bytes m)
{
	
    /* test to see if a castle move is legal and move the rook
       (the king is moved with the usual move code later) */
    if (m.bits & 2) {
        int from, to;

        if (in_check(side))
            return FALSE;
        switch (m.to) {
        case 62:
            if (color[F1] != EMPTY || color[G1] != EMPTY ||
                attack(F1, xside) || attack(G1, xside))
                return FALSE;
            from = H1;
            to = F1;
            break;
        case 58:
            if (color[B1] != EMPTY || color[C1] != EMPTY || color[D1] != EMPTY ||
                attack(C1, xside) || attack(D1, xside))
                return FALSE;
            from = A1;
            to = D1;
            break;
        case 6:
            if (color[F8] != EMPTY || color[G8] != EMPTY ||
                attack(F8, xside) || attack(G8, xside))
                return FALSE;
            from = H8;
            to = F8;
            break;
        case 2:
            if (color[B8] != EMPTY || color[C8] != EMPTY || color[D8] != EMPTY ||
                attack(C8, xside) || attack(D8, xside))
                return FALSE;
            from = A8;
            to = D8;
            break;
        default:  /* shouldn't get here */
            from = -1;
            to = -1;
            break;
        }
        color[to] = color[from];
        piece[to] = piece[from];
        color[from] = EMPTY;
        piece[from] = EMPTY;
    }

    /* back up information so we can take the move back later. */

#if 0
    {
        static int maxh;
        if (hply > maxh)
        {
            maxh = hply;
            printf("hist=%d\n", maxh);
        }
    }
#endif

    hist_dat[hply].m.b = m;
    hist_dat[hply].capture = piece[(int)m.to];
    hist_dat[hply].castle = castle;
    hist_dat[hply].ep = ep;
    hist_dat[hply].fifty = fifty;
    ++ply;
    ++hply;

    /* update the castle, en passant, and
       fifty-move-draw variables */
    castle &= castle_mask[(int)m.from] & castle_mask[(int)m.to];
    if (m.bits & 8) {
        if (side == LIGHT)
            ep = m.to + 8;
        else
            ep = m.to - 8;
    }
    else
        ep = -1;
    if (m.bits & 17)
        fifty = 0;
    else
        ++fifty;

    /* move the piece */
    color[(int)m.to] = side;
    if (m.bits & 32)
        piece[(int)m.to] = m.promote;
    else
        piece[(int)m.to] = piece[(int)m.from];
    color[(int)m.from] = EMPTY;
    piece[(int)m.from] = EMPTY;

    /* erase the pawn if this is an en passant move */
    if (m.bits & 4) {
        if (side == LIGHT) {
            color[m.to + 8] = EMPTY;
            piece[m.to + 8] = EMPTY;
        }
        else {
            color[m.to - 8] = EMPTY;
            piece[m.to - 8] = EMPTY;
        }
    }

    /* switch sides and test for legality (if we can capture
       the other guy's king, it's an illegal position and
       we need to take the move back) */
    side ^= 1;
    xside ^= 1;
    if (in_check(xside)) {
        takeback();
        return FALSE;
    }
    return TRUE;
}


/* takeback() is very similar to makemove(), only backwards :)  */
void takeback()
{
    move_bytes m;

    side ^= 1;
    xside ^= 1;
    --ply;
    --hply;
    m = hist_dat[hply].m.b;
    castle = hist_dat[hply].castle;
    ep = hist_dat[hply].ep;
    fifty = hist_dat[hply].fifty;
    color[(int)m.from] = side;
    if (m.bits & 32)
        piece[(int)m.from] = PAWN;
    else
        piece[(int)m.from] = piece[(int)m.to];
    if (hist_dat[hply].capture == EMPTY) {
        color[(int)m.to] = EMPTY;
        piece[(int)m.to] = EMPTY;
    }
    else {
        color[(int)m.to] = xside;
        piece[(int)m.to] = hist_dat[hply].capture;
    }
    if (m.bits & 2) {
        int from, to;

        switch(m.to) {
        case 62:
            from = F1;
            to = H1;
            break;
        case 58:
            from = D1;
            to = A1;
            break;
        case 6:
            from = F8;
            to = H8;
            break;
        case 2:
            from = D8;
            to = A8;
            break;
        default:  /* shouldn't get here */
            from = -1;
            to = -1;
            break;
        }
        color[to] = side;
        piece[to] = ROOK;
        color[from] = EMPTY;
        piece[from] = EMPTY;
    }
    if (m.bits & 4) {
        if (side == LIGHT) {
            color[m.to + 8] = xside;
            piece[m.to + 8] = PAWN;
        }
        else {
            color[m.to - 8] = xside;
            piece[m.to - 8] = PAWN;
        }
    }
}

/************* SEARCH FUNCTIONS ****************************************/

void think();
int search(int alpha, int beta, int depth);
int quiesce(int alpha, int beta);
int reps();
void sort_pv();
void sort(int from);
void checkup();
int eval();

extern int4 get_ms();

/* see the beginning of think() */
static jmp_buf env;
BOOL stop_search;

/* think() calls search() iteratively. Search statistics */
void think()
{
    int i, x;

#if USE_BOOK
    /* try the opening book first */
    pv[0][0].u = book_move();
    if (pv[0][0].u != -1)
        return;
#endif

    /* some code that lets us longjmp back here and return
       from think() when our time is up */
    stop_search = FALSE;
    setjmp(env);
    if (stop_search) 
    {
        /* make sure to take back the line we were searching */
        while (ply)
            takeback();
        return;
    }

    start_time = get_ms();
    stop_time = start_time + max_time;

    ply = 0;
    nodes = 0;

    memset(pv, 0, sizeof(pv));
#ifdef USE_HISTORY
    memset(history, 0, sizeof(history));
#endif

#ifdef DEBUG_MODE
    printf("ply      nodes  score  pv\n");
#endif

    for (i = 1; i <= max_depth; ++i) 
    {
        follow_pv = TRUE;
        x = search(-10000, 10000, i);

#ifdef DEBUG_MODE
        {
            int j;
            printf("%3d  %9d  %5d ", i, nodes, x);
            for (j = 0; j < pv_length[0]; ++j)
                printf(" %s", move_str(pv[0][j].u));
            printf("\n");
            fflush(stdout);
        }
#endif
        if (x > 9000 || x < -9000)
            break;
    }
}


/* search() does just that, in negamax fashion */

int search(int alpha, int beta, int depth)
{
    int i, j, x;
    BOOL c, f;

    /* we're as deep as we want to be; call quiesce() to get
       a reasonable score and return it. */
    if (!depth)
        return quiesce(alpha,beta);
    ++nodes;

    /* do some housekeeping every 1024 nodes */
    if ((nodes & 1023) == 0)
        checkup();

    pv_length[ply] = ply;

    /* are we too deep? */
    if (ply >= MAX_PLY - 1 || hply >= HIST_STACK - 1)
    {
#ifdef DEBUG_MODE
        printf("ply or hist reached!\n");
#endif
        return eval();
    }

    /* are we in check? if so, we want to search deeper */
    c = in_check(side);
    if (c)
        ++depth;
    gen();
    if (follow_pv)  /* are we following the PV? */
        sort_pv();
    f = FALSE;

    /* loop through the moves */
    for (i = first_move[ply]; i < first_move[ply + 1]; ++i) 
    {
        sort(i);
        if (!makemove(gen_dat[i].m.b))
            continue;
        f = TRUE;
        x = -search(-beta, -alpha, depth - 1);
        takeback();
        if (x > alpha) 
        {
#ifdef USE_HISTORY
            /* this move caused a cutoff, so increase the history
               value so it gets ordered high next time we can
               search it */
            history[(int)gen_dat[i].m.b.from][(int)gen_dat[i].m.b.to] += depth;
#endif
            if (x >= beta)
                return beta;
            alpha = x;

            /* update the PV */
            pv[ply][ply] = gen_dat[i].m;
            for (j = ply + 1; j < pv_length[ply + 1]; ++j)
                pv[ply][j] = pv[ply + 1][j];
            pv_length[ply] = pv_length[ply + 1];
        }
    }

    /* no legal moves? then we're in checkmate or stalemate */
    if (!f) 
    {
        if (c)
            return -10000 + ply;
        else
            return 0;
    }

    /* fifty move draw rule */
    if (fifty >= 100)
        return 0;
    return alpha;
}


/* quiesce() is a recursive minimax search function with
   alpha-beta cutoffs. In other words, negamax. It basically
   only searches capture sequences and allows the evaluation
   function to cut the search off (and set alpha). The idea
   is to find a position where there isn't a lot going on
   so the static evaluation function will work. */

int quiesce(int alpha,int beta)
{
    int i, j, x;

    ++nodes;

    /* do some housekeeping every 1024 nodes */
    if ((nodes & 1023) == 0)
        checkup();

    pv_length[ply] = ply;

    /* are we too deep? */
    if (ply >= MAX_PLY - 1)
        return eval();
    if (hply >= HIST_STACK - 1)
        return eval();

    /* check with the evaluation function */
    x = eval();
    if (x >= beta)
        return beta;
    if (x > alpha)
        alpha = x;

    gen_caps();
    if (follow_pv)  /* are we following the PV? */
        sort_pv();

    /* loop through the moves */
    for (i = first_move[ply]; i < first_move[ply + 1]; ++i) 
    {
        sort(i);
        if (!makemove(gen_dat[i].m.b))
            continue;
        x = -quiesce(-beta, -alpha);
        takeback();
        if (x > alpha) {
            if (x >= beta)
                return beta;
            alpha = x;

            /* update the PV */
            pv[ply][ply] = gen_dat[i].m;
            for (j = ply + 1; j < pv_length[ply + 1]; ++j)
                pv[ply][j] = pv[ply + 1][j];
            pv_length[ply] = pv_length[ply + 1];
        }
    }
    return alpha;
}

/* sort_pv() is called when the search function is following
   the PV (Principal Variation). It looks through the current
   ply's move list to see if the PV move is there. If so,
   it adds 10,000,000 to the move's score so it's played first
   by the search function. If not, follow_pv remains FALSE and
   search() stops calling sort_pv(). */

void sort_pv()
{
    int i;
    follow_pv = FALSE;
    for(i = first_move[ply]; i < first_move[ply + 1]; ++i)
        if (gen_dat[i].m.u == pv[0][ply].u) 
        {
            follow_pv = TRUE;
            gen_dat[i].score += HIGHEST_SCORE;
            return;
        }
}


/* sort() searches the current ply's move list from 'from'
   to the end to find the move with the highest score. Then it
   swaps that move and the 'from' move so the move with the
   highest score gets searched next, and hopefully produces
   a cutoff. */

void sort(int from)
{
    int i;
    int bs;  /* best score */
    int bi;  /* best i */
    gen_t g;

    bs = -1;
    bi = from;
    for (i = from; i < first_move[ply + 1]; ++i)
        if (gen_dat[i].score > bs) 
        {
            bs = gen_dat[i].score;
            bi = i;
        }
    g = gen_dat[from];
    gen_dat[from] = gen_dat[bi];
    gen_dat[bi] = g;
}


/* checkup() is called once in a while during the search. */
void checkup()
{
    /* is the engine's time up? if so, longjmp back to the
       beginning of think() */
    if (get_ms() >= stop_time) 
    {
        stop_search = TRUE;
        longjmp(env, 0);
    }
}

/***************** EVAL FUNCTIONS ***************************************/

int eval_light_pawn(int sq);
int eval_dark_pawn(int sq);
int eval_light_king(int sq);
int eval_lkp(int f);
int eval_dark_king(int sq);
int eval_dkp(int f);

#define DOUBLED_PAWN_PENALTY		10
#define ISOLATED_PAWN_PENALTY		20
#define BACKWARDS_PAWN_PENALTY		8
#define PASSED_PAWN_BONUS		20
#define ROOK_SEMI_OPEN_FILE_BONUS	10
#define ROOK_OPEN_FILE_BONUS		15
#define ROOK_ON_SEVENTH_BONUS		20

/* the values of the pieces */
const unsigned short piece_value[6] = 
{
    100, 300, 300, 500, 900, 0
};

/* The "pcsq" arrays are piece/square tables. They're values
   added to the material value of the piece based on the
   location of the piece. */

const signed char pawn_pcsq[64] = {
	  0,   0,   0,   0,   0,   0,   0,   0,
	  5,  10,  15,  20,  20,  15,  10,   5,
	  4,   8,  12,  16,  16,  12,   8,   4,
	  3,   6,   9,  12,  12,   9,   6,   3,
	  2,   4,   6,   8,   8,   6,   4,   2,
	  1,   2,   3, -10, -10,   3,   2,   1,
	  0,   0,   0, -40, -40,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0
};

const signed char knight_pcsq[64] = {
	-10, -10, -10, -10, -10, -10, -10, -10,
	-10,   0,   0,   0,   0,   0,   0, -10,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-10,   0,   0,   0,   0,   0,   0, -10,
	-10, -30, -10, -10, -10, -10, -30, -10
};

const signed char bishop_pcsq[64] = {
	-10, -10, -10, -10, -10, -10, -10, -10,
	-10,   0,   0,   0,   0,   0,   0, -10,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-10,   0,   0,   0,   0,   0,   0, -10,
	-10, -10, -20, -10, -10, -20, -10, -10
};

const signed char king_pcsq[64] = {
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-20, -20, -20, -20, -20, -20, -20, -20,
	  0,  20,  40, -20,   0, -20,  40,  20
};

const signed char king_endgame_pcsq[64] = {
	  0,  10,  20,  30,  30,  20,  10,   0,
	 10,  20,  30,  40,  40,  30,  20,  10,
	 20,  30,  40,  50,  50,  40,  30,  20,
	 30,  40,  50,  60,  60,  50,  40,  30,
	 30,  40,  50,  60,  60,  50,  40,  30,
	 20,  30,  40,  50,  50,  40,  30,  20,
	 10,  20,  30,  40,  40,  30,  20,  10,
	  0,  10,  20,  30,  30,  20,  10,   0
};

/* The flip array is used to calculate the piece/square
   values for DARK pieces. The piece/square value of a
   LIGHT pawn is pawn_pcsq[sq] and the value of a DARK
   pawn is pawn_pcsq[flip[sq]] */
const unsigned char flip[64] = {
	 56,  57,  58,  59,  60,  61,  62,  63,
	 48,  49,  50,  51,  52,  53,  54,  55,
	 40,  41,  42,  43,  44,  45,  46,  47,
	 32,  33,  34,  35,  36,  37,  38,  39,
	 24,  25,  26,  27,  28,  29,  30,  31,
	 16,  17,  18,  19,  20,  21,  22,  23,
	  8,   9,  10,  11,  12,  13,  14,  15,
	  0,   1,   2,   3,   4,   5,   6,   7
};

/* pawn_rank[x][y] is the rank of the least advanced pawn of color x on file
   y - 1. There are "buffer files" on the left and right to avoid special-case
   logic later. If there's no pawn on a rank, we pretend the pawn is
   impossibly far advanced (0 for LIGHT and 7 for DARK). This makes it easy to
   test for pawns on a rank and it simplifies some pawn evaluation code. */
int pawn_rank[2][10];

int piece_mat[2];  /* the value of a side's pieces */
int pawn_mat[2];  /* the value of a side's pawns */

int eval()
{
    int i;
    int f;  /* file */
    int score[2];  /* each side's score */

    /* this is the first pass: set up pawn_rank, piece_mat, and pawn_mat. */
    for (i = 0; i < 10; ++i) {
        pawn_rank[LIGHT][i] = 0;
        pawn_rank[DARK][i] = 7;
    }
    piece_mat[LIGHT] = 0;
    piece_mat[DARK] = 0;
    pawn_mat[LIGHT] = 0;
    pawn_mat[DARK] = 0;
    for (i = 0; i < 64; ++i) {
        if (color[i] == EMPTY)
            continue;
        if (piece[i] == PAWN) {
            pawn_mat[color[i]] += piece_value[PAWN];
            f = COL(i) + 1;  /* add 1 because of the extra file in the array */
            if (color[i] == LIGHT) {
                if (pawn_rank[LIGHT][f] < ROW(i))
                    pawn_rank[LIGHT][f] = ROW(i);
            }
            else {
                if (pawn_rank[DARK][f] > ROW(i))
                    pawn_rank[DARK][f] = ROW(i);
            }
        }
        else
            piece_mat[color[i]] += piece_value[piece[i]];
    }

    /* this is the second pass: evaluate each piece */
    score[LIGHT] = piece_mat[LIGHT] + pawn_mat[LIGHT];
    score[DARK] = piece_mat[DARK] + pawn_mat[DARK];
    for (i = 0; i < 64; ++i) {
        if (color[i] == EMPTY)
            continue;
        if (color[i] == LIGHT) {
            switch (piece[i]) {
            case PAWN:
                score[LIGHT] += eval_light_pawn(i);
                break;
            case KNIGHT:
                score[LIGHT] += knight_pcsq[i];
                break;
            case BISHOP:
                score[LIGHT] += bishop_pcsq[i];
                break;
            case ROOK:
                if (pawn_rank[LIGHT][COL(i) + 1] == 0) {
                    if (pawn_rank[DARK][COL(i) + 1] == 7)
                        score[LIGHT] += ROOK_OPEN_FILE_BONUS;
                    else
                        score[LIGHT] += ROOK_SEMI_OPEN_FILE_BONUS;
                }
                if (ROW(i) == 1)
                    score[LIGHT] += ROOK_ON_SEVENTH_BONUS;
                break;
            case KING:
                if (piece_mat[DARK] <= 1200)
                    score[LIGHT] += king_endgame_pcsq[i];
                else
                    score[LIGHT] += eval_light_king(i);
                break;
            }
        }
        else {
            switch (piece[i]) {
            case PAWN:
                score[DARK] += eval_dark_pawn(i);
                break;
            case KNIGHT:
                score[DARK] += knight_pcsq[flip[i]];
                break;
            case BISHOP:
                score[DARK] += bishop_pcsq[flip[i]];
                break;
            case ROOK:
                if (pawn_rank[DARK][COL(i) + 1] == 7) {
                    if (pawn_rank[LIGHT][COL(i) + 1] == 0)
                        score[DARK] += ROOK_OPEN_FILE_BONUS;
                    else
                        score[DARK] += ROOK_SEMI_OPEN_FILE_BONUS;
                }
                if (ROW(i) == 6)
                    score[DARK] += ROOK_ON_SEVENTH_BONUS;
                break;
            case KING:
                if (piece_mat[LIGHT] <= 1200)
                    score[DARK] += king_endgame_pcsq[flip[i]];
                else
                    score[DARK] += eval_dark_king(i);
                break;
            }
        }
    }

    /* the score[] array is set, now return the score relative
       to the side to move */
    if (side == LIGHT)
        return score[LIGHT] - score[DARK];
    return score[DARK] - score[LIGHT];
}

int eval_light_pawn(int sq)
{
    int r;  /* the value to return */
    int f;  /* the pawn's file */

    r = 0;
    f = COL(sq) + 1;

    r += pawn_pcsq[sq];

    /* if there's a pawn behind this one, it's doubled */
    if (pawn_rank[LIGHT][f] > ROW(sq))
        r -= DOUBLED_PAWN_PENALTY;

    /* if there aren't any friendly pawns on either side of
       this one, it's isolated */
    if ((pawn_rank[LIGHT][f - 1] == 0) &&
        (pawn_rank[LIGHT][f + 1] == 0))
        r -= ISOLATED_PAWN_PENALTY;

    /* if it's not isolated, it might be backwards */
    else if ((pawn_rank[LIGHT][f - 1] < ROW(sq)) &&
             (pawn_rank[LIGHT][f + 1] < ROW(sq)))
        r -= BACKWARDS_PAWN_PENALTY;

    /* add a bonus if the pawn is passed */
    if ((pawn_rank[DARK][f - 1] >= ROW(sq)) &&
        (pawn_rank[DARK][f] >= ROW(sq)) &&
        (pawn_rank[DARK][f + 1] >= ROW(sq)))
        r += (7 - ROW(sq)) * PASSED_PAWN_BONUS;

    return r;
}

int eval_dark_pawn(int sq)
{
	int r;  /* the value to return */
	int f;  /* the pawn's file */

	r = 0;
	f = COL(sq) + 1;

	r += pawn_pcsq[flip[sq]];

	/* if there's a pawn behind this one, it's doubled */
	if (pawn_rank[DARK][f] < ROW(sq))
		r -= DOUBLED_PAWN_PENALTY;

	/* if there aren't any friendly pawns on either side of
	   this one, it's isolated */
	if ((pawn_rank[DARK][f - 1] == 7) &&
			(pawn_rank[DARK][f + 1] == 7))
		r -= ISOLATED_PAWN_PENALTY;

	/* if it's not isolated, it might be backwards */
	else if ((pawn_rank[DARK][f - 1] > ROW(sq)) &&
			(pawn_rank[DARK][f + 1] > ROW(sq)))
		r -= BACKWARDS_PAWN_PENALTY;

	/* add a bonus if the pawn is passed */
	if ((pawn_rank[LIGHT][f - 1] <= ROW(sq)) &&
			(pawn_rank[LIGHT][f] <= ROW(sq)) &&
			(pawn_rank[LIGHT][f + 1] <= ROW(sq)))
		r += ROW(sq) * PASSED_PAWN_BONUS;

	return r;
}

int eval_light_king(int sq)
{
    int r;  /* the value to return */
    int i;

    r = king_pcsq[sq];

    /* if the king is castled, use a special function to evaluate the
       pawns on the appropriate side */
    if (COL(sq) < 3) {
        r += eval_lkp(1);
        r += eval_lkp(2);
        r += eval_lkp(3) / 2;  /* problems with pawns on the c & f files
                                  are not as severe */
    }
    else if (COL(sq) > 4) {
        r += eval_lkp(8);
        r += eval_lkp(7);
        r += eval_lkp(6) / 2;
    }

    /* otherwise, just assess a penalty if there are open files near
       the king */
    else {
        for (i = COL(sq); i <= COL(sq) + 2; ++i)
            if ((pawn_rank[LIGHT][i] == 0) &&
                (pawn_rank[DARK][i] == 7))
                r -= 10;
    }

    /* scale the king safety value according to the opponent's material;
       the premise is that your king safety can only be bad if the
       opponent has enough pieces to attack you */
    r *= piece_mat[DARK];
    r /= 3100;

    return r;
}

/* eval_lkp(f) evaluates the Light King Pawn on file f */

int eval_lkp(int f)
{
    int r = 0;

    if (pawn_rank[LIGHT][f] == 6);  /* pawn hasn't moved */
    else if (pawn_rank[LIGHT][f] == 5)
        r -= 10;  /* pawn moved one square */
    else if (pawn_rank[LIGHT][f] != 0)
        r -= 20;  /* pawn moved more than one square */
    else
        r -= 25;  /* no pawn on this file */

    if (pawn_rank[DARK][f] == 7)
        r -= 15;  /* no enemy pawn */
    else if (pawn_rank[DARK][f] == 5)
        r -= 10;  /* enemy pawn on the 3rd rank */
    else if (pawn_rank[DARK][f] == 4)
        r -= 5;   /* enemy pawn on the 4th rank */

    return r;
}

int eval_dark_king(int sq)
{
    int r;
    int i;

    r = king_pcsq[flip[sq]];
    if (COL(sq) < 3) {
        r += eval_dkp(1);
        r += eval_dkp(2);
        r += eval_dkp(3) / 2;
    }
    else if (COL(sq) > 4) {
        r += eval_dkp(8);
        r += eval_dkp(7);
        r += eval_dkp(6) / 2;
    }
    else {
        for (i = COL(sq); i <= COL(sq) + 2; ++i)
            if ((pawn_rank[LIGHT][i] == 0) &&
                (pawn_rank[DARK][i] == 7))
                r -= 10;
    }
    r *= piece_mat[LIGHT];
    r /= 3100;
    return r;
}

int eval_dkp(int f)
{
    int r = 0;

    if (pawn_rank[DARK][f] == 1);
    else if (pawn_rank[DARK][f] == 2)
        r -= 10;
    else if (pawn_rank[DARK][f] != 7)
        r -= 20;
    else
        r -= 25;

    if (pawn_rank[LIGHT][f] == 0)
        r -= 15;
    else if (pawn_rank[LIGHT][f] == 2)
        r -= 10;
    else if (pawn_rank[LIGHT][f] == 3)
        r -= 5;

    return r;
}

/* move_str returns a string with move m in coordinate notation */
char *move_str(int4 mu)
{
    static char str[6];
    move_bytes m = ((move*)&mu)->b;


    sprintf(str, "%c%d%c%d",
            COL(m.from) + 'a',
            8 - ROW(m.from),
            COL(m.to) + 'a',
            8 - ROW(m.to));

    if (m.bits & 32) 
    {
        char* p = str + strlen(str);
        switch (m.promote) 
        {
        case KNIGHT:
            *p++ = 'n';
            break;
        case BISHOP:
            *p++ = 'b';
            break;
        case ROOK:
            *p++ = 'r';
            break;
        default:
            *p++ = 'q';
            break;
        }
        *p = 0;
    }
    return str;
}


#pragma pack(pop)

