#ifndef TETRIS_H
#define TETRIS_H

#define FIELD_W 10
#define FIELD_H 22
#define GRID_SPACING 4
#define SPAWN_ROW 1
#define SPAWN_COL 4

#define PIECEBUF_WIDTH 5
#define PIECEBUF_SIZE 25
#define NUM_KICKTESTS 5
#define LOCK_FRAMES 15

#define TET_I 0
#define TET_J 1
#define TET_L 2
#define TET_O 3
#define TET_S 4
#define TET_T 5
#define TET_Z 6
#define TET_COUNT 7

typedef struct {
    char x, y, rot, t, lock;
} PiecePos;

#define PLAYER_DEAD 1
#define PLAYER_DIDHOLD 2
#define PLAYER_BACK_TO_BACK 4

typedef struct {
    char field_offset_x, field_offset_y;
    char playField[FIELD_W*FIELD_H];
    char currentPiece[PIECEBUF_SIZE];
    PiecePos currentPos;
    PiecePos heldPiece;
    char flags;
    unsigned char fallTimer, fallRate;
    int score;
    char pendingGarbage;
    char combo;
} PlayerState;

extern const unsigned char tetro_index[TET_COUNT];
extern const unsigned char tetrominoes[PIECEBUF_SIZE*7];

void initPlayerState(PlayerState* player);
char updatePlayerState(PlayerState* player, int inputs, int last_inputs);

#endif