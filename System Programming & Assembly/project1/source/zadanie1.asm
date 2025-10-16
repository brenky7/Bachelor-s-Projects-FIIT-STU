


zas segment stack
    dw 64 dup(?)
zas ends

data segment
    message db "No args$"
    message2 db "File error$"
    helpMessage db "Help message: This program currently checks for the -h parameter.$"
    char_buffer db 1 DUP (?), '$' 
    ;str_buffer db 65536 DUP ('$'), '$'
    filename db "example.txt$",0
    filehandle dw ?
    counter dw 0
data ends


code segment
assume CS:code, DS:data, SS:zas
_p: 
    mov bx,80h                  ; 80h is the offset of the command line arguments
    mov cl,[bx]                 ; get the length of the command line into cl
    test cl,cl                  ; check if the length is 0
    jz no_args                  ; if it is, jump to no_args
    add cx,bx                   ; store the address of the last character of the command line to cx
    inc bx                      ; skip the first character since its a space
read_args:
    xor dx,dx                   ; clear dx
    inc bx                      ; move to the next character
    cmp bx,cx                   ; check if we reached the end of the command line
    ja load_txt                 ; if we did, jump to no_more_args
    mov dl,[bx]                 ; move the character to dl
    cmp dl,'-'                  ; check if the character is a dash
    je check_arg_h              ; if it is, jump to check_arg_h
    ;mov ah,2                    ; if it isn't, print the character
    ;int 21h                     ; using the DOS interrupt
    jmp read_args               ; loop back to read_args
check_arg_h:                        
    inc bx                      ; move to the next character
    mov dl,[bx]                 ; move the character to dl
    cmp dl,'h'                  ; check if the character is an h
    jne read_args               ; if it isn't, jump to read_args
    mov ax,seg helpMessage      ; if it is, load the address of helpMessage into ax
    mov ds,ax                   ; and move it to ds
    mov dx,offset helpMessage   ; load the offset of helpMessage into dx
    mov ah,9                    ; print the message
    int 21h                     ; using the DOS interrupt
    xor ax,ax
    mov ds,ax
    xor dx,dx
    jmp load_txt            ; jump to no_more_args
no_args:
    mov ax,seg data
    mov ds,ax
    mov dx,0
    xor ax,ax
    mov ah,9
    int 21h
    jmp load_txt
load_txt:
    clc
    mov ax,seg filename
    mov ds,ax
    mov bx,ax
    mov ah, 3Dh
    mov al, 0
    mov dx,offset filename
    int 21h   
    mov filehandle, ax  
    jc file_error
    jmp read_file_loop
file_error:
    mov ax, seg message2
    mov ds, ax
    mov dx, offset message2
    mov ah, 9
    int 21h
    jmp exit
read_file_loop:
    mov ah, 3fh              ; DOS function for read file
    mov bx, filehandle               ; Load file handle
    mov cx, 1                ; Read one byte at a time
    mov dx, offset char_buffer           ; Load buffer address
    int 21h
    jc file_error
    mov bx, counter     
    inc bx                              ; Call DOS
    mov counter, bx
    test ax, ax              ; Check if EOF (ax = 0)
    jz zisti_dlzku            ; If ax = 0, close the file
    mov ah, 02h                ; DOS function for print character
    mov dl, byte ptr [char_buffer]       ; Load byte from buffer
    int 21h                  ; Call DOS
    jmp read_file_loop       ; Continue reading file
zisti_dlzku:
    xor cx,cx
    mov ax,counter
slucka:
    mov bx,10d
    xor dx,dx
    div bx
    push dx
    inc cx
    cmp ax, 0
    jz vypis_dlzku
    jmp slucka
vypis_dlzku:
    pop dx
    add dx, '0'
    mov ah, 02h
    int 21h
    dec cx
    cmp cx, 0
    jz exit
    jmp vypis_dlzku
close_file:
    mov ah, 3Eh              ; DOS function for close file
    int 21h  
exit:
    mov ax, 4C00h
    int 21h
code ends
    end _p