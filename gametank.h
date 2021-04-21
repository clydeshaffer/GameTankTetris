#define dma_flags ((volatile char *) 0x2007)
#define vram ((volatile char *) 0x4000)
#define vram_VX ((volatile char *) 0x4000)
#define vram_VY ((volatile char *) 0x4001)
#define vram_GX ((volatile char *) 0x4002)
#define vram_GY ((volatile char *) 0x4003)
#define vram_WIDTH ((volatile char *) 0x4004)
#define vram_HEIGHT ((volatile char *) 0x4005)
#define vram_COLOR ((volatile char *) 0x4007)
#define vram_START ((volatile char *) 0x4006)

#define DMA_ENABLE 1
#define DMA_PAGE_OUT 2
#define DMA_NMI 4
#define DMA_GRAM_PAGE 8
#define DMA_VRAM_PAGE 16
#define DMA_CPU_TO_VRAM 32
#define DMA_IRQ 64
#define DMA_TRANS 128

#define VX 0
#define VY 1
#define GX 2
#define GY 3
#define WIDTH 4
#define HEIGHT 5
#define START 6
#define COLOR 7

#define gamepad_1 ((volatile char *) 0x2008)
#define gamepad_2 ((volatile char *) 0x2009)

#define INPUT_MASK_UP		2056
#define INPUT_MASK_DOWN		1028
#define INPUT_MASK_LEFT		512
#define INPUT_MASK_RIGHT	256
#define INPUT_MASK_A		16
#define INPUT_MASK_B		4096
#define INPUT_MASK_C		8192
#define INPUT_MASK_START	32