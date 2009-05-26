
/*-
 * Copyright (c) 2009 Voidware Ltd.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of the source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Any redistribution solely in binary form must conspicuously
 *    reproduce the following disclaimer in documentation provided with the
 *    binary redistribution.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'', WITHOUT ANY WARRANTIES, EXPRESS
 * OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  LICENSOR SHALL
 * NOT BE LIABLE FOR ANY LOSS OR DAMAGES RESULTING FROM THE USE OF THIS
 * SOFTWARE, EITHER ALONE OR IN COMBINATION WITH ANY OTHER SOFTWARE.
 * 
 */

/***************************************************************************

* This is VoidCHESS  v1.4
* 
* Vchess has been designed to run in a small code and data footprint. for
* example, embedded applications.  vchess does not have an abundance of static
* data, eg precomputed move and evaluation data. there is only one array of
* move offsets and another of piece values (also the starting board layout).
*
* Vchess draws its static board evaluation from positional mobility along with
* the usual material score. This makes for a small eval routine (less code),
* but has some serious drawbacks. For example, there is no help for pawn
* structures nor prevention of rash attacks. There are small tweaks to the
* positional counters inside the move scan itself. I hope to improve on this
* eventually.
*
* For data space, vchess has the board, the move stack and other small data
* including the killer array. The biggest of these is the move stack. A move is
* 4 bytes and this stack is the maximum number of moves we can have in our
* search tree. change the move stack size to what you have available.
* 
* The board is represented as a 128 byte array. This is the "0x88" board
* layout. The left 64 bytes are the board squares and the right 64 bytes are
* divided into 32 white and 32 black piece position stores. For example the
* white king is in square Board[0x4]. Board[4] contains the piece slot (eg
* 0xb). This is an index into the right side of the board array. We have
* Board[0xb] = 4. So the piece position corresponding to a board square points
* back to the board square. This arrangement is so that we can either traverse
* the board or traverse the pieces depending on the situation. Each of the 32
* position slots (on the right side of the 0x88 board) is actually the 16
* pieces (board square indices) followed by their piece type. You add 32 to
* the piece position slot to get to the piece type (this is because to add 16
* on 0x88 you add 0x20 otherwise you wind up on the left board again!).
*
* This picture makes this arrangement a bit clearer (in hex):
*
* 70 71 72 73 74 75 76 77 | 78 79 7a 7b 7c 7d 7e 7f
* 60 61 62 63 64 65 66 67 | 68 69 6a 6b 6c 6d 6e 6f
* 50 51 52 53 54 55 56 57 | 58 59 5a 5b 5c 5d 5e 5f
* 40 41 42 43 44 45 46 47 | 48 49 4a 4b 4c 4d 4e 4f
* 30 31 32 33 34 35 36 37 | 38 39 3a 3b 3c 3d 3e 3f
* 20 21 22 23 24 25 26 27 | 28 29 2a 2b 2c 2d 2e 2f
* 10 11 12 13 14 15 16 17 | 18 19 1a 1b 1c 1d 1e 1f
*  0  1  2  3  4  5  6  7 |  8  9  a  b  c  d  e  f
* 
* SUMMARY:



* when `index' & 0x88 == 0, we have a valid board square and Board[index] =
*       piece position slot (pps), zero => empty.
* Board[pps] = square index (ie back to the board square), -1 => empty.
* Board[pps + 32] = piece code (eg pawn, knight, rook etc.)
* 16 white pieces (pps)  in 0x8 thru 0xf then 0x18 thru 0x1f
* 16 white piece types  in 0x28 to 0x2f, then 0x38 to 0x3f
* The black pieces start at 0x48 and the same pattern follows.
* 
* The principal variation (PV) found during iterative deepening is not
* stored in static data, only the very top-level PV is held in `MainPV'. Note
* that the PVLine object is an array of MAX_PV move slots (eg 10). This means
* it is around 44 bytes (inc length). The PV tree is held on the stack during
* search and only alpha improvers are copied to the parent. This avoids having
* static data for the PV tree. However it does mean there might be a stack
* space problem. This is mitigated by arranging for only those search depths >
* 0 having a PVLine object (no PV is maintained in quiescence). So, for
* example, if your search depth is 6 levels, you will need 6*44 = 264 bytes of
* stack for the PVLines at least. Plus there are other local variables in
* search, but this is the biggest.
* 
* There are a small number of chess features ignored (currently).
* 
* * three move repetition rule
* * 50 move rule (todo).
*
* Using:
*
* If this is the vchess.c source file, you can just compile it. the main loop
* prints out a rather terrible board and command line UI.  However, the command
* interface supports the WinBoard protocol. I strongly recommend using winboard
* to host vchess.
*
*
**************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "menu.h"
#include "uWatch-vchess.h"
#include "uWatch-LCD.h"
#include "characterset.h"


#define CASE_MIN        0
#define CASE_MOVES      1
#define CASE_NONSLIDERS 2
#define CASE_ALL        0xff


typedef unsigned char uchar;
typedef signed char byte;
typedef unsigned int uint;
typedef unsigned long uint4;

#define VERSION "VoidCHESS 1.4"

// maximum moves in tree search
#define MAX_STACK       200
#define MAX_PV          10
#define MAX_DEPTH       10
#define WIN_SCORE       10000


// canonial piece codes
typedef enum
{
    empty = 0,
    pawn = 1,
    knight = 2,
    king = 3,
    bishop = 5,
    rook = 6,
    queen = 7
} Piece;

typedef struct
{
    uchar from;
    uchar to;
    uchar promote;
    uchar toPos;
} Move;

// represent the principal variation
typedef struct 
{
    int         n;
    Move        m[MAX_PV];
} PVLine;


void initBoard();
static int computerMoves();
int moveGen(int cases, int side);
int moveToBoard(const char* p);
void playMove(Move m);
int search(int alpha, int beta, int depth, int nullDepth, int tryPV, PVLine* ppv);
const char* moveToStr(Move m, int fancy);
int moveNumber;


// board offset of king position
#define POSKING  0x08

// position delta for piece info
#define POSMAT   0x20

#define A1 0
#define B1 1
#define C1 2
#define D1 3
#define E1 4
#define F1 5
#define G1 6
#define H1 7

#define A8 0x70
#define B8 0x71
#define C8 0x72
#define D8 0x73
#define E8 0x74
#define F8 0x75
#define G8 0x76
#define H8 0x77

#define WHITE 0
#define BLACK 1
#define PIECEMASK 0x7

// pieces separated by 32 so we have a bit for black
#define BLACKPOS 0x40


// for pos codes
#define SIDEOF(_x) ((_x)>=(BLACKPOS+POSKING))
#define COMPARE_MOVES(_a, _b) \
    (*(uint4*)&(_a) == *(uint4*)&(_b))

// for piece code
#define SLIDER(_x)  ((_x)&4)

// also piece codes if SLIDER will have these bits too
#define SLIDE_DIAG 1
#define SLIDE_ORTH 2


byte Board[128]; // 0x88 representation
uint Side;
Move MoveStack[MAX_STACK];
Move* moveStackPtr;
Move* moveStackEnd;
uchar InCheck;  
int Overflow;
uint4 Nodes;
uint MoveCount[2];
uint MatScore[2];
uint EP;
PVLine MainPV;
int UsePV;
uint Castle;
Move Killer[MAX_PV];
int Top;

// represent a board in piece codes, will be translated into
// pos codes on the 0x88 board
static const uchar startboard[] = 
{
    rook, knight, bishop, queen, king, bishop, knight, rook,
    pawn, pawn, pawn, pawn, pawn, pawn, pawn, pawn,        
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    pawn|BLACKPOS, pawn|BLACKPOS, pawn|BLACKPOS, pawn|BLACKPOS, pawn|BLACKPOS, pawn|BLACKPOS, pawn|BLACKPOS, pawn|BLACKPOS,
    rook|BLACKPOS, knight|BLACKPOS, bishop|BLACKPOS, queen|BLACKPOS, king|BLACKPOS, bishop|BLACKPOS, knight|BLACKPOS, rook|BLACKPOS
};

// values of piece codes, pnk?brq
const short pieceValue[] = { 0, 100, 300, 0, 0, 300, 500, 900 };

// movements by piece code on the 0x88 board
const byte offset[6][9] = 
{
    { 31, 33, 14, 18, -18, -14, -33, -31 }, // n
    { 16, -16, -1, 1, 15, 17, -17, -15 }, // k
    { 0 }, // ?
    { 15, 17, -17, -15 }, // b
    { 16, -1, 1, -16 }, // r
    { 15, 16, 17, -1, 1, -17, -16, -15 }, // q
};

static int chessLevel;
static int computer;
static int runOnce;

const unsigned char character_king[] = { 0x04, 0x0e, 0x04, 0x11, 0x15, 0x0E, 0x0E, 0x1F };
const unsigned char character_queen[] = { 0x00, 0x15, 0x00, 0x15, 0x15, 0x0E, 0x0E, 0x1F };
const unsigned char character_pawn[] = { 0x00, 0x04, 0x0E, 0x0E, 0x04, 0x4, 0x0E, 0x1F };
const unsigned char character_rook[] = { 0x00, 0x15, 0x1F, 0xE, 0xE, 0x0E, 0x0E, 0x1F };
const unsigned char character_bishop[] = { 0x04, 0x0E, 0x1D, 0x1F, 0x0E, 0x04, 0x0E, 0x1F };
const unsigned char character_knight[] = { 0x03, 0x1A, 0x1F, 0x07, 0x0E, 0x1E, 0x1F, 0x1F };

//const unsigned char character_up[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x0E, 0x1F };

const unsigned char character_blacksquare[] = { 0x15, 0, 0x0A, 0, 0x15, 0, 0x0A, 0 };



int chosen( int computerColour ) {
    computer = 1-computerColour;
    return MODE_EXIT;
}

/* data for the board display */

