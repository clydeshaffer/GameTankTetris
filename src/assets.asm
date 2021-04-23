    PAGE0
    ORG $F0
    xdef InflateParams
InflateParams rmb 4
    ENDS

;UDATA used by INFLATE
INFLATED SECTION
    ORG $200
    rmb 765
    ENDS

    xdef Inflate
Inflate SECTION
    ORG $E000
Inflate insert inflate_e000_0200.obx
    ENDS
    
    KDATA
    xdef GameSprites
GameSprites insert gamesprites.gtg.deflate

    xdef DynaWave
DynaWave    insert  dynawave.acp.deflate
    ENDS