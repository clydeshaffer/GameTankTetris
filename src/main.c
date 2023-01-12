#include "gametank.h"
#include "input.h"
#include "dynawave.h"
#include "music.h"
#include "tetris.h"
#include "drawing_funcs.h"

PlayerState players[2];

void Sleep(int frames) {
    int i;
    for(i = 0; i < frames; i++) {
        frameflag = 1;
        while(frameflag) {}
    }
}

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
    players[0].field_offset_x = 16;
    players[0].field_offset_y = 24;
    players[0].heldPiece.x = 0;
    players[0].heldPiece.y = 0;

    initPlayerState(&(players[1]));
    players[1].field_offset_x = 72;
    players[1].field_offset_y = 24;
    players[1].heldPiece.x = 0;
    players[1].heldPiece.y = 0;

    while(1){
        tick_music();

        updateInputs();
        CLS(BG_COLOR);
        via[ORB] = 0x80;
        via[ORB] = 0x00;
        drawBackground(&(players[0]));
        players[1].pendingGarbage += updatePlayerState(&(players[0]), inputs, last_inputs);
        drawBackground(&(players[1]));
        players[0].pendingGarbage += updatePlayerState(&(players[1]), inputs2, last_inputs2);
        via[ORB] = 0x80;
        via[ORB] = 0x40;

        via[ORB] = 0x80;
        via[ORB] = 0x01;
        drawPlayerState(&(players[0]));
        drawPlayerState(&(players[1]));
        via[ORB] = 0x80;
        via[ORB] = 0x41;

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
    frameflag = 0;
}