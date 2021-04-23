#include "dynawave.h"
#include "gametank.h"

void init_dynawave()
{
    *audio_rate = 0x7F;
    asm {
        xref InflateParams
        xref Inflate
        xref DynaWave
        LDA #<DynaWave
        STA InflateParams
        LDA #>DynaWave
        STA InflateParams+1
        LDA #$00
        STA InflateParams+2
        LDA #$30
        STA InflateParams+3
        JSR Inflate
    };
    *audio_reset = 0;
    *audio_rate = 255;
}