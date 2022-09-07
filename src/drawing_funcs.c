/*
 * GameTank-specific implementation of drawing functions
 */
#include <zlib.h>
#include "drawing_funcs.h"
#include "gametank.h"

char cursorX, cursorY;

extern const unsigned char* GameSprites;

const unsigned char tetro_colors[TET_COUNT+2] = { 0 , 244, 219, 92, 61, 29, 155, 124, 5};

extern void wait();
extern void nop5();
extern void nop10();

void load_spritesheet() {
    flagsMirror = DMA_NMI | DMA_IRQ;
    *dma_flags = flagsMirror;
    inflatemem(vram, &GameSprites);
}

void init_tetromino_minis() {
    char i, x, y, r, c;
    char *vram_ptr, *tet_ptr;
    flagsMirror = DMA_NMI | DMA_IRQ;
    *dma_flags = flagsMirror;
    vram_ptr = vram;
    tet_ptr = tetrominoes;
    for(i = 0; i < TET_COUNT; i++) {
        for(r = 0; r < PIECEBUF_WIDTH; r++) {
            for(c = 0; c < PIECEBUF_WIDTH; c++) {
                *vram_ptr = tetro_colors[*tet_ptr];
                vram_ptr++;
                tet_ptr++;
            }
            vram_ptr += SCREEN_WIDTH - PIECEBUF_WIDTH;
        }
        vram_ptr += SCREEN_WIDTH * 3;
    }
}

void CLB(char c) {
    SET_COLORFILL
    FillRect(0, 0, SCREEN_WIDTH-1, 7, c);
    FillRect(0, 7, 1, SCREEN_HEIGHT-7, c);
    FillRect(1, SCREEN_HEIGHT-8, SCREEN_WIDTH-1, 8, c);
    FillRect(SCREEN_WIDTH-1, 0, 1, SCREEN_HEIGHT-8, c);
}

void CLS(char c) {
    SET_COLORFILL
    vram[VX] = 1;
    vram[VY] = 7;
    vram[GX] = 0;
    vram[GY] = 0;
    vram[WIDTH] = SCREEN_WIDTH-2;
    vram[HEIGHT] = SCREEN_HEIGHT-7-8;
    vram[COLOR] = ~c;
    vram[START] = 1;
    wait();
}

void FillRect(char x, char y, char w, char h, char c) {
    vram[VX] = x;
    vram[VY] = y;
    vram[GX] = 0;
    vram[GY] = 0;
    vram[WIDTH] = w;
    vram[HEIGHT] = h;
    vram[COLOR] = ~c;
    vram[START] = 1;
    wait();
}

void SpriteRect(char x, char y, char w, char h, char gx, char gy) {
    vram[VX] = x;
    vram[VY] = y;
    vram[GX] = gx;
    vram[GY] = gy;
    vram[WIDTH] = w;
    vram[HEIGHT] = h;
    vram[START] = 1;
}

void printnum(int num) {
    vram[VX] = cursorX;
    vram[VY] = cursorY;
    vram[GY] = SPRITE_ROW_0_F;
    vram[WIDTH] = SPRITE_CHAR_W;
    vram[HEIGHT] = SPRITE_CHAR_H;
    if(num == 0) {
        vram[GX] = 0;
        vram[START] = 1;
        wait();
    } else {
        while(num != 0) {
            vram[GX] = (num % 10) << 3;
            vram[START] = 1;
            wait();
            cursorX -= 8;
            num = num / 10;
            vram[VX] = cursorX;
        }
    }
}

void print(char* str) {
    vram[WIDTH] = SPRITE_CHAR_W;
    vram[HEIGHT] = SPRITE_CHAR_H;
    while(*str != 0) {
        if(*str >= '0' && *str <= '9') {
            vram[GX] = (*str - '0') << 3;
            vram[GY] = SPRITE_ROW_0_F;
        } else if(*str >= 'a' && *str <= 'f') {
            vram[GX] = ((*str - 'a') << 3) + 0x50;
            vram[GY] = SPRITE_ROW_0_F;
        } else if(*str >= 'g' && *str <= 'v') {
            vram[GX] = (*str - 'g') << 3;
            vram[GY] = SPRITE_ROW_G_V;
        } else if(*str >= 'w' && *str <= 'z') {
            vram[GX] = (*str - 'w') << 3;
            vram[GY] = SPRITE_ROW_W_Z;
        } else {
            vram[GX] = SPRITE_CHAR_BLANK_X;
            vram[GY] = SPRITE_CHAR_BLANK_Y;
        }
        if(*str == '\n') {
            cursorX = 0;
            cursorY += 8;
        } else {
            vram[VX] = cursorX;
            vram[VY] = cursorY;
            vram[START] = 1;
            wait();
            cursorX += 8;
        }
        str++;
        if(cursorX >= 128) {
            cursorX = 0;
            cursorY += 8;
        }
        if(cursorY >= 128) {
            cursorX = 0;
            cursorY = 0;
        }
    }
}

