/* 
 * Game logic implementation
 *
 * Mostly decoupled from the GameTank platform.
 * gametank.h is imported for the input button masks
 */
#include "tetris.h"
#include "gametank.h"

char tmpPieceBuf[PIECEBUF_SIZE];

const unsigned char rotation_matrix[PIECEBUF_SIZE] = {
    20,15,10, 5, 0,
    21,16,11, 6, 1,
    22,17,12, 7, 2,
    23,18,13, 8, 3,
    24,19,14, 9, 4
};

const unsigned char tetro_index[TET_COUNT] = {0, PIECEBUF_SIZE, PIECEBUF_SIZE*2, PIECEBUF_SIZE*3, PIECEBUF_SIZE*4, PIECEBUF_SIZE*5, PIECEBUF_SIZE*6};

/*
 * Tetrominoes in I J L O S T Z order
 * Values represent the color, 0=clear
 */
const unsigned char tetrominoes[PIECEBUF_SIZE*7] = {
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

/* Convert half-byte kick offsets to actual signed chars */
char k2o[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, -7, -6, -5, -4, -3, -2, -1};

/* Every shape except I and O */
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

int xorshift16(int x) {
    x |= x == 0;   /* if x == 0, set x = 1 instead */
    x ^= (x & 0x07ff) << 5;
    x ^= x >> 7;
    x ^= (x & 0x0003) << 14;
    return x;
}

int rnd_seed = 234;

int rnd() {
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
    rnd_seed = xorshift16(rnd_seed);
    rnd_seed = xorshift16(rnd_seed);
    return rnd_seed;
}

void init_piece(char type, PiecePos* pos, char* dest) {
    char r, c, i;
    type &= 0x7F;
    type = type % 7;
    i = tetro_index[type];
    pos->x = SPAWN_COL;
    pos->y = SPAWN_ROW;
    pos->t = type;
    pos->rot = 0;
    pos->lock = 0;
    for(r = 0; r < PIECEBUF_WIDTH; r++) {
        for(c = 0; c < PIECEBUF_WIDTH; c++) {
            *dest = tetrominoes[i];
            dest++;
            i++;
        }
    }
}

void place_at(PiecePos* pos, char* pieceBuf, char* field) {
    char r, c, i = 0, j;
    j = (pos->y * FIELD_W) + pos->x;
    j -= (FIELD_W*2 + 2);
    for(r = 0; r < PIECEBUF_WIDTH; r++) {
        for(c = 0; c < PIECEBUF_WIDTH; c++) {
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
    j = (pos->y * FIELD_W) + pos->x;
    j -= (FIELD_W*2) + 2;
    for(r = 0; r < PIECEBUF_WIDTH; r++) {
        for(c = 0; c < PIECEBUF_WIDTH; c++) {
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
        j += FIELD_W - PIECEBUF_WIDTH;
    }
    return 1;
}

void copyPiece(char* src, char* dest) {
    char r = 0;
    for(r = 0; r < PIECEBUF_SIZE; r++) {
        *dest = *src;
        dest++; src++;
    }
}

void rotateRight(char* pieceBuf) {
    char r, c, i = 0;

    copyPiece(pieceBuf, tmpPieceBuf);

    for(i=0;i<PIECEBUF_SIZE;i++) {
        *pieceBuf = tmpPieceBuf[rotation_matrix[i]];
        pieceBuf++;
    }
}

void rotateLeft(char* pieceBuf) {
    char r, c, i = 0;

    copyPiece(pieceBuf, tmpPieceBuf);

    for(i=0;i<PIECEBUF_SIZE;i++) {
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
        kicksrc = kicks_I + (NUM_KICKTESTS * pos->rot);
        kickdst = kicks_I + (NUM_KICKTESTS * newRot);
    } else {
        kicksrc = kicks_main + (NUM_KICKTESTS * pos->rot);
        kickdst = kicks_main + (NUM_KICKTESTS * newRot);
    }

    for(i = 0; i < NUM_KICKTESTS; i ++) {
        pos->x += k2o[(kicksrc[i] >> 4) & 15];
        pos->y -= k2o[kicksrc[i] & 15];
        pos->x -= k2o[(kickdst[i] >> 4) & 15];
        pos->y += k2o[kickdst[i] & 15];
        if(1 == test_at(pos, pieceBuf, field)) {
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

#define T_SPIN_NONE 0
#define T_SPIN_MINI 1
#define T_SPIN_FULL 2
const int cornerOffsets[5] = {
    0 - FIELD_W - 1,
    0 - FIELD_W + 1,
    0 + FIELD_W + 1,
    0 + FIELD_W - 1,
    0 - FIELD_W - 1
};

char checkTSpin(char* playField, PiecePos* pos) {
    char center = (pos->y * FIELD_W) + pos->x, count = 0;
    count += !!playField[center - FIELD_W - 1] || (pos->x == 0);
    count += !!playField[center - FIELD_W + 1] || (pos->x == FIELD_W-1);
    count += !!playField[center + FIELD_W - 1] || (pos->x == 0) || (pos->y == FIELD_H-1);
    count += !!playField[center + FIELD_W + 1] || (pos->x == FIELD_W-1) || (pos->y == FIELD_H-1);
    if(count > 2) {
        count = 0;
        count += !!playField[center + cornerOffsets[pos->rot]];
        count += !!playField[center + cornerOffsets[pos->rot+1]];

        if(count == 2) {
            return T_SPIN_FULL;
        } else {
            return T_SPIN_MINI;
        }
    } else {
        return T_SPIN_NONE;
    }
}

char checkLineClears(char* playField) {
    char r, c, i = (FIELD_W*FIELD_H) - 1, j, clearCount = 0, blocks = 0;
    for(r = FIELD_H-1; r >= 1; r--) {
        blocks = 0;
        for(c = 0; c < FIELD_W; c++) {
            blocks += !!playField[i--];
        }
        if(blocks == FIELD_W) {
            clearCount++;
            r++;
            i+=FIELD_W;
            for(j = i; j >= FIELD_W; j--) {
                playField[j] = playField[j-FIELD_W];
            }
        }
    }
    return clearCount;
}

char garbageTable[10] = {0, 0, 1, 2, 4, 0, 2, 4, 6, 0};
char comboGarbage[10] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5};

void addGarbage(char* playField, char amount) {
    char i, len, nlen;
    if(amount > FIELD_H) {
        amount = FIELD_H;
    }
    len = FIELD_W * amount;
    nlen = FIELD_W * (FIELD_H - amount);
    for(i = 0; i < nlen; i ++) {
        playField[i] = playField[i + len];
    }
    for(i = nlen; i < FIELD_H * FIELD_W; i++) {
        playField[i] = 5;
    }
    for(i = nlen; i < FIELD_H * FIELD_W; i+=FIELD_W) {
        playField[i + ((rnd() & 127) % FIELD_W)] = 0;
    }
}

void initPlayerState(PlayerState* player) {
    init_piece(rnd(), &(player->currentPos), player->currentPiece);
    player->fallRate = 10;
    player->fallTimer = 0;
    player->score = 0;
    player->heldPiece.rot = 0;
    player->heldPiece.t = TET_COUNT;
    player->flags = 0;
    player->pendingGarbage = 0;
    player->combo = 0;
}

char updatePlayerState(PlayerState* player, int inputs, int last_inputs) {
    char oldX, oldY, tmp, tSpinType, garbageOut = 0;
    if(player->flags & PLAYER_DEAD) {
        return 0;
    }
    oldX = player->currentPos.x;
        oldY = player->currentPos.y;
        if(player->fallTimer < player->fallRate){
            player->currentPos.y++;
        } else if(!(player->flags&PLAYER_DIDHOLD) && (inputs & INPUT_MASK_C & ~last_inputs)) {
            if(player->heldPiece.t == TET_COUNT) {
                tmp = rnd();
            } else {
                tmp = player->heldPiece.t;
            }
            player->heldPiece.t = player->currentPos.t;
            init_piece(tmp,  &(player->currentPos), player->currentPiece);
            player->fallTimer = 255 - player->fallRate;
            player->flags |= PLAYER_DIDHOLD;
        } else if(inputs & INPUT_MASK_UP & ~last_inputs) {
            player->currentPos.lock = LOCK_FRAMES+1;
            while(test_at(&(player->currentPos), player->currentPiece, player->playField)) {
                oldY = player->currentPos.y;
                player->currentPos.y++;
            }
        } else if(inputs & INPUT_MASK_A & ~last_inputs) {
            tryRotate(&(player->currentPos), player->currentPiece, player->playField, -1);
        } else if(inputs & INPUT_MASK_B & ~last_inputs) {
            tryRotate(&(player->currentPos),player-> currentPiece, player->playField, 1);
        } else {
            if(inputs & INPUT_MASK_LEFT) {
                player->currentPos.x--;
            }
            if(inputs & INPUT_MASK_RIGHT) {
                player->currentPos.x++;
            }
            if(inputs & INPUT_MASK_DOWN) {
            player->currentPos.y++;
            }
        }

        if(0 == test_at(&(player->currentPos), player->currentPiece, player->playField)){
            if(player->currentPos.x == oldX && player->currentPos.y == oldY) {
                player->flags |= PLAYER_DEAD;
            } else if(player->currentPos.y > oldY) {
                tmp = player->currentPos.x;
                player->currentPos.x = oldX;
                if(0 == test_at(&(player->currentPos), player->currentPiece, player->playField)){
                    player->currentPos.y = oldY;
                    player->currentPos.x = tmp;
                    if(tmp != oldX) {
                        if(0 == test_at(&(player->currentPos), player->currentPiece, player->playField)) {
                            player->currentPos.x = oldX;
                        } else {
                            player->currentPos.lock = 0;
                        }
                    }

                    if(player->currentPos.lock > LOCK_FRAMES) {
                        tSpinType = 0;
                        if(player->currentPos.t == TET_T) {
                            tSpinType = checkTSpin(player->playField, &(player->currentPos));
                        }

                        place_at(&(player->currentPos), player->currentPiece, player->playField);
                        tmp = checkLineClears(player->playField);

                        player->score += tmp;

                        garbageOut = garbageTable[tmp + (4 * (tSpinType == T_SPIN_FULL))];

                        if(tmp != 0) {
                            if((tmp == 4) || tSpinType == T_SPIN_FULL) {
                                if(player->flags & PLAYER_BACK_TO_BACK) {
                                    garbageOut++;
                                } else {
                                    player->flags |= PLAYER_BACK_TO_BACK;
                                }
                            } else {
                                player->flags &= ~PLAYER_BACK_TO_BACK;
                            }
                            if(player->combo > 9) {
                                garbageOut += comboGarbage[9];
                            } else {
                                garbageOut += comboGarbage[player->combo];
                            }
                            player->combo++;
                        } else {
                            player->combo = 0;
                        }

                        if(player->pendingGarbage > garbageOut) {
                            player->pendingGarbage -= garbageOut;
                            garbageOut = 0;
                        } else {
                            garbageOut -= player->pendingGarbage;
                            player->pendingGarbage = 0;
                        }
                        
                        if((tmp == 0) && (player->pendingGarbage != 0)) {
                            addGarbage(player->playField, player->pendingGarbage);
                            player->pendingGarbage = 0;
                        }

                        init_piece(rnd(),  &(player->currentPos), player->currentPiece);
                        player->fallTimer = 255 - player->fallRate;
                        player->flags &= ~PLAYER_DIDHOLD;
                    } else {
                        if((inputs & ~INPUT_MASK_DOWN) == 0) {
                            player->currentPos.lock+=10;
                        } else {
                            player->currentPos.lock++;
                        }
                    }
                }
            } else {
                player->currentPos.y = oldY;
                player->currentPos.x = oldX;   
            }
        }
        player->fallTimer+=player->fallRate;
        return garbageOut;
}
