;[]-----------------------------------------------------------------[]
;|      H_PSBP.ASM -- long pointer routines                          |
;[]-----------------------------------------------------------------[]

;
;       C/C++ Run Time Library - Version 5.0
; 
;       Copyright (c) 1987, 1992 by Borland International
;       All Rights Reserved.
; 

                INCLUDE RULES.ASI

; calls to these routines are generated by the compiler to perform
; arithmetic operations on long pointers.

_TEXT           SEGMENT BYTE PUBLIC 'CODE'
                ASSUME  CS:_TEXT
;
;       ax:dx   left hand pointer
;       bx:cx   right hand pointer
;
;               To subtract, first convert both pointers to longs.
;               then do a simple signed long subtraction.  Actually
;               we only store 24 bit ints until the subtraction is
;               done.
;
                public  PSBP@
                public  F_PSBP@
                public  N_PSBP@

N_PSBP@:
                pop     es              ;fix up far return
                push    cs
                push    es
PSBP@:
F_PSBP@:
                push    di
                mov     di,cx
                mov     ch,dh
                mov     cl,4
                shl     dx,cl
                shr     ch,cl           ; dx:ch has the left hand long
                add     dx,ax
                adc     ch,0
                mov     ax,di
                shl     di,cl
                shr     ah,cl
                add     bx,di
                adc     ah,0            ; bx:ah has the right hand long
                sub     dx,bx
                sbb     ch,ah
                mov     al,ch
                cbw                     ; sign extend the upper part
                xchg    ax,dx
                pop     di
                retf

_TEXT           ENDS
                END

