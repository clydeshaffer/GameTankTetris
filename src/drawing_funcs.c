/*
 * GameTank-specific implementation of drawing functions
 */
#include "drawing_funcs.h"
#include "gametank.h"

char cursorX, cursorY;

void load_spritesheet() {
    flagsMirror = DMA_NMI | DMA_IRQ;
    *dma_flags = flagsMirror;
    asm {
        xref GameSprites
        xref InflateParams
        xref Inflate
        LDA #<GameSprites
        STA InflateParams
        LDA #>GameSprites
        STA InflateParams+1
        LDA #$00
        STA InflateParams+2
        LDA #$40
        STA InflateParams+3
        JSR Inflate
    };
}

void CLS(char c) {
    vram[VX] = 0;
    vram[VY] = 0;
    vram[GX] = DMA_GX_SOLIDCOLOR_FLAG;
    vram[GY] = 0;
    vram[WIDTH] = SCREEN_WIDTH-1;
    vram[HEIGHT] = SCREEN_HEIGHT-1;
    vram[COLOR] = ~c;
    vram[START] = 1;
    asm wai;
}

void FillRect(char x, char y, char w, char h, char c) {
    vram[VX] = x;
    vram[VY] = y;
    vram[GX] = DMA_GX_SOLIDCOLOR_FLAG;
    vram[GY] = 0;
    vram[WIDTH] = w;
    vram[HEIGHT] = h;
    vram[COLOR] = ~c;
    vram[START] = 1;
    asm wai;
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
        asm {
                nop
                nop
                nop
                nop
                nop
                nop
                nop
                nop
                nop
                nop
            }
    } else {
        while(num != 0) {
            vram[GX] = (num % 10) << 3;
            vram[START] = 1;
            asm {
                nop
                nop
                nop
                nop
                nop
                nop
                nop
                nop
                nop
                nop
            }
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
            asm {
                nop
                nop
                nop
                nop
                nop
                nop
                nop
                nop
                nop
                nop
            }
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
    vram[GX] = DMA_GX_SOLIDCOLOR_FLAG;
    vram[GY] = 0;
    vram[WIDTH] = GRID_SPACING;
    vram[HEIGHT] = GRID_SPACING;
    vy = y;
    for(r = 0; r < FIELD_H; r++) {
        vx = x;
        for(c = 0; c < FIELD_W; c++) {
            f = *field;
            if(f) {
                vram[VX] = vx;
                vram[VY] = vy;
                vram[COLOR] = f ^ 0xFF;
                vram[START] = 1;
                asm {
                    nop
                    nop
                    nop
                    nop
                    nop
                }
            }
            vx+=GRID_SPACING;
            field++;
        }
        vy+=GRID_SPACING;
    }
}

void draw_piece(PiecePos* pos, const char* piece, char offsetX, char offsetY) {
    char r, c, i = 0;
    vram[GX] = DMA_GX_SOLIDCOLOR_FLAG;
    vram[GY] = 0;
    vram[WIDTH] = GRID_SPACING;
    vram[HEIGHT] = GRID_SPACING;
    for(r = 0; r < PIECEBUF_WIDTH*GRID_SPACING; r+=GRID_SPACING) {
        for(c = 0; c < PIECEBUF_WIDTH*GRID_SPACING; c+=GRID_SPACING) {
            if(!!piece[i]) {
                vram[VX] = GRID_SPACING*pos->x + c + offsetX - 2*GRID_SPACING;
                vram[VY] = GRID_SPACING*pos->y + r + offsetY - 2*GRID_SPACING;
                vram[COLOR] = ~piece[i];
                vram[START] = 1;
                asm {
                    nop
                    nop
                    nop
                    nop
                    nop
                }
            }
            i++;
        }
    }
}

void drawPlayerState(PlayerState* player) {
    flagsMirror |= DMA_TRANS;
    *dma_flags = flagsMirror;
    FillRect(player->field_offset_x, player->field_offset_y, GRID_SPACING * FIELD_W, GRID_SPACING * FIELD_H, 0);

    flagsMirror &= ~DMA_TRANS;
    *dma_flags = flagsMirror;
    draw_field(player->playField, player->field_offset_x, player->field_offset_y);
    draw_piece(&(player->currentPos), player->currentPiece, player->field_offset_x, player->field_offset_y);


    FillRect(player->field_offset_x + player->heldPiece.x*GRID_SPACING - GRID_SPACING*2,
        player->field_offset_y + player->heldPiece.y*GRID_SPACING - GRID_SPACING*2,
        GRID_SPACING * PIECEBUF_WIDTH, GRID_SPACING * PIECEBUF_WIDTH, 1);

    if(player->heldPiece.t != TET_COUNT) {
        draw_piece(
            &(player->heldPiece),
            &(tetrominoes[tetro_index[player->heldPiece.t]]),
            player->field_offset_x, player->field_offset_y);
    }

    FillRect(player->field_offset_x, player->field_offset_y, GRID_SPACING * FIELD_W, 6, 3);

    cursorX = player->field_offset_x + (GRID_SPACING * FIELD_W - SPRITE_CHAR_W);
    cursorY = player->field_offset_y + (GRID_SPACING * FIELD_H);
    printnum(player->score);

    FillRect(
        player->field_offset_x + GRID_SPACING * FIELD_W,
        player->field_offset_y + GRID_SPACING * FIELD_H - GRID_SPACING * player->pendingGarbage,
        1,
        player->pendingGarbage * GRID_SPACING,
        6
    );

    if(player->flags & PLAYER_DEAD) {
        cursorX = player->field_offset_x + (GRID_SPACING * (FIELD_W/2 - 5));
        cursorY = player->field_offset_y + (GRID_SPACING * FIELD_H/2);
        print("game");
        cursorX = player->field_offset_x + (GRID_SPACING * ((FIELD_W/2) - 3));
        cursorY = player->field_offset_y + (GRID_SPACING * FIELD_H/2) + SPRITE_CHAR_H;
        print("over");
    }
}
