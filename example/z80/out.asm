.code  $8000
..begin _start
_start:
;asm
 
	

	
;PPU_CTRL1 = 0x88
ld a,136
ld (0),a
;PPU_CTRL2 = 0x1E
ld a,30
ld (1),a
;jump main
jp main
..end
..begin main
main:
;.ptr = 0x1234
ld a,52
ld (6),a
;.ptr = testb + 0x1001
ld a,(2)
ld b,1
add a,b
ld (6),a
;.ptr = .ptr + 0x1003
ld a,(2)
ld b,3
add a,b
ld (6),a
;res = testb
ld a,(2)
ld (4),a
;res = 0x1234
ld a,52
ld (4),a
;res = .ptr
ld a,(2)
ld (4),a
;res = .ptr + testb
ld a,(2)
ld b,(2)
add a,b
ld (4),a
;.ptr = .ptr + 5
ld a,(2)
ld b,5
add a,b
ld (4),a
;res = 0x211
ld a,17
ld (4),a
;res = testb +22
ld a,(2)
ld b,2
add a,b
ld (4),a
;res +=1
ld a,(4)
ld b,1
add a,b
ld (4),a
;res = testb >>8
ld a,(2)
ld b,8
sra a,b
ld (4),a
;res = testb <<8
ld a,(2)
ld b,8
sla a,b
ld (4),a
;if test &  3
cp 
jp .label_0
;else
bra .label_1
.label_0:
.label_1:
.label_b2:
.label_2:
;while 1 == 1
cp 
jp .label_b2
..end
.org  $800
