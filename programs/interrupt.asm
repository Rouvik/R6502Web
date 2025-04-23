* = 0x600

ldx #0
lda #50

loop:
sta $02,X
inx
jmp loop

interrupt:
pha
lda #$FF
sta $00
pla
rti

nonmaskable:
pha
lda #$A0
sta $01
pla
rti