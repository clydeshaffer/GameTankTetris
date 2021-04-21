//v1.0
//>> 2017 05 26
//draw background               o
//draw blocks                   o
//rotate blocks.                o
//go left, go right the block.  o
//>> 2017 05 29
//left, right limit             o
//rotate near the wall          o
//show the second block         o
//>> 2017 05 30
//drop the block.               o
//collision with bottom wall    o
//delete the correct line       o
//score                         o
//game over                     o
//space button                  o
//save the best score           o

#include "gametank.h"

#define FIELD_W 10
#define FIELD_H 20

#define TET_I 0
#define TET_J 1
#define TET_L 2
#define TET_O 3
#define TET_S 4
#define TET_T 5
#define TET_Z 6

typedef struct {
    char x, y, rot, t;
} PiecePos;

int inputs = 0, last_inputs = 0;
char frameflag, frameflip = DMA_VRAM_PAGE, flagsMirror, cursorX, cursorY;
char playField[FIELD_W*FIELD_H];
char currentPiece[25];
char tmpPieceBuf[25];
PiecePos currentPos;

const unsigned char rotation_matrix[25] = {
    20,15,10, 5, 0,
    21,16,11, 6, 1,
    22,17,12, 7, 2,
    23,18,13, 8, 3,
    24,19,14, 9, 4
};

const unsigned char tetro_index[7] = {0, 25, 50, 75, 100, 125, 150};

/*I J L O S T Z order*/
const unsigned char tetrominoes[25*7] = {
      0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,
      0,244,244,244,244,
      0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,

      0,  0,  0,  0,  0,
      0,219,  0,  0,  0,
      0,219,219,219,  0,
      0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,

      0,  0,  0,  0,  0,
      0,  0,  0, 92,  0,
      0, 92, 92, 92,  0,
      0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,

      0,  0,  0,  0,  0,
      0,  0, 61, 61,  0,
      0,  0, 61, 61,  0,
      0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,

      0,  0,  0,  0,  0,
      0,  0, 29, 29,  0,
      0, 29, 29,  0,  0,
      0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,

      0,  0,  0,  0,  0,
      0,  0,155,  0,  0,
      0,155,155,155,  0,
      0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,

      0,  0,  0,  0,  0,
      0,124,124,  0,  0,
      0,  0,124,124,  0,
      0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,
};

char k2o[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, -7, -6, -5, -4, -3, -2, -1};

char kicks_main[20] = {
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x1F, 0x02, 0x12,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xF0, 0xFF, 0x02, 0xF2,
};

char kicks_I[20] = {
    0x00, 0xF0, 0x20, 0xF0, 0x20,
    0xF0, 0x00, 0x00, 0x01, 0x0E,
    0xF1, 0x11, 0xE1, 0x10, 0xE0,
    0x01, 0x01, 0x01, 0x0F, 0x02,
};

void binprint(int num) {
    int i;
    vram[VY] = 0;
    vram[GX] = 0x80;
    vram[GY] = 0;
    vram[WIDTH] = 6;
    vram[HEIGHT] = 8;
    for(i = 0; i < 128; i+=8) {
        vram[VX] = i;
        vram[COLOR] = (num & 1) - 1;
        vram[START] = 1;
        asm wai;
        num = num >> 1;
    }
}

int xorshift16(int x) {
    x |= x == 0;   // if x == 0, set x = 1 instead
    x ^= (x & 0x07ff) << 5;
    x ^= x >> 7;
    x ^= (x & 0x0003) << 14;
    return x;
}

int rnd_seed = 0;

int rnd() {
    /*rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);*/
    rnd_seed++;
    return rnd_seed;
}


//get keyboard input.
void updateInputs(){
    char inputsA, inputsB;
    inputsA = *gamepad_2;
    inputsA = *gamepad_1;
    inputsB = *gamepad_1;

    last_inputs = inputs;
    inputs = ~((((int) inputsB) << 8) | inputsA);
}

void Sleep(int frames) {
    int i;
    for(i = 0; i < frames; i++) {
        frameflag = 1;
        while(frameflag) {}
    }
}

void CLS(char c) {
    vram[VX] = 0;
    vram[VY] = 0;
    vram[GX] = 0x80;
    vram[GY] = 0;
    vram[WIDTH] = 127;
    vram[HEIGHT] = 127;
    vram[COLOR] = ~c;
    vram[START] = 1;
    asm wai;
}

