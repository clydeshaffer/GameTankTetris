#include "gametank.h"
#include "dynawave.h"
#include "music.h"
#include "tetris.h"
#include "drawing_funcs.h"

#define DEFAULT_DMA 197 //DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS

PlayerState players[2];

int inputs = 0, last_inputs = 0;
int inputs2 = 0, last_inputs2 = 0;

extern void wait();

#pragma optimize(off)
void updateInputs(){
    char inputsA, inputsB;
    inputsA = *gamepad_2;
    inputsA = *gamepad_1;
    inputsB = *gamepad_1;

    last_inputs = inputs;
    inputs = ~((((int) inputsB) << 8) | inputsA);
    inputs &= INPUT_MASK_ALL_KEYS;

    inputsA = *gamepad_2;
    inputsB = *gamepad_2;
    last_inputs2 = inputs2;
    inputs2 = ~((((int) inputsB) << 8) | inputsA);
    inputs2 &= INPUT_MASK_ALL_KEYS;
}
#pragma optimize(on)

void Sleep(int frames) {
    int i;
    for(i = 0; i < frames; i++) {
        frameflag = 1;
        while(frameflag) {}
    }
}

void main() {

    via[DDRB] = 0xFF;

    frameflip = DMA_PAGE_OUT;

    init_dynawave();

    load_spritesheet();
    init_tetromino_minis();

    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS | frameflip;
    *dma_flags = flagsMirror;
    CLB(0);
    frameflip ^= DMA_PAGE_OUT | DMA_VRAM_PAGE;
    flagsMirror = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_TRANS | frameflip;
    *dma_flags = flagsMirror;
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
        //CLS(BG_COLOR); //Start CLS first to parallelize with update
        *dma_flags = flagsMirror | DMA_GRAM_PAGE;
        SpriteRect(1,7, SCREEN_WIDTH-2, SCREEN_HEIGHT-7-8, 1, 0);

        tick_music();
        updateInputs();
        players[1].pendingGarbage += updatePlayerState(&(players[0]), inputs, last_inputs);
        players[0].pendingGarbage += updatePlayerState(&(players[1]), inputs2, last_inputs2);

        wait();
        *dma_flags = flagsMirror;
        via[ORB] = 0x80;
        via[ORB] = 0x00;
        drawPlayerState(&(players[0]));
        via[ORB] = 0x80;
        via[ORB] = 0x40;
        via[ORB] = 0x80;
        via[ORB] = 0x01;
        drawPlayerState(&(players[1]));
        via[ORB] = 0x80;
        via[ORB] = 0x41;

        frameflip ^= DMA_PAGE_OUT | DMA_VRAM_PAGE;
        flagsMirror = DEFAULT_DMA | frameflip;
        *dma_flags = flagsMirror;
        Sleep(1);
    }
}

void IRQHandler() {

}

void NMIHandler() {
    frameflag = 0;
}