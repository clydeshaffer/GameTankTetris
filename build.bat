WDC02AS T0.ASM
WDC02AS ASSETS.ASM
WDC02CC -SP -SM main.c
WDCLN -HB -C8000 -D500, main.obj assets.obj T0.obj -LC -T
dd bs=32768 skip=1 if=main.bin of=main.gtr