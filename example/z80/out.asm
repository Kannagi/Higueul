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
;rb = .ptr
ld a, (hl)
ld b, a
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
;NOS_sysMissedClock = 0
xor a
ld (16), a
;NOS_state = 0
ld (17), a
;SGA_selectedBank = 0
ld hl, 0
ld (18), (hl)
;SGA_selectedBank = 1
inc hl
ld (18), hl
;rb = 0
;acc = rb
;idhl = 3
ld hl, 3
;SGA_selectedBank = ra
ld (18), a
;asm
 ei
..end
