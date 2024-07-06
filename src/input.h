#ifndef INCLUDE_H
#define INCLUDE_H

#define NUM_BUTTONS 8
#define NUM_CONTROLLERS 2

#define BUTTON_U 0
#define BUTTON_D 1
#define BUTTON_L 2
#define BUTTON_R 3
#define BUTTON_A 4
#define BUTTON_B 5
#define BUTTON_C 6
#define BUTTON_S 7

#define BUTTONS_P1 0
#define BUTTONS_P2 8

extern unsigned int inputs[NUM_CONTROLLERS];
extern unsigned int last_inputs[NUM_CONTROLLERS];
extern char inputTimes[NUM_BUTTONS * NUM_CONTROLLERS];

void updateInputs();

#endif