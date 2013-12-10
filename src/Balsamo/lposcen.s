;****************************************************************************
;* lposcen.s: Enables low power external 32 kHz crystal oscillator. This    *
;* has to be done in assembly languaje to properly follow the OSCCONL write *
;* sequence.                                                                *
;*--------------------------------------------------------------------------*
;* Jesus Alonso Fernandez (doragasu)                                        *
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

	; Local includes
	.nolist
.include	"p30f6014.inc"		; Register definitions
	.list

	.text

	.global _LPOSCEnable	;Export LPOSCEnable symbol to C code.
_LPOSCEnable:
	mov.b #0x46,w1 ; follow write sequence
	mov.b #0x57,w2 ; for OSCCONL writes.
	mov #OSCCONL,w3
	mov.b w1,[w3]
	mov.b w2,[w3]
	bset OSCCONL,#LPOSCEN ;enable 32Khz external xtal
	RETURN
	.end
