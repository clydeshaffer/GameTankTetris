WDC02AS T0.ASM
WDC02AS ASSETS.ASM
WDC02CC -SP -SM gametank.c
WDC02CC -SP -SM dynawave.c
WDC02CC -SP -SM drawing_funcs.c
WDC02CC -SP -SM tetris.c
WDC02CC -SP -SM main.c
WDCLN -HB -C8000 -D500, gametank.obj dynawave.obj drawing_funcs.obj tetris.obj main.obj assets.obj T0.obj -LC -T
dd bs=32768 skip=1 if=main.bin of=bin/tetris.gtr