; AeroDesk COA - 80386 Assembly
; CO2: Flight Distance and Fuel Calculation

global _main
extern _printf

section .data

    msg db "================================", 10
        db "   AeroDesk Flight Calculation", 10
        db "================================", 10
        db "Flight No     : AD101", 10
        db "Distance      : %d km", 10
        db "Fuel Rate     : %d units/km", 10
        db "Fuel Required : %d units", 10
        db "================================", 10
        db 0

    distance dd 1200
    fuelRate dd 5

section .text

_main:

    ; Load flight data
    mov eax, [distance]
    mov ebx, [fuelRate]

    ; Fuel Required = Distance × Fuel Rate
    imul eax, ebx

    ; Save calculated result
    mov ecx, eax

    ; printf arguments
    push ecx
    push dword [fuelRate]
    push dword [distance]
    push msg
    call _printf

    ; Clean stack
    add esp, 16

    ; Return 0
    xor eax, eax
    ret