void FillRect(char x, char y, char w, char h, char c) {
    vram[VX] = x;
    vram[VY] = y;
    vram[GX] = 0x80;
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
    vram[GY] = 0x60;
    vram[WIDTH] = 8;
    vram[HEIGHT] = 8;
    if(num == 0) {
        vram[GX] = 0;
        vram[START] = 1;
        //asm wai;
    } else {
        while(num != 0) {
            vram[GX] = (num % 10) << 3;
            vram[START] = 1;
            cursorX -= 8;
            num = num / 10;
            vram[VX] = cursorX;
        }
    }
}

void print(char* str) {
    *dma_flags = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS | frameflip;
    while(*str != 0) {
        if(*str >= '0' && *str <= '9') {
            vram[GX] = (*str - '0') << 3;
            vram[GY] = 0x60;
        } else if(*str >= 'a' && *str <= 'f') {
            vram[GX] = ((*str - 'a') << 3) + 0x50;
            vram[GY] = 0x60;
        } else if(*str >= 'g' && *str <= 'v') {
            vram[GX] = (*str - 'g') << 3;
            vram[GY] = 0x68;
        } else if(*str >= 'w' && *str <= 'z') {
            vram[GX] = (*str - 'w') << 3;
            vram[GY] = 0x70;
        } else {
            vram[GX] = 0x70;
            vram[GY] = 0x70;
        }
        if(*str == '\n') {
            cursorX = 0;
            cursorY += 8;
        } else {
            vram[VX] = cursorX;
            vram[VY] = cursorY;
            vram[WIDTH] = 8;
            vram[HEIGHT] = 8;
            vram[START] = 1;
            asm wai;
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
    char r, c, i = 0;
    vram[GX] = 0x80;
    vram[GY] = 0;
    vram[WIDTH] = 4;
    vram[HEIGHT] = 4;
    for(r = 0; r < FIELD_H*4; r+=4) {
        for(c = 0; c < FIELD_W*4; c+=4) {
            vram[VX] = x + c;
            vram[VY] = y + r;
            vram[COLOR] = ~field[i++];
            vram[START] = 1;
        }
    }
}

void draw_piece(PiecePos* pos, char* piece, char offsetX, char offsetY) {
    char r, c, i = 0;
    vram[GX] = 0x80;
    vram[GY] = 0;
    vram[WIDTH] = 4;
    vram[HEIGHT] = 4;
    for(r = 0; r < 5*4; r+=4) {
        for(c = 0; c < 5*4; c+=4) {
            vram[VX] = 4*pos->x + c + offsetX - 8;
            vram[VY] = 4*pos->y + r + offsetY - 8;
            vram[COLOR] = ~piece[i++];
            vram[START] = 1;
        }
    }
}

void init_piece(char type, PiecePos* pos, char* dest) {
    char r, c, i;
    i = tetro_index[type];
    pos->x = 4;
    pos->y = 1;
    pos->t = type;
    pos->rot = 0;
    for(r = 0; r < 5; r++) {
        for(c = 0; c < 5; c++) {
            *dest = tetrominoes[i];
            dest++;
            i++;
        }
    }
}

void place_at(PiecePos* pos, char* pieceBuf, char* field) {
    char r, c, i = 0, j;
    j = (pos->y * 10) + pos->x;
    j -= 22;
    for(r = 0; r < 5; r++) {
        for(c = 0; c < 5; c++) {
            if(pieceBuf[i]) {
                field[j] = pieceBuf[i];
            }
            i++;
            j++;
        }
        j += 5;
    }
}

int test_at(PiecePos* pos, char* pieceBuf, char* field) {
    char r, c, i = 0, j;
    j = (pos->y * 10) + pos->x;
    j -= 22;
    for(r = 0; r < 5; r++) {
        for(c = 0; c < 5; c++) {
            if(pieceBuf[i]) {
                if((pos->x + c - 2) >= FIELD_W) {
                    return 0;
                } else if((pos->x + c - 2) == -1) {
                    return 0;
                }  else if((pos->y + r - 2) >= FIELD_H) {
                    return 0;
                } else if(field[j]) {
                    return 0;
                }
            }
            i++;
            j++;
        }
        j += 5;
    }
    return 1;
}

void copyPiece(char* src, char* dest) {
    char r = 0;
    for(r = 0; r < 25; r++) {
        *dest = *src;
        dest++; src++;
    }
}

void rotateRight(char* pieceBuf) {
    char r, c, i = 0;

    copyPiece(pieceBuf, tmpPieceBuf);

    for(i=0;i<25;i++) {
        *pieceBuf = tmpPieceBuf[rotation_matrix[i]];
        pieceBuf++;
    }
}

void rotateLeft(char* pieceBuf) {
    char r, c, i = 0;

    copyPiece(pieceBuf, tmpPieceBuf);

    for(i=0;i<25;i++) {
        pieceBuf[rotation_matrix[i]] = tmpPieceBuf[i];
    }
}

void tryRotate(PiecePos* pos, char* pieceBuf, char* field, char direction) {
    char newRot = (pos->rot + direction + 4) % 4;
    char oldX = pos->x, oldY = pos->y, i;
    char* kicksrc, *kickdst;
    if(pos->t == TET_O) {
        return;
    }
    if(direction == 1) {
        rotateRight(pieceBuf);
    } else {
        rotateLeft(pieceBuf);
    }
    if(pos->t == TET_I) {
        kicksrc = kicks_I + (5 * pos->rot);
        kickdst = kicks_I + (5 * newRot);
    } else {
        kicksrc = kicks_main + (5 * pos->rot);
        kickdst = kicks_main + (5 * newRot);
    }

    for(i = 0; i < 5; i ++) {
        pos->x += k2o[(kicksrc[i] >> 4) & 15];
        pos->y -= k2o[kicksrc[i] & 15];
        pos->x -= k2o[(kickdst[i] >> 4) & 15];
        pos->y += k2o[kickdst[i] & 15];
        if(1 == test_at(&currentPos, pieceBuf, field)) {
            pos->rot = newRot;
            return;
        }
        pos->x = oldX;
        pos->y = oldY;
    }


    if(direction == 1) {
        rotateLeft(pieceBuf);
    } else {
        rotateRight(pieceBuf);
    }
}

int checkLineClears(char* playField) {
    char r, c, i = (FIELD_W*FIELD_H) - 1, j, clearCount = 0, blocks = 0;
    for(r = FIELD_H-1; r >= 10; r--) {
        blocks = 0;
        for(c = 0; c < FIELD_W; c++) {
            blocks += !!playField[i--];
        }
        if(blocks == FIELD_W) {
            clearCount++;
            r++;
            i+=FIELD_W;
            for(j = i; j >= FIELD_W; j--) {
                playField[j] = playField[j-10];
            }
        }
    }
    return clearCount;
}

void main() {
    char oldX, oldY;
    unsigned char fallTimer = 0, fallRate = 8;
    int score = 0;
    asm sei;
    
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

    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS | frameflip;
    *dma_flags = flagsMirror;

    cursorX = 16;
    cursorY = 0;
    print("tetris");

    srand(0xABCD);
    init_piece(2, &currentPos, currentPiece);

    currentPos.x = 4;
    currentPos.y = 1;

    while(1){
        updateInputs();

        oldX = currentPos.x;
        oldY = currentPos.y;
        if(fallTimer < fallRate){
            currentPos.y++;
        } else if(inputs & INPUT_MASK_UP & ~last_inputs) {
            while(test_at(&currentPos, currentPiece, playField)) {
                oldY = currentPos.y;
                currentPos.y++;
            }
        } else if(inputs & INPUT_MASK_A & ~last_inputs) {
            tryRotate(&currentPos, currentPiece, playField, -1);
        } else if(inputs & INPUT_MASK_B & ~last_inputs) {
            tryRotate(&currentPos, currentPiece, playField, 1);
        } else if(inputs & INPUT_MASK_DOWN) {
            currentPos.y++;
        } else if(inputs & INPUT_MASK_LEFT) {
            currentPos.x--;
        } else if(inputs & INPUT_MASK_RIGHT) {
            currentPos.x++;
        }

        if(0 == test_at(&currentPos, currentPiece, playField)){
            if(currentPos.y > oldY) {
                currentPos.y = oldY;
                place_at(&currentPos, currentPiece, playField);
                score += checkLineClears(playField);

                init_piece(rnd() % 7,  &currentPos, currentPiece);
                fallTimer = 255 - fallRate;
            } else {
                currentPos.y = oldY;
                currentPos.x = oldX;   
            }
        }

        flagsMirror |= DMA_TRANS;
        *dma_flags = flagsMirror;
        CLS(3);
        FillRect(16, 16, 40, 80, 0);

        flagsMirror &= ~DMA_TRANS;
        *dma_flags = flagsMirror;
        draw_field(playField, 16, 16);
        draw_piece(&currentPos, currentPiece, 16, 16);

        cursorX = 100;
        cursorY = 40;
        printnum(score);

        frameflip ^= DMA_PAGE_OUT | DMA_VRAM_PAGE;
        flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS | frameflip;
        *dma_flags = flagsMirror;
        Sleep(1);
        fallTimer+=fallRate;
    }
}

void IRQHandler() {

}

void NMIHandler() {
    frameflag = 0;
}