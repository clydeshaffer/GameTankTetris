CC = cc65
AS = ca65
LN = ld65

CFLAGS = -t none -O --cpu 65sc02
AFLAGS = --cpu 65C02 --bin-include-dir lib --bin-include-dir assets
LFLAGS = -C gametank-32k.cfg -m $(ODIR)/out.map
LLIBS = lib/gametank.lib

SDIR = src
ODIR = build

_COBJS = gametank.o dynawave.o music.o drawing_funcs.o input.o tetris.o main.o
COBJS = $(patsubst %,$(ODIR)/%,$(_COBJS))
_AOBJS = assets.o wait.o vectors.o interrupt.o
AOBJS = $(patsubst %,$(ODIR)/%,$(_AOBJS))

ASSETDEPS = assets/gamesprites.gtg.deflate assets/bg.gtg.deflate assets/title.bmp lib/dynawave.acp.deflate lib/inflate_e000_0200.obx

bin/tetris.gtr: $(AOBJS) $(COBJS) $(LLIBS) sprites
	mkdir -p $(@D)
	$(LN) $(LFLAGS) $(AOBJS) $(COBJS) -o $@ $(LLIBS)
	head -c 16384 $@ > $@.bankFE
	tail -c 16384 $@ > $@.bankFF

$(ODIR)/assets.o: src/assets.s sprites
	mkdir -p $(@D)
	$(AS) $(AFLAGS) -o $@ $<

$(ODIR)/%.si: src/%.c src/%.h
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

$(ODIR)/%.si: src/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

$(ODIR)/%.o: $(ODIR)/%.si
	mkdir -p $(@D)
	$(AS) $(AFLAGS) -o $@ $<

$(ODIR)/%.o: src/%.s
	mkdir -p $(@D)
	$(AS) $(AFLAGS) -o $@ $<

lib/gametank.lib: src/crt0.s
	$(AS) src/crt0.s -o build/crt0.o
	ar65 a lib/gametank.lib build/crt0.o

sprites: assets/gamesprites.bmp assets/bg.bmp
	cd assets ;\
	tail -c 16384 gamesprites.bmp > gamesprites.gtg ;\
	zopfli --deflate gamesprites.gtg ;\
	tail -c 16384 title.bmp > title.gtg ;\
	zopfli --deflate title.gtg ;\
	tail -c 16384 bg.bmp > bg.gtg ;\
	zopfli --deflate bg.gtg

.PHONY: clean

clean:
	rm -rf $(ODIR)/*