static char dispBoard[20][17];
static char lastMove[17];
static int contGame;

int cont( int p ) {
    contGame = p;
    return MODE_EXIT;
}


const menuItem contMenuMenu[] = {
    { "Continue", &cont, TRUE },
    { "New Game", &cont, FALSE },
};

const packedMenu2 contMenu = {
    "- VCHESS v1.4 -",
    printMenu,
    0, 0, 2, contMenuMenu
};


const menuItem colourMenuMenu[] = {
    { "White", &chosen, WHITE },
    { "Black", &chosen, BLACK },
};

const packedMenu2 colourMenu = {
    "- VCHESS v1.4 -",
    printMenu,
    0, 0, 2, colourMenuMenu
};

int levelChoose( int p ) {
    chessLevel = p;
    return MODE_EXIT;
}

const charSet charLevel[] = {
    character_y,
};

const menuItem levelMenuMenu[] = {
    { "Eas\3",   levelChoose, 2 },
    { "Medium", levelChoose, 3 },
    { "Hard",   levelChoose, 4 },
};


const packedMenu2 levelMenu = {
    "Difficult\3?",
    printMenu,
    1, charLevel, 3, levelMenuMenu
};


void updateLine( char *destW, char *destB, int line ) {

    int col;
    for ( col = 0; col < 8; col++ ) {

        *destB = ' ';

        char visual = (( line + col ) & 1 ) ? ' ' : 8;
        int pos = Board[ ( line << 4 ) + col ];
        
        if ( pos > 0 ) {
            visual = Board[ pos + POSMAT ];         // type = custom char# too
            if ( pos & BLACKPOS )                    // black piece?  QUICKEST WAY TO DO THIS!
                *destB = visual;
        }

        *destW = visual;
        destW++;
        destB++;

    }
}

