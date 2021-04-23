build:
	wine WDC02AS T0.ASM
	wine WDC02AS ASSETS.ASM
	wine WDC02CC -SP -SM gametank.c
	wine WDC02CC -SP -SM dynawave.c
	wine WDC02CC -SP -SM drawing_funcs.c
	wine WDC02CC -SP -SM tetris.c
	wine WDC02CC -SP -SM main.c
	wine WDCLN -HB -C8000 -D500, gametank.obj dynawave.obj drawing_funcs.obj tetris.obj main.obj assets.obj T0.obj -LC -T
	dd bs=32768 skip=1 if=main.bin of=bin/tetris.gtr