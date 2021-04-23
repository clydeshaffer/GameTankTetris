#ifndef DYNAWAVE_H

#define DYNAWAVE_H
/* Pointers specific to the "dynawave" audio co-program */
#define audio_freqs_h ((volatile char *) 0x3010)
#define audio_freqs_l ((volatile char *) 0x3020)
#define audio_amplitudes ((volatile char *) 0x3030)

void init_dynawave();

#endif