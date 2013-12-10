;***************************************************************************
;* fsk_coher_dem: Coherent FSK demodulator implementation                  *
;*-------------------------------------------------------------------------*
;* license: GPL-3.0+ <http://www.gnu.org/licenses/gpl.html>
;*****************************************************************************/
;* This file is part of BALSAMO source package.
;*
;* BALSAMO is free software: you can redistribute
;* it and/or modify it under the terms of the GNU General Public
;* License as published by the Free Software Foundation, either
;* version 3 of the License, or (at your option) any later version.
;*
;* Some open source application is distributed in the hope that it will
;* be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
;* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;* GNU General Public License for more details.
;*
;* You should have received a copy of the GNU General Public License
;* along with BALSAMO.  If not, see <http://www.gnu.org/licenses/>.

; - Routine has been modified to only use X-data memory
;
; NOTES:
; - It looks like using MODULO ADDRESSING is not worth the effort, because
;   configuring it requires at least to write 3 registers and to initialize
;   the pointer (6 + 2 cycles). Copying 3 delays should take a similar
;   amount of cycles, maybe even less.
; - It still looks like interesting to accumulate N samples in the buffer
;   before processing them all, to lower overload caused by FRCT and
;   registers save/restore operations. It also avoids overrun conditions
;   and might allow to do processing outside interrupt context. Samples
;   accumulation can be coded in C because it doesn't require modulo
;   addressing.
; - This code could be merged with the IIR filter if needed, but maximum
;   length of DO loops must be checked to make sure.
; - Maybe it would be best implementing everything excepting filtering in
;   plain C.

	; Local inclusions
	.nolist
	.include	"dspcommon.inc"		; fractsetup
	.include	"p30f6014.inc"		; Register definitions
	.list

	; Constant definitions
	.equ	nx, 64		; Number of samples to process in a block
	.equ	nk, 3		; Number of delays for demodulation


	; Put code inside libdsp section
	.section .libdsp, code

;Function void FskCoherentDem(fractional x[], fractional y[])
;Implements a FSK coherent demodulation, by multiplying x[n] * x[n+k]
;Paso de parámetros:
;- W0: x pointer
;- W1: y pointer
;- W2: x+k pointer
;
;Register usage
;W3:  Loop counter
;W4:  Multiplicand 1:1
;W5:  Multiplicand 1:2
;W6:  Multiplicand 2:1
;W7:  Multiplicand 2:2
;W8:  x pointer
;W9:  x+nk pointer
;w13: y pointer
;(W0 and W2 are not used)

	.global	_FskCoherentDemod	; Export function
_FskCoherentDemod:

	;Enable fractional mode (macro fractsetup in dspcommon.inc)
	PUSH CORCON
	PUSH.D W8
	PUSH W13
	fractsetup	W4

	;Prepare input and output pointers to use prefetch and writeback
	MOV W0, W8			;x
	ADD W0, #(2*nk), W9	;x+nk (word)
	MOV W1, W13			;y

	;Prepare nx iterations loop
	MOV #(nx/2 - 1), W3
	;Read prefetch variables
	MOV [W8++], W4
	MOV [W9++], W5
	;Start loop
	DO	W3, fsk_loop
		;ACA=x[n]*x[n-k]; W6=x[n+1]; W7=x[n+k+1]
		MPY W4*W5, A, [W8]+=2, W6
		;y[n]=x[n]*x[n+k]
		MOVSAC B, [W9]+=2, W7, [W13]+=2	;WB de ACA
		;ACB=x[n+1]*x[n+k+1]; W4=x[n+2]; W5=x[n+k+2]
		MPY W6*W7, B, [W8]+=2, W4
fsk_loop:
		;y[n+1]=x[n+1]*x[n+k+1]
		MOVSAC A, [W9]+=2, W5, [W13]+=2	;WB de ACB

	;Restore CORCON.
	POP W13
	POP.D W8
	POP	CORCON
	RETURN

	.end
