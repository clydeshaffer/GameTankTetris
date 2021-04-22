build:
	wine WDC02AS T0.ASM
	wine WDC02AS ASSETS.ASM
	wine WDC02CC -SP -SM main.c
	wine WDCLN -HB -C8000 -D500, main.obj assets.obj T0.obj -LC -T
	dd bs=32768 skip=1 if=main.bin of=bin/tetris.gtr