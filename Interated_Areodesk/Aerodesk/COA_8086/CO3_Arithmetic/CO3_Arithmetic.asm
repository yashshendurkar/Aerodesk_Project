
; =====================================================
; AERODESK - COA CO3 ARITHMETIC MODULE
; 80386 Assembly Language - NASM 32-bit Windows
;
; Implemented Concepts:
; 1. Add-and-Shift Multiplication Algorithm
; 2. Flight Fuel Calculation
; 3. String Length
; 4. String Copy
; 5. String Concatenation
; 6. String Comparison
; 7. Character Search
; =====================================================

global _main
extern _printf

section .data

    title db 10
          db "========================================", 10
          db "       AERODESK COA CO3 MODULE", 10
          db "       80386 ARITHMETIC", 10
          db "========================================", 10, 0

    fuelTitle db 10, "----- FLIGHT FUEL CALCULATION -----", 10, 0

    fuelFormat db "Distance       : %u km", 10
               db "Fuel Rate      : %u units/km", 10
               db "Fuel Required  : %u units", 10
               db "Available Fuel : %u units", 10
               db "Remaining Fuel : %u units", 10, 0

    algorithmMsg db 10
                 db "Algorithm Used : Add-and-Shift Multiplication", 10
                 db "IMUL instruction is NOT used.", 10, 0

    stringTitle db 10, "----- STRING OPERATIONS -----", 10, 0

    flightLabel db "Flight Number : %s", 10, 0
    sourceLabel db "Source        : %s", 10, 0
    destLabel db "Destination   : %s", 10, 0
    lengthLabel db "String Length : %u", 10, 0
    copyLabel db "Copied String : %s", 10, 0
    concatLabel db "Concatenated Route : %s", 10, 0

    compareEqual db "String Comparison: Strings are EQUAL", 10, 0
    compareNotEqual db "String Comparison: Strings are NOT EQUAL", 10, 0

    searchFound db "Character Search: Character FOUND", 10, 0
    searchNotFound db "Character Search: Character NOT FOUND", 10, 0

    endMsg db 10, "========================================", 10
           db "       CO3 EXECUTION COMPLETED", 10
           db "========================================", 10, 0

    ; Flight fuel input values
    distance dd 1200
    fuelRate dd 5
    availableFuel dd 10000

    ; Flight information
    flightNo db "AI101", 0
    source db "PUNE", 0
    destination db "DELHI", 0

    ; Search character
    searchChar db '1'

section .bss

    fuelRequired resd 1
    remainingFuel resd 1

    stringLength resd 1
    compareResult resd 1
    searchResult resd 1

    copiedString resb 32
    routeString resb 64

section .text

_main:

    ; Preserve Windows callee-saved registers
    push ebx
    push esi
    push edi

    cld

    ; =========================================
    ; DISPLAY TITLE
    ; =========================================

    push title
    call _printf
    add esp, 4

    ; =========================================
    ; PART 1: ADD-AND-SHIFT MULTIPLICATION
    ;
    ; Fuel Required = Distance x Fuel Rate
    ;
    ; Distance = 1200
    ; Fuel Rate = 5
    ; Result = 6000
    ;
    ; No IMUL instruction is used.
    ; =========================================

    mov eax, [distance]       ; Multiplicand
    mov ebx, [fuelRate]       ; Multiplier
    xor ecx, ecx              ; Result = 0

multiply_loop:

    cmp ebx, 0
    je multiplication_done

    ; Check least significant bit
    test ebx, 1
    jz skip_addition

    ; Add multiplicand to result
    add ecx, eax

skip_addition:

    ; Shift multiplicand left
    shl eax, 1

    ; Shift multiplier right
    shr ebx, 1

    jmp multiply_loop

multiplication_done:

    ; Save calculated fuel
    mov [fuelRequired], ecx

    ; =========================================
    ; CALCULATE REMAINING FUEL
    ; =========================================

    mov eax, [availableFuel]
    sub eax, [fuelRequired]

    mov [remainingFuel], eax

    ; =========================================
    ; DISPLAY FUEL RESULTS
    ; =========================================

    push fuelTitle
    call _printf
    add esp, 4

    push dword [remainingFuel]
    push dword [availableFuel]
    push dword [fuelRequired]
    push dword [fuelRate]
    push dword [distance]
    push fuelFormat

    call _printf
    add esp, 24

    push algorithmMsg
    call _printf
    add esp, 4

    ; =========================================
    ; PART 2: STRING LENGTH
    ; Find length of flight number AI101
    ; =========================================

    mov esi, flightNo
    xor ecx, ecx

length_loop:

    lodsb

    cmp al, 0
    je length_done

    inc ecx
    jmp length_loop

length_done:

    mov [stringLength], ecx

    ; =========================================
    ; PART 3: STRING COPY
    ; Copy source string PUNE
    ; into copiedString
    ; =========================================

    mov esi, source
    mov edi, copiedString

copy_loop:

    lodsb
    stosb

    cmp al, 0
    jne copy_loop

    ; =========================================
    ; PART 4: STRING CONCATENATION
    ; Create PUNE-DELHI
    ; =========================================

    ; Copy source into routeString
    mov esi, source
    mov edi, routeString

concat_source:

    lodsb
    cmp al, 0
    je append_hyphen

    stosb
    jmp concat_source

append_hyphen:

    ; Add hyphen
    mov al, '-'
    stosb

    ; Append destination
    mov esi, destination

concat_destination:

    lodsb
    stosb

    cmp al, 0
    jne concat_destination

    ; =========================================
    ; PART 5: STRING COMPARISON
    ; Compare source with copiedString
    ; =========================================

    mov esi, source
    mov edi, copiedString

compare_loop:

    cmpsb
    jne strings_not_equal

    ; Check whether null terminator was compared
    cmp byte [esi-1], 0
    jne compare_loop

    mov dword [compareResult], 1
    jmp comparison_done

strings_not_equal:

    mov dword [compareResult], 0

comparison_done:

    ; =========================================
    ; PART 6: CHARACTER SEARCH
    ; Search character '1' in AI101
    ; =========================================

    mov edi, flightNo
    mov al, [searchChar]
    mov ecx, [stringLength]

    cld
    repne scasb

    jne character_not_found

    mov dword [searchResult], 1
    jmp search_done

character_not_found:

    mov dword [searchResult], 0

search_done:

    ; =========================================
    ; DISPLAY STRING RESULTS
    ; =========================================

    push stringTitle
    call _printf
    add esp, 4

    push flightNo
    push flightLabel
    call _printf
    add esp, 8

    push source
    push sourceLabel
    call _printf
    add esp, 8

    push destination
    push destLabel
    call _printf
    add esp, 8

    push dword [stringLength]
    push lengthLabel
    call _printf
    add esp, 8

    push copiedString
    push copyLabel
    call _printf
    add esp, 8

    push routeString
    push concatLabel
    call _printf
    add esp, 8

    ; Display comparison result
    cmp dword [compareResult], 1
    jne display_not_equal

    push compareEqual
    call _printf
    add esp, 4
    jmp display_search

display_not_equal:

    push compareNotEqual
    call _printf
    add esp, 4

display_search:

    ; Display character search result
    cmp dword [searchResult], 1
    jne display_not_found

    push searchFound
    call _printf
    add esp, 4
    jmp display_end

display_not_found:

    push searchNotFound
    call _printf
    add esp, 4

display_end:

    push endMsg
    call _printf
    add esp, 4

    ; =========================================
    ; RESTORE REGISTERS AND RETURN
    ; =========================================

    pop edi
    pop esi
    pop ebx

    xor eax, eax
    ret
