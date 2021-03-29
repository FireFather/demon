//
//demon define.h
//

#ifndef DEFINES_H
#define DEFINES_H

#define NAME "Demon"
#define VERSION "1.0"
#define AUTHOR "Kranium"

//fractional ply
#define ONE_PLY 40

//extensions
#define CHECK_EXT 40
#define NULL_EXT 30
#define PAWN_PUSH 40
#define RECAP_EXT 30

#define HISTORY_SCANS 3

#define MAX_DEPTH 128
#define MAX_ITERATION 64
#define MAX_MOVES 256
#define MAX_STRING 512
#define MAX_GAME 1024

//hash defines
#define NIX 0
#define BELOW_TRUE 1
#define ABOVE_TRUE 2
#define EQUAL_TRUE 3
#define NULL_FAILS 4

//search defines
#define MATE 30000
#define NEAR_MATE 29000
#define INFINITY 31000
#define LIMIT 32000

#define WHITE 0
#define BLACK 1

#define EMPTY 0
#define BLACK_ROOK 1
#define BLACK_BISHOP 2
#define BLACK_QUEEN 3
#define BLACK_KNIGHT 4
#define BLACK_PAWN 5
#define BLACK_KING 6

#define WHITE_ROOK 9
#define WHITE_BISHOP 10
#define WHITE_QUEEN 11
#define WHITE_KNIGHT 12
#define WHITE_PAWN 13
#define WHITE_KING 14

//square defines
#define H1 0
#define G1 1
#define F1 2
#define E1 3
#define D1 4
#define C1 5
#define B1 6
#define A1 7
#define H2 8
#define G2 9
#define F2 10
#define E2 11
#define D2 12
#define C2 13
#define B2 14
#define A2 15
#define H3 16
#define G3 17
#define F3 18
#define E3 19
#define D3 20
#define C3 21
#define B3 22
#define A3 23
#define H4 24
#define G4 25
#define F4 26
#define E4 27
#define D4 28
#define C4 29
#define B4 30
#define A4 31
#define H5 32
#define G5 33
#define F5 34
#define E5 35
#define D5 36
#define C5 37
#define B5 38
#define A5 39
#define H6 40
#define G6 41
#define F6 42
#define E6 43
#define D6 44
#define C6 45
#define B6 46
#define A6 47
#define H7 48
#define G7 49
#define F7 50
#define E7 51
#define D7 52
#define C7 53
#define B7 54
#define A7 55
#define H8 56
#define G8 57
#define F8 58
#define E8 59
#define D8 60
#define C8 61
#define B8 62
#define A8 63

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

#define WHITE 0
#define BLACK 1

#define BLACK_SIDE(s) ((s)>0)
#define WHITE_SIDE(s) ((s)<0)

#define BLACK_TURN(t) (t)
#define WHITE_TURN(t) ((t)^1)

#define WHITE_PIECE(p) ((p)&8)

#define NULL_REDUCTION(d) ((d)> 6 * ONE_PLY ? 4 * ONE_PLY : 3 * ONE_PLY)
#endif
