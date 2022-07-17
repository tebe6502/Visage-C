;-
;- przerwania DLI dla HIP'a i TIP'a
;- calosc musi miescic sie w obrebie jednej strony pamieci

 opt h+
 org $0600
	
ant1	equ $5800
ant2	equ $5c00

 jmp hip
 jmp tip

;--------
;- T I P
;--------
tip
 jsr wai
 mwa #ant1 560

 lda #$40
 sta tip_+1
 
 lda <dli_tip
 ldx >dli_tip

quit
 sta $200
 stx $201
 lda #$c0
 sta $d40e
 
loop ldy 764
 iny
 beq loop
 
*---
wai lda 20
 cmp 20
 beq *-2
 rts
*---

dli_tip	sta ra+1	;przerwanie dli
	stx rx+1
	sty ry+1

	ldy #119
tip_	lda #$40
	ldx #$c0

	sta $d40a
	stx $d01b

	sta $d40a
	sta $d01b

	eor #$c0
	sta tip_+1

	dey
	bne tip_

	lda $231
	eor >ant1^ant2
	sta $231
	
ra	lda #0
rx	ldx #0
ry	ldy #0
	rti
	
;--------
;- H I P
;--------
hip
 jsr wai
 mwa #ant2 560
 
 lda 712
 sta tlo+1
 sta tlo_+1
 
 lda #0
 sta 712
 
 lda <dli
 ldx >dli
 jmp quit
 

dli sta ra_+1
 stx rx_+1

 ldx #100
_d0 lda #$40
 sta $d40a
 sta $d01b
 lda #0
 sta $d01a
 lda #$80
 sta $d40a
 sta $d01b
tlo lda #$00
 sta $d01a
 dex
 bpl _d0
 
 lda $231
 eor >ant1^ant2
 sta $231

 lda <dli2
 sta $200

ra_ lda #0
rx_ ldx #0
 rti


dli2 sta ra__+1
 sta rx__+1

 ldx #100
_d1 lda #$80
 sta $d40a
 sta $d01b
tlo_ lda #$00
 sta $d01a
 lda #$40
 sta $d40a
 sta $d01b
 lda #0
 sta $d01a
 dex
 bpl _d1

 lda $231
 eor >ant1^ant2
 sta $231

 lda <dli
 sta $200 

ra__ lda #0
rx__ ldx #0
 rti
