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
;NOS_state = 0
ld a, 0
ld 0, a
;SGA_selectedBank = 0
ld (18), a
;asm
 ei
..end
