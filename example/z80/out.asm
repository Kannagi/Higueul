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
;ra = .ptr
ld a, (hl)
;SGA_selectedBank = ra
ld h, 0
ld l, a
ld (18), (hl)
;NOS_sysMissedClock = .ptr
ld a, (ix+0)
ld (16), a
;NOS_sysMissedClock = 0
xor a
ld (16), a
;NOS_state = 0
ld (17), a
;SGA_selectedBank = 0
ld hl, 0
ld (18), hl
;SGA_selectedBank = 1
inc hl
ld (18), hl
;rb = 0
;acc = rb
;idhl = 3
ld hl, 3
;rb = 22
ld b, 22
;.ptr = .ptr
ld a, (hl)
ld (bc), a
inc bc
inc hl
ld a, (hl)
ld (bc), a
dec bc
dec hl
;.ptr = .ptr
ld a, (ix+0)
ld (bc), a
inc bc
inc ix
ld a, (ix+0)
ld (bc), a
dec bc
dec ix
;NOS_state = SGA_selectedBank
ld a, (19)
ld (17), a
;SGA_selectedBank = SGA_selectedBank2
ld hl, (20)
ld (18), hl
;asm
 ei
..end