/*
void clearArrow( int line ) {
    dispBoard[line][15]=0x20;
    dispBoard[line+10][15]=0x20;
}
*/

/*void fixArrow( int line ) {

    const unsigned char *newArrow = character_arrow_updown;
    if ( line == 8 )
        newArrow = character_arrow_down;
    else if ( !line )
        newArrow = character_arrow_up;

    custom_character( 4, newArrow );
    dispBoard[ line ][15] = 4;
    dispBoard[ line+10 ][15] = 4;

}
*/

int contrast = 4;       // RANGE 0 - 7 INCLUSIVE
int line = 0;

int showBoard() {

    // Odd ordering of character #s is so we can use the chess engine's piece #s without
    // requiring translation.

    custom_character( 0, character_blacksquare );
/*    custom_character( 1, character_pawn );
    custom_character( 2, character_knight );
    custom_character( 3, character_king );
    custom_character( 5, character_bishop );
    custom_character( 6, character_rook );
    custom_character( 7, character_queen );
*/

    unsigned int flickerPattern[] = {
        0xFFFF,     // 1111 1111 1111 1111 0
        0xFEFE,     // 1111 1110 1111 1110 2
        0xEEEE,     // 1110 1110 1110 1110 4
        0xB6B5,     // 1011 0110 1011 0101 6
        0xAAAA,     // 1010 1010 1010 1010 8
        0xA52A,     // 1010 0101 0010 1010 10
        0x8844,     // 1000 1000 0100 0100 12
        0x8080,     // 1000 0000 1000 0000 14
        0x0000,     // 0000 0000 0000 0000 16
    };

    // Rebuild the board display strings
    int row;
    for ( row = 0; row < 8; row ++ )
        updateLine( dispBoard[ row + 11 ] + 4, dispBoard[ row + 1 ] + 4, row );
    


    extern unsigned int mask;
    mask = 0;
    
//    fixArrow( line );
//    ResetSleepTimer();
//    EnableSleepTimer();
    
    unsigned int counter = 0;
    while ( TRUE ) {

        counter >>= 1;
        if ( !counter )
            counter = 0x8000;

        int offset = (flickerPattern[ contrast ] & counter) ? 10 : 0;
        UpdateLCDline2( dispBoard[ line + offset ]);
        UpdateLCDline1( dispBoard[ line + offset + 1 ]);

        int key = KeyScan2();
        if ( !key )
            mask = 0xFFFF;
        key &= mask;

        if ( key == KeyClear || ENTER(key))
            break;
        if ( key == KeyMode )
            return MODE_KEYMODE;

        int temp = ReturnNumber(key);
        if ( temp >= 1 && temp <= 9 ) {
            contrast = temp - 1;
            mask = 0;
        }    

        if ( key == KeyRCL )
            backlightControl();

        if ( PREVIOUS( key ) && line < 8 ) {
            //clearArrow( line );
            line++;
            //fixArrow( line );
            mask = 0;
        }

        else if ( NEXT( key ) && line > 0 ) {
            //clearArrow( line );
            line--;
            //fixArrow( line );
            mask = 0;
        }
    }

//    DisableSleepTimer();
    return MODE_EXIT;
}


Move *printer;
char vis[100];

char *printChessMove(int *n, int max ) {

    Clock4MHz();

    Move *p = printer + *n;
    int pos = Board[ p->from ];
#define OFFSET_OF_DESTINATION_ROW 10
    sprintf( vis, "%3d.%c %s", (moveNumber+1)/2, Board[ pos + POSMAT ], moveToStr( *p, 1 ));

    if ( max ) {
        sprintf( out, "Your Move? %2d/%2d", (*n)+1,max );
        Clock250KHz();
        UpdateLCDline1(out );
    }

    Clock250KHz();
    return vis;
}



