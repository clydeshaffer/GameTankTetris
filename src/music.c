#include "gametank.h"
#include "dynawave.h"
#include "note_numbers.h"

#define PATTERN_LEN 16
char patterns[PATTERN_LEN*6] = {
    En5, RST, Bn4, Cn5, Dn5, RST, Cn5, Bn4, An4, RST, An4, Cn5, En5, RST, Dn5, Cn5,
    Bn4, RST, Bn4, Cn5, Dn5, RST, En5, RST, Cn5, RST, An4, RST, An4, RST, RST, RST,
    Dn5, RST, RST, Fn5, An5, RST, Gn5, Fn5, En5, RST, Cn5, RST, En5, RST, Dn5, Cn5,
    En2, En3, En2, En3, En2, En3, En2, En3, An2, An3, An2, An3, An2, An3, An2, An3, 
    Af2, Af3, Af2, Af3, En2, En3, En2, En3, An2, An3, An2, An3, An2, An3, An2, An3,
    Dn2, Dn3, Dn2, Dn3, Dn2, Dn3, Dn2, Dn3, An2, An3, An2, An3, An2, An3, An2, An3, 
};

char sequence[4] = {
    0, 1, 2, 1
};

char sequence2[4] = {
    3, 4, 5, 4
};

#define BEAT_FRAMES 4
char metronome = BEAT_FRAMES;
char noteNum = 0;
char pattNum = 0;
char noteNum2 = 0;
char pattNum2 = 0;
char *currentPattern = patterns;
char *currentPattern2 = patterns;

void tick_music() {
    if(audio_amplitudes[0] > 0) {
        audio_amplitudes[0] -= 16;
    }
     /*if(audio_amplitudes[3] > 0) {
        audio_amplitudes[3] -= 16;
    }*/
    
    metronome--;
    if(metronome == 0) {
        if(currentPattern[noteNum] != RST) {
            SET_NOTE(0, (currentPattern[noteNum]-12) );
            audio_amplitudes[0] = 128;
        }
        noteNum = (noteNum + 1) % PATTERN_LEN;
        if(noteNum == 0) {
            pattNum = (pattNum + 1) % 4;
            currentPattern = &(patterns[sequence[pattNum]*PATTERN_LEN]);
        }

        if(currentPattern2[noteNum2] != RST) {
            SET_NOTE(3, (currentPattern2[noteNum2]-12) );
            audio_amplitudes[3] = 127;
        }
        noteNum2 = (noteNum2 + 1) % PATTERN_LEN;
        if(noteNum2 == 0) {
            pattNum2 = (pattNum2 + 1) % 4;
            currentPattern2 = &(patterns[sequence2[pattNum2]*PATTERN_LEN]);
        }
        metronome = BEAT_FRAMES;
    }
}