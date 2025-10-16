data segment
    message db "Enter a string: $"
    buffer db 100 DUP (?) 
data ends

code segment
start:  
    mov ax, seg data         
    mov ds, ax             
    
    mov dx, offset message  
    mov ah, 09h
    int 21h
    
    xor dx, dx
    mov dx, offset buffer
    mov ah, 0Ah            
    int 21h                 
    
    xor dx, dx
    mov dx, offset buffer
    mov ah, 09h             
    int 21h                           

    mov ax, 4C00h           
    int 21h            
code ends
    end start



