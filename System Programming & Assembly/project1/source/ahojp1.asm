     ;*** POUZITIE PROCEDURY ***

;nazov procedury je p
;procedura je definovana na zaciatku kodoveho segmentu
;nazov procedury je umiesteny pred proc aj pred endp

DATA    SEGMENT                 ;zaciatok datoveho segmentu
TEXT    DB      'AHOJ$'         ;definicia retazca
DATA    ENDS                    ;koniec datoveho segmentu

CODE    SEGMENT                 ;zaciatok kodoveho segmentu
        ASSUME  CS:CODE, DS:DATA        ;direktiva oznamuje
 p	proc 
	mov ah,9
	mov dx,offset text
	int 21h
	ret
p	endp

start:  MOV AX, SEG DATA        ;do AX vloz adresu segmentu DATA
        MOV DS, AX              ;segmentovu adresu presun do DS
	call p
        MOV Ax, 4C00H           ;funkcia ukoncenie programu
        INT 21H                 ;prerusenie na vykonanie ukoncenia programu
code	ends	
	end start
