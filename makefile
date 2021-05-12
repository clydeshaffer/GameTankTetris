ifeq ($(OS), Windows_NT)
	CC = WDC02CC
	AS = WDC02AS
	LN = WDCLN
else
	CC = wine WDC02CC
	AS = wine WDC02AS
	LN = wine WDCLN
endif

SDIR = src
ODIR = build

_COBJS = gametank.obj dynawave.obj music.obj drawing_funcs.obj tetris.obj main.obj
COBJS = $(patsubst %,$(ODIR)/%,$(_COBJS))
_AOBJS = assets.obj boot.obj
AOBJS = $(patsubst %,$(ODIR)/%,$(_AOBJS))

CFLAGS = -SP -SM -I src -DUSING_02
AFLAGS = -I assets -I lib -DUSING_02
LFLAGS = -HB -C8000 -D500,
LLIBS = -LC

ASSETDEPS = assets/gamesprites.gtg.deflate lib/dynawave.acp.deflate lib/inflate_e000_0200.obx

$(ODIR)/%.obj: src/%.c src/%.h
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

$(ODIR)/%.obj: src/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

$(ODIR)/%.obj: src/%.asm
	mkdir -p $(@D)
	$(AS) $(AFLAGS) -o $@ $<

bin/tetris.gtr: $(ODIR)/tetris.bin
	dd bs=32768 skip=1 if=$< of=$@

$(ODIR)/tetris.bin: $(COBJS) $(AOBJS)
	mkdir -p $(@D)
	$(LN) $(LFLAGS) $^ -o $@ $(LLIBS)

.PHONY: clean

clean:
	rm -rf $(ODIR)/*