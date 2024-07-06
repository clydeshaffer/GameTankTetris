    .export _GameSprites, _DynaWave, _BGSprite

    .RODATA
_GameSprites:
    .incbin "gamesprites.gtg.deflate"

_BGSprite:
    .incbin "bg.gtg.deflate"

_DynaWave:
    .incbin "dynawave.acp.deflate"