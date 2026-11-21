format MZ

entry code_seg:start

stack 256

segment data_seg
    msg db 'AeroDesk 8086 Module$'

segment code_seg
start:

    mov ax, data_seg
    mov ds, ax

    mov dx, msg
    mov ah, 9
    int 21h

    mov ax, 4c00h
    int 21h