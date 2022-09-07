#ifndef DRAWING_FUNCS_H

#include "tetris.h"

#define DRAWING_FUNCS_H

#define SPRITE_CHAR_W 8
#define SPRITE_CHAR_H 8
#define SPRITE_ROW_0_F 0x60
#define SPRITE_ROW_G_V 0x68
#define SPRITE_ROW_W_Z 0x70
#define SPRITE_CHAR_BLANK_X 0x70
#define SPRITE_CHAR_BLANK_Y 0x70

#define BG_COLOR 3
#define WINDOW_COLOR 0

void load_spritesheet();

void init_tetromino_minis();

void CLB(char c);
void CLS(char c);

void FillRect(char x, char y, char w, char h, char c);

void SpriteRect(char x, char y, char w, char h, char gx, char gy);

void printnum(int num);

void print(char* str);

void draw_field(char* field, char x, char y);

void draw_piece(PiecePos* pos, const char* piece, char offsetX, char offsetY);

void draw_mini(const char tet_index, char x, char y);

void drawBackground(PlayerState* player);

void drawPlayerState(PlayerState* player);

extern char cursorX, cursorY;

extern const unsigned char tetro_colors[TET_COUNT+2];

#endif