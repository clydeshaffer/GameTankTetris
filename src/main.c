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

extern void wait();
char i;
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

    initPlayerState(&(players[0]));
    players[0].playernum = 0;
    players[0].playField = playField_0;
    players[0].field_offset_x = 16;
    players[0].field_offset_y = 24;
    players[0].heldPiece.x = 0;
    players[0].heldPiece.y = 0;

    initPlayerState(&(players[1]));
    players[1].playernum = 1;
    players[1].playField = playField_1;
    players[1].field_offset_x = 72;
    players[1].field_offset_y = 24;
    players[1].heldPiece.x = 0;
    players[1].heldPiece.y = 0;

    for(i = 0; i < 220; ++i) {
        playField_0[i] = 0;
        playField_1[i] = 0;

    }

    while(1){
        updateInputs();
        tick_music();
        
        //CLS(BG_COLOR);
        
        *banking_reg = bankflip | 1;
        SpriteRect(0, 0, 127, 127, 0, 0);

        via[ORB] = 0x80;
        via[ORB] = 0x03;
        players[1].pendingGarbage += updatePlayerState(&(players[0]), inputs[0], last_inputs[0]);
        players[0].pendingGarbage += updatePlayerState(&(players[1]), inputs[1], last_inputs[1]);
        via[ORB] = 0x80;
        via[ORB] = 0x43;
        wait();
        *banking_reg = bankflip;
        via[ORB] = 0x80;
        via[ORB] = 0x00;
        drawPlayerState(&(players[0]));
        drawPlayerState(&(players[1]));
        via[ORB] = 0x80;
        via[ORB] = 0x40;

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
}