int chessGame( int p )
{
#if 1
    int moveok;
    Move* mv;
    Move* first;
//    int to, from, promote;

    contGame = 0;

    if ( runOnce ) {
        // have previously run... so continue, or new game?    
        if ( genericMenu2( &contMenu ) == MODE_KEYMODE )
            return MODE_KEYMODE;
    }


    strcpy( dispBoard[0], "    ABCDEFGH" );
    strcpy( dispBoard[10], dispBoard[0] );
    strcpy( dispBoard[9], dispBoard[0] );
    strcpy( dispBoard[19], dispBoard[0] );

    int line;
    for ( line = 1; line <= 8; line++ ) {
        sprintf( dispBoard[ line ], "  %d|        |   ", line );
        strcpy( dispBoard[ line + 10 ], dispBoard[ line ] );
    }    



    if ( !contGame ) {
        
        initBoard();

        // choose colour    
        if ( genericMenu2( &colourMenu ) == MODE_KEYMODE )
            return MODE_KEYMODE;

        // get level
        if ( genericMenu2( &levelMenu ) == MODE_KEYMODE )
            return MODE_KEYMODE;

//        UpdateLCDline1("");
        UpdateLCDline2("");         //so we don't see glitch in "easy"

    }

    custom_character( 1, character_pawn );
    custom_character( 2, character_knight );
    custom_character( 3, character_king );
    custom_character( 5, character_bishop );
    custom_character( 6, character_rook );
    custom_character( 7, character_queen );


    runOnce = TRUE;



    for ( ;; ) {
        if ( computer == Side ) {
            if ( computerMoves() ) {
                //GetDebouncedKey();
                runOnce = FALSE; // new game next time
                break; // game over
            }
        }

        moveStackPtr = MoveStack;
        first = moveStackPtr;

        // generate legal moves
        moveGen( CASE_ALL, Side );

        do {

            moveok = 0;

            int sel = 0;
            int key;
            do {

                if ( showBoard() == MODE_KEYMODE )
                    return MODE_KEYMODE;

                int mc = 0;
                for ( mv = first; mv != moveStackPtr; ++mv )
                   mc--;
    
                printer = first;
    
    
                key = genericMenu( "", &printChessMove, &increment, &decrement, mc, &sel );

            } while ( key == MODE_KEYMODE );


            moveNumber++;

            mv = first + sel;
            moveok = 1;

        } while ( !moveok );

        if ( moveok )
            playMove( *mv );
    }
#endif
    return MODE_EXIT;
}


static int computerMoves()
{
    int i;
    int v = 0;
    int chk = InCheck;
    int dmax = chessLevel;

    Nodes = 0;

    /* bump the CPU whilst we think... */
    StopSleepTimer();

    custom_character(4,character_g);
    UpdateLCDline1( "Thinkin\4..." );

    // boost one level when in check
    if ( chk ) ++dmax;

    Clock4MHz();
    memset( &MainPV, 0, sizeof( MainPV ) );
    for ( i = 1; i <= dmax; ++i ) {
        UsePV = 1;
        InCheck = chk;
        moveStackPtr = MoveStack;
        Top = i;
        v = search( -WIN_SCORE, WIN_SCORE, i, 0, 1, &MainPV );
    }

    /* and back again to slow.. */
    Clock250KHz();
//    ResetSleepTimer();
    StartSleepTimer();

    if ( MainPV.n ) {

        Move* m = MainPV.m;
        printer = m;
        int sel = 0;
        strcpy(lastMove, printChessMove( &sel, 0 ));
        playMove(*m);

        // focus board display on the last move's line
        //clearArrow( line );
        line = lastMove[OFFSET_OF_DESTINATION_ROW]-'0'-1; 

        if (InCheck) strcat( lastMove, "+" );

        moveNumber++;

        UpdateLCDline1("I move:");
        UpdateLCDline2(lastMove);
        GetDebouncedKey();
    }

    if ( fabs(v) >= WIN_SCORE ) {

//        sprintf( out, "c=%d win=%d", computer, v );
//        UpdateLCDline1(out);
//        GetDebouncedKey();


        if (( computer && v >= WIN_SCORE ) || ( !computer && v <= -WIN_SCORE ))
            UpdateLCDline1( "I Win!" );
        else
            UpdateLCDline1( "You Win!" );

        GetDebouncedKey();
        showBoard();
        return 1;
    }
    return 0;
}

void initPieces(const uchar* board)
{
    int i, j;
    int dbw = POSKING + 1; // first nonking slot
    int dbb = dbw + BLACKPOS;
    const uchar* bp = board;
    int p;
    
    for (i = 0; i < 8; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            int t = j+(i<<4);
            int pt = *bp++;
            if (pt)
            {
                int blk = pt & BLACKPOS;
                pt &= PIECEMASK;
            
                if (pt == king)
                    p = POSKING + blk;
                else
                {
                    if (blk)
                    {
                        p = dbb;
                        dbb = (dbb + 1) | 0x08;
                    }
                    else
                    {
                        p = dbw;
                        dbw = (dbw + 1) | 0x08;
                    }
                }

                Board[t] = p;
                Board[p] = t;
                Board[p + POSMAT] = pt;  // save the piece type too
            }
            else
                Board[t] = 0;
        }
    }
}

