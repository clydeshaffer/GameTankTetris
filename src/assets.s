    .export _GameSprites, _DynaWave, _BGSprite, _TitleSprite

    .RODATA
_GameSprites:
    .incbin "gamesprites.gtg.deflate"

_BGSprite:
    .incbin "bg.gtg.deflate"

_TitleSprite:
    .incbin "title.gtg.deflate"

_DynaWave:
    .incbin "dynawave.acp.deflate"