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
;acc = NOS_sysMissedClock
ld a, (16)
;acc = NOS_sysMissedClock
ld a, 16
;acc = NOS_state
inc a
;acc = NOS_sysMissedClock
ld a, (16)
;asm
 ei
..end
