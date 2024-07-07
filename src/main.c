#include "gametank.h"
#include "./input.h"
#include "dynawave.h"
#include "music.h"
#include "tetris.h"
#include "drawing_funcs.h"

void Sleep(int frames) {
    int i;
    for(i = 0; i < frames; i++) {
        frameflag = 1;
        while(frameflag) {}
    }
}

#define GAME_STATE_TITLE 0
#define GAME_STATE_PLAY_SINGLE 2
#define GAME_STATE_PLAY_DUEL 3
#define GAME_STATE_SINGLE_END 4
#define GAME_STATE_DUEL_END 5
#define STATE_FLAG_END 4
#define STATE_FLAG_DUEL 1

extern void wait();
char i;
char did_init_music = 0;
char music_cnt = 0;
char game_state = GAME_STATE_TITLE;
char mode_select = 0;
void main() {

    init_dynawave();

    load_spritesheet();
    init_tetromino_minis();

    *dma_flags = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS;
    *banking_reg = 0;
    FillRect(0, SCREEN_HEIGHT-1, SCREEN_WIDTH - 1, 1, 0);
    *dma_flags = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS | DMA_PAGE_OUT;
    *banking_reg = BANK_VRAM_MASK;
    FillRect(0, SCREEN_HEIGHT-1, SCREEN_WIDTH - 1, 1, 0);
    *dma_flags = DMA_NMI | DMA_CPU_TO_VRAM;
    *banking_reg = 0;
    vram[SCREEN_HEIGHT*SCREEN_WIDTH-1] = 0;
    *banking_reg = BANK_VRAM_MASK;
    vram[SCREEN_HEIGHT*SCREEN_WIDTH-1] = 0;

    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS | frameflip;
    *dma_flags = flagsMirror;
    CLB(0);
    frameflip ^= DMA_PAGE_OUT;
    bankflip ^= BANK_VRAM_MASK;
    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS | frameflip | DMA_AUTOTILE;
    *dma_flags = flagsMirror;
    *banking_reg = bankflip;
    CLB(0);

    
    players[0].playernum = 0;
    players[0].playField = playField_0;
    players[0].field_offset_x = 16;
    players[0].field_offset_y = 24;

    players[1].playernum = 1;
    players[1].playField = playField_1;
    players[1].field_offset_x = 72;
    players[1].field_offset_y = 24;

    while(1){
        updateInputs();
        
        if(game_state == GAME_STATE_TITLE) {
            *banking_reg = bankflip | 2;
            SpriteRect(0, 0, 127, 127, 0, 0);
            if(inputs[0]& ~last_inputs[0] & INPUT_MASK_START) {
                if(mode_select)
                    game_state = GAME_STATE_PLAY_DUEL;
                else
                    game_state = GAME_STATE_PLAY_SINGLE;
                init_music();
                music_cnt = 0;
                did_init_music = 1;
                for(i = 0; i < 220; ++i) {
                    playField_0[i] = 0;
                    playField_1[i] = 0;
                }
                initPlayerState(&(players[0]));
                initPlayerState(&(players[1]));
            }
            if(inputs[0] & ~last_inputs[0] & (INPUT_MASK_UP | INPUT_MASK_DOWN)) {
                mode_select ^= 16;
            }
            rnd();
            wait();
            flagsMirror &= ~DMA_TRANS;
            *dma_flags = flagsMirror;
            *banking_reg = bankflip;
            SpriteRect(23, 73 + mode_select, 8, 7, 88, 113);
        } else {
            *banking_reg = bankflip | 1;
            SpriteRect(0, 0, 127, 127, 0, 0);

            via[ORB] = 0x80;
            via[ORB] = 0x03;

            if(game_state & STATE_FLAG_END) {
                if(inputs[0] & ~last_inputs[0] & INPUT_MASK_START) {
                    game_state = GAME_STATE_TITLE;    
                } else if((game_state & STATE_FLAG_DUEL) && (inputs[1] & ~last_inputs[1] & INPUT_MASK_START)) {
                    game_state = GAME_STATE_TITLE;    
                }
                if(game_state == GAME_STATE_TITLE) {
                    initPlayerState(&(players[0]));
                    initPlayerState(&(players[1]));
                }
            } else {
                players[1].pendingGarbage += updatePlayerState(&(players[0]), inputs[0], last_inputs[0]);
                if(game_state & STATE_FLAG_DUEL)
                    players[0].pendingGarbage += updatePlayerState(&(players[1]), inputs[1], last_inputs[1]);
            }

            if((players[0].flags | players[1].flags) & PLAYER_DEAD) {
                game_state |= STATE_FLAG_END;
                did_init_music = 0;
                silence_audio();
            }
            via[ORB] = 0x80;
            via[ORB] = 0x43;
            wait();
            *banking_reg = bankflip;
            via[ORB] = 0x80;
            via[ORB] = 0x00;
            drawPlayerState(&(players[0]));
            if(game_state & STATE_FLAG_DUEL)
                drawPlayerState(&(players[1]));

            
            via[ORB] = 0x80;
            via[ORB] = 0x40;
        }
        frameflip ^= DMA_PAGE_OUT;
        bankflip ^= BANK_VRAM_MASK;
        flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS | frameflip | DMA_AUTOTILE;
        *dma_flags = flagsMirror;
        *banking_reg = bankflip;
        Sleep(1);
    }
}

void IRQHandler() {

}

void NMIHandler() {
    ++nmi_count;
    frameflag = 0;
    if(did_init_music) {
        if(++music_cnt == 3) {
            music_cnt = 0;
            tick_music();
        }
    }
}