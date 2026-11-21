; AeroDesk COA - 80386 Architecture Demonstration

global _main
extern _printf

section .data

    msg db "================================", 10
        db "   AeroDesk 80386 Architecture", 10
        db "================================", 10
        db "EAX = %d", 10
        db "EBX = %d", 10
        db "ECX = %d", 10
        db "EDX = %d", 10
        db "================================", 10
        db 0

section .text

_main:

    ; Load values into 80386 32-bit registers
    mov eax, 100
    mov ebx, 200
    mov ecx, 300
    mov edx, 400

    ; Display register values
    push edx
    push ecx
    push ebx
    push eax
    push msg
    call _printf

    ; Clean stack
    add esp, 20

    ; Return 0
    xor eax, eax
    ret