void initBoard()
{
    // the first 64 are the board squares then 32 white positions
    // then 32 black positions
    memset(Board, -1, 128);
    memset(Killer, 0, sizeof(Killer));
    initPieces(startboard);
    
    moveStackPtr = MoveStack;
    moveStackEnd = MoveStack + MAX_STACK;
    Side = WHITE;
    InCheck = 0;
    EP = 0;
    Castle = 0x0f; // both sides can castle
    lastMove[0] = 0;

    moveNumber = 1;
}

int attackTest(int cases, int side, int pos)
{
    const byte* offp = offset[king-2];
    int i;
    for (i = 0; i < 8; ++i)
    {
        int p = pos;
        int d = 0;
        int p2;
        int pt;
        for (;;)
        {
            p += offp[i];
            if (p & 0x88) break;
            p2 = Board[p];
            if (p2)
            {
                if (SIDEOF(p2) == side) // opponent?
                {
                    pt = Board[p2 + POSMAT];
                    if (SLIDER(pt))
                    {
                        if (i < 4)
                        {
                            if (pt & SLIDE_ORTH) return 1;
                        }
                        else
                        {
                            if (pt & SLIDE_DIAG) return 1;
                        }
                    }
                    else if (!d && (cases & CASE_NONSLIDERS))
                    {
                        if (pt == king) return 1;
                        if (pt == pawn)
                        {
                            int d = p - pos;
                            if (side == WHITE) d = -d;
                            if (d == 0x0f || d == 0x11) return 1;
                        }
                    }
                }
                break;
            }
            ++d;
        }
    }

    if (cases & CASE_NONSLIDERS)
    {
        // knights?
        const byte* offp = offset[knight-2];
        while (*offp)
        {
            int p = pos + *offp++;
            if (!(p & 0x88))
            {
                int p2 = Board[p];
                if (p2 && SIDEOF(p2) == side && Board[p2 + POSMAT] == knight)
                    return 1;
            }
        }
    }
    return 0;
}

// move to blank square
#define MOVE(_f, _t, _fp, _tp)     \
    Board[_fp] = _t;                \
    Board[_t] = _fp;                \
    Board[_f] = _tp;


int isLegalMove(Move mv)
{
    /* its legal if we are not in check */
    int legal;
    int from = mv.from;
    int to = mv.to;
    int fromPos = Board[from];
    int toPos = Board[to];
    int oside = !SIDEOF(fromPos); // other side
    int kingpos = Board[POSKING + (oside ? 0 : BLACKPOS)];
    int kingmove = (from == kingpos);

    if (kingmove)
    {
        // if this is a castle, consider the king moving twice to
        // detect move over check
        int d = to - from;
        if (d == 2 || d == -2)
        {
            // castle
            // is this square attacked?
            if (attackTest(CASE_NONSLIDERS, oside, from+(d>>1)))
            {
                // was illegal
                return 0;
            }
        }
        kingpos = to;
    }

    // make the move    
    MOVE(from, to, fromPos, 0);
    if (toPos) Board[toPos] = -1;

    // if we are not moving the king, we can only in in check from sliders.
    // otherwise check nonsliders too
    // if we're already in check, examine all captures
    legal = !attackTest((kingmove || InCheck) ? CASE_NONSLIDERS : CASE_MIN,
                        oside,
                        kingpos);

    // undo the move
    MOVE(to, from, fromPos, toPos);
    if (toPos) Board[toPos] = to;
    return legal;
}

void pushMove(Move mv)
{
    mv.toPos = Board[mv.to];
    if (moveStackPtr < moveStackEnd)
        *moveStackPtr++ = mv;
    else
        Overflow = 1;
}

int addPawnMove(Move mv, int all)
{
    int mc = isLegalMove(mv);
    if (mc)
    {
        int r = mv.to &0xF0;
        if (r == 0x70 || !r ) // rank 1 o 8
        {
            // consider promotion
            mv.promote = queen;
            pushMove(mv);

            mv.promote = rook;
            pushMove(mv);

            mv.promote = bishop;
            pushMove(mv);
            
            mv.promote = knight;
            pushMove(mv);
        }
        else
            if (all) pushMove(mv);
    }
    return mc;
}

