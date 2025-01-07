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
;NOS_state = SGA_selectedBank2 + rc
ld d, 0
ld e, c
ld hl, (21)
add hl, de
ld a, l
ld (17), a
;SGA_selectedBank = NOS_state + rc
ld l, c
ld a, (17)
add a, l
ld h, 0
ld l, a
ld (18), hl
;SGA_selectedBank = SGA_selectedBank2 + rc
ld e, c
ld hl, (20)
add hl, de
ld (18), hl
;acc = 0
xor a
;acc = acc + acc
;NOS_state = idhl
ld a, l
ld (17), a
;NOS_state = idhl + idhl
add hl, hl
ld a, l
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
;NOS_state = rc + NOS_state
ld l, c
ld a, (17)
add a, l
ld (17), a
;NOS_state = .ptr + rbc
ld e, c
ld d, b
add (hl), de
ld a, (hl)
ld (17), a
;NOS_state = .ptr + .ptr
ld a, (hl)
rlca
ld (17), a
;asm
 ei
..end
