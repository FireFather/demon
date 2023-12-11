//
//demon define.h
//

#pragma once

#define NAME "Demon"
#define VERSION "1.3"

#ifdef _WIN64
#define PLATFORM "x64"
#else
#define PLATFORM "w32"
#endif

#define AUTHOR "Norman Schmidt"

#pragma warning(disable : 4100) // 'side': unreferenced formal parameter
#pragma warning(disable : 4244) // conversion from 'int' to 'short', possible loss of data
#pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable : 4456) // declaration of 'value' hides previous local declaration
#pragma warning(disable : 4459) // declaration of 'move_time' hides global declaration
#pragma warning(disable : 4706) // assignment within conditional expression
#pragma warning(disable : 4996) // 'sscanf': This function or variable may be unsafe

#define ABS(x) (((x) >= 0) ? (x) : -(x))

//fractional ply
#define ONE_PLY 40

//extensions
enum
{
	CHECK_EXT = 40,
	NULL_EXT = 30,
	PAWN_PUSH = 40,
	RECAP_EXT = 30
};

enum
{
	HISTORY_SCANS = 3
};

enum
{
	MAX_DEPTH = 128,
	MAX_ITERATION = 64,
	MAX_MOVES = 256,
	MAX_STRING = 512,
	MAX_GAME = 1024
};

//hash defines
enum
{
	NIX = 0,
	BELOW_TRUE = 1,
	ABOVE_TRUE = 2,
	EQUAL_TRUE = 3,
	NULL_FAILS = 4
};

//search defines
enum
{
	MATE = 30000,
	NEAR_MATE = 29000,
	LIMIT = 31000,
	MAX = 32000
};

enum
{
	WHITE = 0,
	BLACK = 1
};

enum
{
	EMPTY = 0,
	BLACK_ROOK = 1,
	BLACK_BISHOP = 2,
	BLACK_QUEEN = 3,
	BLACK_KNIGHT = 4,
	BLACK_PAWN = 5,
	BLACK_KING = 6
};

enum
{
	WHITE_ROOK = 9,
	WHITE_BISHOP = 10,
	WHITE_QUEEN = 11,
	WHITE_KNIGHT = 12,
	WHITE_PAWN = 13,
	WHITE_KING = 14
};

//square defines
enum
{
	H1 = 0,
	G1 = 1,
	F1 = 2,
	E1 = 3,
	D1 = 4,
	C1 = 5,
	B1 = 6,
	A1 = 7,
	H2 = 8,
	G2 = 9,
	F2 = 10,
	E2 = 11,
	D2 = 12,
	C2 = 13,
	B2 = 14,
	A2 = 15,
	H3 = 16,
	G3 = 17,
	F3 = 18,
	E3 = 19,
	D3 = 20,
	C3 = 21,
	B3 = 22,
	A3 = 23,
	H4 = 24,
	G4 = 25,
	F4 = 26,
	E4 = 27,
	D4 = 28,
	C4 = 29,
	B4 = 30,
	A4 = 31,
	H5 = 32,
	G5 = 33,
	F5 = 34,
	E5 = 35,
	D5 = 36,
	C5 = 37,
	B5 = 38,
	A5 = 39,
	H6 = 40,
	G6 = 41,
	F6 = 42,
	E6 = 43,
	D6 = 44,
	C6 = 45,
	B6 = 46,
	A6 = 47,
	H7 = 48,
	G7 = 49,
	F7 = 50,
	E7 = 51,
	D7 = 52,
	C7 = 53,
	B7 = 54,
	A7 = 55,
	H8 = 56,
	G8 = 57,
	F8 = 58,
	E8 = 59,
	D8 = 60,
	C8 = 61,
	B8 = 62,
	A8 = 63
};

//macros
#define FROM(m) ((m)&63)
#define TO(m) (((m)>>6)&63)
#define PIECE(m) (((m)>>12)&15)
#define CAPTURED(m) (((m)>>16)&15)
#define SPECIAL(m) (((m)>>20)&7)
#define PROMOTE(m) (((m)>>23)&7)

#define RANK(sq) ((sq)>>3)
#define FILE(sq) ((sq)&7)

#define SWITCH_TURN(t) ((t)^1)
#define CHANGE_SIDE(s) (-(s))

#define BLACK_SIDE(s) ((s)>0)
#define WHITE_SIDE(s) ((s)<0)

#define BLACK_TURN(t) (t)
#define WHITE_TURN(t) ((t)^1)

#define WHITE_PIECE(p) ((p)&8)

#define NULL_REDUCTION(d) ((d)> 6 * ONE_PLY ? 4 * ONE_PLY : 3 * ONE_PLY)