int moveGen(int cases, int side)
{
    /* always scan sliders and captures, `cases' include also
     * nonsliders and ordinary moves.
     */

    // return number of legal moves
    int all = cases & CASE_MOVES;
    int mc = 0;  // legal move count
    int cc = 0;  // captures & defenders
    int dp = POSKING;
    int i;
    Move mv;

    Overflow = 0;
    if (side != WHITE) dp += BLACKPOS;
    
    mv.promote = 0;
    for (i = 0; i < 16; ++i)
    {
        int pos = Board[dp];
        if (pos >= 0)
        {
            int pt = Board[dp + POSMAT];
            mv.from = pos;
            if (pt == pawn)
            {
                int d = 0x10;
                int d2 = 0x20;
                int r2 = 0x10;
                int p2;

                if (side != WHITE)
                {
                    d = -d;
                    d2 = -d2;
                    r2 = 0x60;
                }

                if (!Board[pos+d])
                {
                    if ((pos&0xF0) == r2 && !Board[pos+d2])
                    {
                        // double forward
                        mv.to = pos + d2;
                        if (isLegalMove(mv))
                        {
                            ++mc;
                            if (all) pushMove(mv);
                        }
                    }
                 
                    // pawn advance
                    mv.to = pos + d;
                    mc += addPawnMove(mv, all);
                }
            
                // captures
                p2 = pos + d + 1;
                if (!(p2 & 0x88))
                {
                    int pd = Board[p2];
                    if (pd)
                    {
                        ++cc;
                        if (SIDEOF(pd) != side)
                        {
                            mv.to = p2;
                            mc += addPawnMove(mv, all);
                        }
                    }
                }
                p2 = pos + d - 1;
                if (!(p2 & 0x88))
                {
                    int pd = Board[p2];
                    if (pd)
                    {
                        ++cc;
                        if (SIDEOF(pd) != side)
                        {
                            mv.to = p2;
                            mc += addPawnMove(mv, all);
                        }
                    }
                }
                
                // EP
                if (EP)
                {
                    p2 = Board[EP];
                    if ((p2+1 == pos || p2-1 == pos) && SIDEOF(EP) != side)
                    {
                        p2 += d2 - d; // to square
                        if (!Board[p2])
                        {
                            mv.to = p2;
                            if (isLegalMove(mv))
                            {
                                ++mc;
                                pushMove(mv);
                            }
                        }
                    }
                }
            }
            else
            {
                const byte* off = offset[pt-2];
                int d = *off;
                do
                {
                    int p0 = pos;
                    int bp;
                    do
                    {
                        p0 += d;
                        if (p0 & 0x88) break;
                        bp = Board[p0];
                        mv.to = p0;
                        if (bp)
                        {
                            ++cc;
                            if (SIDEOF(bp) != side)
                            {
                                if (isLegalMove(mv))
                                {
                                    ++mc;
                                    pushMove(mv);
                                }
                            }
                            break;
                        }
                        else
                        {
                            if (isLegalMove(mv))
                            {
                                ++mc;
                                if (all) pushMove(mv);
                            }
                            else if (!InCheck) break; // same legality.
                        }
                    } while (SLIDER(pt));
                } while ((d = *++off) != 0);
            }
        }
        dp = (dp + 1) | 0x08;
    }

    // consider castle if not in check
    // NB: dont include castle moves in move count to encourage castling
    if (!InCheck && all)
    {
        if (side == WHITE)
        {
            // queenside castle
            if ((Castle&1) && Board[B1]+Board[C1]+Board[D1] == 0)
            {
                int p = Board[A1];
                if (p && Board[p+POSMAT] == rook)
                {
                    mv.from = E1; mv.to = C1;
                    if (isLegalMove(mv)) pushMove(mv);
                }
            }
                
            // kingside castle
            if ((Castle&2) && Board[F1]+Board[G1] == 0)
            {
                int p = Board[H1];
                if (p && Board[p+POSMAT]==rook)
                {
                    mv.from = E1; mv.to = G1;
                    if (isLegalMove(mv)) pushMove(mv);
                }
            }
        }
        else
        {
            // queenside castle
            if ((Castle&4) && Board[B8]+Board[C8]+Board[D8] == 0)
            {
                int p = Board[A8];
                if (p && Board[p+POSMAT]==rook)
                {
                    mv.from = E8; mv.to = C8;
                    if (isLegalMove(mv)) pushMove(mv);
                }
            }
                
            // kingside castle
            if ((Castle&8) && Board[F8]+Board[G8]==0)
            {
                int p = Board[H8];
                if (p && Board[p+POSMAT] == rook)
                {
                    mv.from = E8; mv.to = G8;
                    if (isLegalMove(mv)) pushMove(mv);
                }
            }
        }
    }
    return mc + cc;
}

int opponentCheck(int side)
{
    // is other side in check
    return attackTest(CASE_ALL, side,
                      Board[POSKING + (side == WHITE ? BLACKPOS : 0)]);
}

