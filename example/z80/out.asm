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
;acc = 0
xor a
;NOS_state = rd + acc
add a, d
ld (17), a
;acc = rc + acc
add a, c
;acc = rc + acc
add a, c
;SGA_selectedBank = rc
ld h, 0
ld l, c
ld (18), hl
;NOS_state = rc + acc
add a, c
ld (17), a
;asm
 ei
..end
