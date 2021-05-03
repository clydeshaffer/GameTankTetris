#ifndef DYNAWAVE_H

#define DYNAWAVE_H
/* Pointers specific to the "dynawave" audio co-program */
#define audio_freqs_h ((volatile char *) 0x3010)
#define audio_freqs_l ((volatile char *) 0x3020)
#define audio_amplitudes ((volatile char *) 0x3030)

#define SET_NOTE(ch, n) audio_freqs_h[ch] = pitch_table[ n * 2]; audio_freqs_l[ch] = pitch_table[ n * 2 + 1];

extern char pitch_table[216];

void init_dynawave();

#endif