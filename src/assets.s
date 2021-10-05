    .export _GameSprites, _DynaWave, _BackgroundImage

    .RODATA
_GameSprites:
    .incbin "gamesprites.gtg.deflate"
_BackgroundImage:
    .incbin "background.gtg.deflate"

_DynaWave:
    .incbin "dynawave.acp.deflate"