int makeMove(Move m, int* ep)
{
    int from = m.from;
    int to = m.to;
    int fromPos = Board[from];
    int d;
    int oldEP;
    int pt;
    int toPos = Board[to];

    // remember old EP square and reset
    oldEP = EP;
    *ep = oldEP;
    EP = 0; 

    // deal with promotion
    if (m.promote)
    {
        // change the from piece into the promoted piece
        Board[fromPos + POSMAT] = m.promote;
    }

    pt = Board[fromPos + POSMAT];
    if (pt == king) // kingmove
    {
        d = to - from;
        if (d == 2)
        {
            // kingside castle, move rook
            int fp = Board[from+3];
            MOVE(from+3, from+1, fp, 0);
            
        }
        else if (d == -2)
        {
            // queenside castle, move rook
            int fp = Board[from-4];
            MOVE(from-4,from-1, fp, 0);
        }
        
        // clear castle bits
        Castle &= ~(0x3<<(SIDEOF(fromPos)<<1));
    }
    else if (pt == pawn)
    {
        d = to - from;
        if (d < 0) d = -d;
        if (d == 0x20)
        {
            // initial pawn move, set EP square
            EP = fromPos;
        }
        else if ((d&1) && !m.toPos) // EP capture?
        {
            // diagonal with no capture = EP
            // EP square must still contain the piece
            Board[Board[oldEP]] = 0;
            Board[oldEP] = -1; // pos
        }
    }
    else if (pt == rook)
    {
        if (from == 0x70)
            Castle &= ~4;
        else if (from == 0x77)
            Castle &= ~8;
        else if ( !from )
            Castle &= ~1;
        else if (from == 0x7)
            Castle &= ~2;
    }

    MOVE(from, to, fromPos, 0);
    if (toPos) Board[toPos] = -1;

    InCheck = opponentCheck(Side);

    // switch sides
    Side = !Side;

    return InCheck;
}

void unmakeMove(Move m, int ep)
{
    int toPos = m.toPos;
    int from = m.from;
    int to = m.to;
    int fromPos = Board[to];
    int pt;

    // switch sides back
    Side = !Side;

    // restore EP square
    EP = ep;

    if (m.promote)
    {
        // demote piece
        Board[fromPos + POSMAT] = pawn;
    }

    pt = Board[fromPos + POSMAT];
    if (pt == king) // kingmove
    {
        // undo castle
        int d = to - from;
        if (d == 2)
        {
            // undo kingside rook
            int fp = Board[from+1];
            MOVE(from+1, from+3, fp, 0);
        }
        else if (d == -2)
        {
            // undo queenside rook
            int fp = Board[from-1];
            MOVE(from-1, from-4, fp, 0);
        }
    }
    else if (pt == pawn && !m.toPos)  // non capture
    {
        int d = to - from;        
        if (d&1)
        {
            // diagonal non-capture move => EP
            int p2 = to;
            if (d < 0) p2 += 0x10;
            else p2 -= 0x10;
            Board[p2] = EP;
            Board[EP] = p2;
        }
    }

    MOVE(to, from, fromPos, toPos);
    if (toPos) Board[toPos] = to;

    InCheck = 0;
}

void playMove(Move m)
{
    int ep;
    int i;
    for (i = 0; i < MAX_PV-1; ++i) Killer[i] = Killer[i+1];
    makeMove(m, &ep);
}

#define MS(_p, _m) { *(_p)++ = ((_m)&7)+'A';  *(_p)++ = ((_m)>>4)+'1'; }

const char* moveToStr(Move m, int fancy)
{
    static char buf[16];
    char* p = buf;
    MS(p, m.from);
    if (fancy) *p++ = m.toPos ? 'x'  : '-';
    MS(p, m.to);
    *p = 0;
    return buf;
}

int scoreStatic()
{
    int v;
    int i;
    int dp = POSKING+1;
    int p;

    v = 0;
    for (i = 0; i < 15; ++i)
    {
        p = Board[dp];
        if (p >= 0) v += pieceValue[Board[dp + POSMAT]];
        dp = (dp + 1) | 0x08;
    }

    // update the latest material score
    MatScore[WHITE]=v;

    dp = POSKING + BLACKPOS + 1;
    v = 0;
    for (i = 0; i < 15; ++i)
    {
        p = Board[dp];
        if (p >= 0) v += pieceValue[Board[dp + POSMAT]];
        dp = (dp + 1) | 0x08;
    }
    MatScore[BLACK] = v;

    v = MatScore[WHITE] - v;
    if (Side == BLACK) v = -v;
    
    v += MoveCount[Side];

    if (!(Castle & 0x3<<(Side<<1)))
        v += 10; // bonus for castling.

    return v;
}

int moveValue(Move* m)
{
    int v = 0;
    if (m->toPos) // capture
    {
        // MVV/LVA
        // use value of pieces ranges from P*8+(8-K) = 10 to
        // Q*8+(8-P) = 47
        v = Board[m->toPos+POSMAT]*8 + (8 - Board[Board[m->from] + POSMAT]);
    }
    return v;
}

Move* bestMoveVal(Move* first, Move* last)
{
    Move* best = first;
    Move* mv;

    int bestVal = moveValue(best);
    mv = first;
    while (++mv != last)
    {
        int v = moveValue(mv);
        if (v > bestVal)
        {
            bestVal = v;
            best = mv;
        }
    }
    return best;
}

Move* bestMove(Move* first, Move* last, int ply)
{
    Move* best = 0;
    Move* mv;

    if (UsePV)
    {
        for (mv = first; mv != last; ++mv)
        {
            if (COMPARE_MOVES(*mv, MainPV.m[ply]))
            {
                best = mv;
                break;
            }
        }
        if (!best) UsePV = 0; // dont use the pv anymore.
    }

    if (!best)
        for (mv = first; mv != last; ++mv)
        {
            if (COMPARE_MOVES(*mv, Killer[ply])) 
            {
                best = mv;
                break;
            }
        }            
    return best;
}