void draw_field(char* field, char x, char y) {
    char r, c, vx, vy, f;
    vram[GY] = 64;
    vram[WIDTH] = GRID_SPACING;
    vram[HEIGHT] = GRID_SPACING;
    vy = y;
    for(r = 0; r < FIELD_H; r++) {
        vx = x;
        for(c = 0; c < FIELD_W; c++) {
            f = *field;
            if(f) {
                vram[GX] = f*4;
                vram[VX] = vx;
                vram[VY] = vy;
                vram[START] = 1;
                wait();
            }
            vx+=GRID_SPACING;
            field++;
        }
        vy+=GRID_SPACING;
    }
}

void draw_piece(PiecePos* pos, const char* piece, char offsetX, char offsetY) {
    char r, c, i = 0;
    vram[GY] = 64;
    vram[WIDTH] = GRID_SPACING;
    vram[HEIGHT] = GRID_SPACING;
    for(r = 0; r < PIECEBUF_WIDTH*GRID_SPACING; r+=GRID_SPACING) {
        for(c = 0; c < PIECEBUF_WIDTH*GRID_SPACING; c+=GRID_SPACING) {
            if(!!piece[i]) {
                vram[GX] = piece[i]*4;
                vram[VX] = GRID_SPACING*pos->x + c + offsetX - 2*GRID_SPACING;
                vram[VY] = GRID_SPACING*pos->y + r + offsetY - 2*GRID_SPACING;
                vram[START] = 1;
                wait();
            }
            i++;
        }
    }
}

void draw_mini(const char tet_index, char x, char y) {
    vram[GX] = 0;
    vram[GY] = tet_index * 8;
    vram[WIDTH] = PIECEBUF_WIDTH;
    vram[HEIGHT] = PIECEBUF_WIDTH;
    vram[VX] = x;
    vram[VY] = y;
    vram[START] = 1;
    wait();
}

void drawBackground(PlayerState* player) {
    UNSET_COLORFILL
    SpriteRect(player->field_offset_x-2, player->field_offset_y-2, GRID_SPACING * FIELD_W+4, GRID_SPACING * FIELD_H + 4, 64, 0);
}

void drawPlayerState(PlayerState* player) {
    char i, j;
    flagsMirror &= ~DMA_TRANS;
    *dma_flags = flagsMirror;

    UNSET_COLORFILL
    draw_field(player->playField, player->field_offset_x, player->field_offset_y);
    draw_piece(&(player->currentPos), player->currentPiece, player->field_offset_x, player->field_offset_y);

    SET_COLORFILL
    FillRect(player->field_offset_x +20,
        player->field_offset_y - 15,
        GRID_SPACING * PIECEBUF_WIDTH, 12, 2);

    if(player->heldPiece.t != TET_COUNT) {
        i = 0;
        j = 0;
        if(player->heldPiece.t == TET_I) {
            i = 2;
            j = 2;
        }
        else if(player->heldPiece.t == TET_O) {
            i = 2;
        }
        UNSET_COLORFILL
        draw_piece(
            &(player->heldPiece),
            &(tetrominoes[tetro_index[player->heldPiece.t]]),
            player->field_offset_x - i + 28, player->field_offset_y - j - 9);
    }

    SET_COLORFILL
    FillRect(player->field_offset_x, player->field_offset_y, GRID_SPACING * FIELD_W, 6, BG_COLOR);
    UNSET_COLORFILL
    SpriteRect(player->field_offset_x-2, player->field_offset_y-2, GRID_SPACING * FIELD_W+4, 8, 64, 0);
    wait();

    cursorX = player->field_offset_x + SPRITE_CHAR_W + 4;
    cursorY = player->field_offset_y - SPRITE_CHAR_H - 4;
    printnum(player->score);

    for(i = 0; i < PREVIEW_COUNT; i++) {
        draw_mini(player->bag[(player->bag_index + i) % (TET_COUNT*2)], player->field_offset_x + (i * 8) + player->bag_anim, player->field_offset_y);
    }
    if(player->bag_anim > 0) {
        player->bag_anim--;
    }
    SET_COLORFILL
    FillRect(player->field_offset_x + (i * 8), player->field_offset_y, PIECEBUF_WIDTH, PIECEBUF_WIDTH, BG_COLOR);
    UNSET_COLORFILL
    SpriteRect(player->field_offset_x + (i * 8), player->field_offset_y,PIECEBUF_WIDTH, PIECEBUF_WIDTH, 106, 2);
    wait();

    if(player->pendingGarbage != 0) {
        SET_COLORFILL
        FillRect(
            player->field_offset_x + GRID_SPACING * FIELD_W + 2,
            player->field_offset_y + GRID_SPACING * FIELD_H - (player->pendingGarbage << 2),
            2,
            player->pendingGarbage << 2,
            6
        );
    }

    if(player->flags & PLAYER_DEAD) {
        UNSET_COLORFILL
        cursorX = player->field_offset_x + (GRID_SPACING * (FIELD_W/2 - 5));
        cursorY = player->field_offset_y + (GRID_SPACING * FIELD_H/2);
        print("game");
        cursorX = player->field_offset_x + (GRID_SPACING * ((FIELD_W/2) - 3));
        cursorY = player->field_offset_y + (GRID_SPACING * FIELD_H/2) + SPRITE_CHAR_H;
        print("over");
    }
}
