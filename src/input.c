#include "input.h"
#include "gametank.h"

unsigned int inputs[NUM_CONTROLLERS];
unsigned int last_inputs[NUM_CONTROLLERS];
char inputTimes[NUM_BUTTONS * NUM_CONTROLLERS];

#pragma optimize(off)
void updateInputs(){
    char inputsA, inputsB;
    last_inputs[0] = inputs[0];
    last_inputs[1] = inputs[1];
    inputsA = *gamepad_2;
    inputsA = *gamepad_1;
    inputsB = *gamepad_1;

    inputs[0] = ~((((int) inputsB) << 8) | inputsA);
    inputs[0] &= INPUT_MASK_ALL_KEYS;

    inputsA = *gamepad_2;
    inputsB = *gamepad_2;
    inputs[1] = ~((((int) inputsB) << 8) | inputsA);
    inputs[1] &= INPUT_MASK_ALL_KEYS;
}
#pragma optimize(on)