int quiesce(int alpha, int beta, int depth)
{
    Move* first = moveStackPtr;
    Move* mv;
    int v;
    int ep;
    int castle;
    int done;
    int bv = -WIN_SCORE;

    ++Nodes;

    // generate moves
    // if we're in check, consider legal moves incase there is no escape.
    MoveCount[Side] = moveGen(InCheck ? CASE_ALL : CASE_MIN, Side);

    // give up if we're going too deep whatever
    done = (first == moveStackPtr) || (depth < -MAX_DEPTH);

    // only consider the stand-pat if we're not in check
    if (!InCheck)
    {
        bv = scoreStatic();
    
        if (bv > alpha)
        {
            alpha = bv;
            if (bv >= beta)
                done = 1;
        }
    }
    
    if (!done)
    {
        for (mv = first; mv != moveStackPtr; ++mv)
        {
            // try to search the moves in a better order
            Move* m2 = bestMoveVal(mv, moveStackPtr);
            if (m2 != mv)
            {
                Move t = *mv;
                *mv = *m2;
                *m2 = t;
            }

            castle = Castle;
            makeMove(*mv, &ep);

            v = -quiesce(-beta, -alpha, depth-1);

            Castle = castle;
            unmakeMove(*mv, ep);

            if (v > bv)
            {
                bv = v;
                if (v > alpha)
                {
                    if (v >= beta) break;
                    alpha = v;
                }
            }
        }
    }

    // if we're still in check, we've lost
    if (InCheck)
        bv = -WIN_SCORE + (Top - depth) - 1;

    moveStackPtr = first;
    return bv;
}
    
int search(int alpha, int beta, int depth, int nullDepth, int tryPV,
           PVLine* ppv)
{
    Move* first;
    Move* mv;
    int v;
    PVLine pv;
    int ep;
    int castle;
    int bv;
    int ply;
    int i;
    int moves;
    int chk;

    ppv->n = 0;

    if (depth <= 0) return quiesce(alpha, beta, depth);

    first = moveStackPtr;
    bv = -WIN_SCORE;
    ply = Top - depth;

    ++Nodes;

    // try null moves
    if (ply > 0 && depth > 2 && !InCheck && !(nullDepth&1))
    {
        // check we have sensible material for null move
        if (MatScore[Side] >= 400)
        {
            Side = !Side;
            v = -search(-beta, 1-beta, depth-1-2, nullDepth+1, 0, &pv);
            Side = !Side;
            if (v >= beta)
                return v;
        }
    }
    
    // generate moves
    MoveCount[Side] = moveGen(CASE_ALL, Side);
    moves = moveStackPtr - first;

    if (moves)
    {
        int newPV = 0;
        for (mv = first; mv != moveStackPtr; ++mv)
        {
            // try to search the moves in a better order
            Move* m2 = 0;
            if (!nullDepth) m2 = bestMove(mv, moveStackPtr, ply);
            if (!m2) m2 = bestMoveVal(mv, moveStackPtr);
            if (m2 != mv)
            {
                Move t = *mv;
                *mv = *m2;
                *m2 = t;
            }
            
            castle = Castle;
            chk = makeMove(*mv, &ep);

            // if we are inside a null make even
            if (nullDepth) nullDepth = (nullDepth + 2) & ~1;

            if (depth > 2 && tryPV && newPV)
            {
                v = -search(-alpha-1, -alpha, depth-1, nullDepth, 0, &pv);
                if (v > alpha && v < beta)
                {
                    InCheck = chk;
                    v = -search(-beta, -alpha, depth-1, nullDepth, 1, &pv);
                }
            }
            else
                v = -search(-beta, -alpha, depth-1, nullDepth, tryPV, &pv);
            
            Castle = castle;
            unmakeMove(*mv, ep);

            if (v > bv)
            {
                bv = v;
                if (v > alpha)
                {
                    alpha = v;
                    newPV = 1;

                    // collect the PV
                    ppv->m[0] = *mv;
                    i = pv.n;
                    if (i == MAX_PV) --i;
                    memcpy(ppv->m + 1, pv.m, i * sizeof(Move));
                    ppv->n = i + 1;

                    if (v >= beta)
                    {
                        if (!mv->toPos && ply < MAX_PV && !nullDepth)
                            Killer[ply] = *mv; // update killer move
                        break;
                    }
                }
            }
        }
    }
    else
    {
        // no moves!
        bv = 0; // stalemate
        if (InCheck)
        {
            // mate seen, add ply to prevent mate declaration except
            // top-level
            bv= -WIN_SCORE + ply - 1;
        }
    }

    moveStackPtr = first;
    return bv;
}

int moveToBoard(const char* p)
{
    // NB: changed to use numbers !!
    int b = -1;
    if (*p >= '1' && *p <= '8' && p[1] >= '1' && p[1] <= '8')
        b = ((p[1]-'1')<<4) + p[0]-'1';
    return b;
}


