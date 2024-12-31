.code  $0000
..begin _start
_start:
;jump main
jp main
..end
.code  $100
..begin main
main:
;asm
 di
;asm
 ld sp, 0x1FFF
;asm
 im 1
;idhl = 3
ld hl, 3
;rb = .ptr
ld a, (0)
ld b, a
;SGA_selectedBank = NOS_state
ld hl, (17)
ld (18), hl
;NOS_state = SGA_selectedBank
ld a, (19)
ld (17), a
;asm
 ei
..end
