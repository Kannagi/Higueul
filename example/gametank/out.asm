.org  $0000
.code  $C000
..begin _start
_start:
;asm
 
	sei
	cld
	ldx #$FE
	txs

	
;AUDIO_RATE = 0x00
stz 8198
;FRAME_FLAG = 0
stz 1536
;rectX = 30
lda #30
sta 1537
;rectY = 10
lda #10
sta 1538
;velX = 1
lda #1
sta 1539
;velY = 1
lda #1
sta 1540
;jump main
jmp main
..end
..begin VBlank
VBlank:
;FRAME_FLAG = 0
stz 1536
;asm
 rti
..end
..begin IRQ
IRQ:
;asm
 rti
..end
..begin main
main:
.label_b0:
;call BlitRectangle
lda 1537
sta @BlitRectangle..arg0
lda 1538
sta @BlitRectangle..arg1
lda #16
sta @BlitRectangle..arg2
lda #16
sta @BlitRectangle..arg3
lda #28
sta @BlitRectangle..arg4
jsr BlitRectangle
;VIDEO_CTRL = 77
lda #77
sta 8199
;BLIT_DSTX = 0
stz 16384
;BLIT_DSTY = 0
stz 16385
;BLIT_WIDTH = 127
lda #127
sta 16388
;BLIT_HEIGHT = 127
lda #127
sta 16389
;BLIT_COLOR = 252
lda #252
sta 16391
;BLIT_START = 1
lda #1
sta 16390
.db  $CB,
;BLIT_DSTX = rectX
lda 1537
sta 16384
;BLIT_DSTY = rectY
lda 1538
sta 16385
;BLIT_WIDTH = 16
lda #16
sta 16388
;BLIT_HEIGHT = 16
lda #16
sta 16389
;BLIT_COLOR = 228
lda #228
sta 16391
;BLIT_START = 1
lda #1
sta 16390
.db  $CB,
;rectX = rectX + velX
lda 1537
clc
adc 1539
sta 1537
;rectY = rectY + velY
lda 1538
clc
adc 1540
sta 1538
;if rectX == 111
lda 1537
cmp #111
bne .label_1
;velX = 255
lda #255
sta 1539
.label_1:
;if rectX == 0
lda 1537
cmp #0
bne .label_2
;velX = 1
lda #1
sta 1539
.label_2:
;if rectY == 111
lda 1538
cmp #111
bne .label_3
;velY = 255
lda #255
sta 1540
.label_3:
;if rectY == 0
lda 1538
cmp #0
bne .label_4
;velY = 1
lda #1
sta 1540
.label_4:
;call waitVBlank
jsr waitVBlank
.label_0:
;while 1 == 1

jmp .label_b0
..end
..begin waitVBlank
waitVBlank:
;FRAME_FLAG = 1
lda #1
sta 1536
.label_b5:
.label_5:
;while FRAME_FLAG == 1
lda 1536
cmp #1
beq .label_b5
rts
..end
..begin BlitRectangle
BlitRectangle:
;VIDEO_CTRL = 73
lda #73
sta 8199
;BLIT_DSTX = x
lda 224
sta 16384
;BLIT_DSTY = y
lda 225
sta 16385
;BLIT_WIDTH = w
lda 226
sta 16388
;BLIT_HEIGHT = h
lda 227
sta 16389
;BLIT_COLOR = color
lda 228
sta 16391
;BLIT_START = 1
lda #1
sta 16390
.db  $CB,
rts
..end
.org  $1FFA
.dw @VBlank,
.dw @_start,
.dw @IRQ,
.rodata  $0000
