
; =====================================================
; AERODESK - COA CO4 PRACTICAL
; NASM 32-bit x86 Assembly for Windows
; Assembler : NASM
; Compiler  : MinGW GCC 32-bit
;
; Topics:
; 1. Control Signal Generation
; 2. Hardwired Control Unit
; 3. Microprogrammed Control Unit
; 4. Control Unit Organization
; 5. Instruction Pipelining
; =====================================================

BITS 32

extern _printf
global _main

section .data

    title db 10
          db "========================================", 10
          db "      AERODESK - COA CO4 MODULE", 10
          db "========================================", 10, 0

    h1 db 10, "1. CONTROL SIGNAL GENERATION", 10
       db "----------------------------------------", 10, 0

    instruction db "Instruction: ADD EAX, EBX", 10, 0

    h2 db 10, "2. HARDWIRED CONTROL UNIT", 10, 0
    h3 db 10, "3. MICROPROGRAMMED CONTROL UNIT", 10, 0
    h4 db 10, "4. CONTROL UNIT ORGANIZATION", 10, 0
    h5 db 10, "5. INSTRUCTION PIPELINING", 10, 0

    s1 db "IR_Load        = 1", 10, 0
    s2 db "ALU_ADD        = 1", 10, 0
    s3 db "Register_Write = 1", 10, 0
    s4 db "Memory_Read    = 0", 10, 0
    s5 db "PC_Increment   = 1", 10, 0

    m1 db "T0: MAR <- EIP; initiate instruction fetch", 10, 0
    m2 db "T1: IR <- fetched instruction", 10, 0
    m3 db "T2: Decode instruction opcode", 10, 0
    m4 db "T3: EAX <- EAX + EBX", 10, 0

    o1 db "Instruction Register (IR)", 10, 0
    o2 db "Opcode Decoder", 10, 0
    o3 db "Control Logic / Control Memory", 10, 0
    o4 db "Control Signals -> ALU, Registers, Memory", 10, 0

    p1 db "IF  = Instruction Fetch", 10, 0
    p2 db "ID  = Instruction Decode", 10, 0
    p3 db "EX  = Execute", 10, 0
    p4 db "MEM = Memory Access", 10, 0
    p5 db "WB  = Write Back", 10, 0

    t1 db 10, "PIPELINE SCHEDULE", 10
       db "----------------------------------------", 10, 0

    t2 db "Cycle 1: I1-IF", 10, 0
    t3 db "Cycle 2: I1-ID | I2-IF", 10, 0
    t4 db "Cycle 3: I1-EX | I2-ID | I3-IF", 10, 0
    t5 db "Cycle 4: I1-MEM | I2-EX | I3-ID", 10, 0
    t6 db "Cycle 5: I1-WB | I2-MEM | I3-EX", 10, 0

    note db 10, "NOTE: Educational control/pipeline simulation.", 10, 0
    done db 10, "COA CO4 DEMONSTRATION COMPLETED!", 10, 0

section .text

_main:

    ; Preserve Windows x86 non-volatile registers
    push ebx
    push esi
    push edi

    ; ------------------------------
    ; MAIN TITLE
    ; ------------------------------

    push dword title
    call _printf
    add esp, 4

    ; ------------------------------
    ; 1. CONTROL SIGNAL GENERATION
    ; ------------------------------

    push dword h1
    call _printf
    add esp, 4

    push dword instruction
    call _printf
    add esp, 4

    ; ------------------------------
    ; 2. HARDWIRED CONTROL UNIT
    ; ------------------------------

    push dword h2
    call _printf
    add esp, 4

    push dword s1
    call _printf
    add esp, 4

    push dword s2
    call _printf
    add esp, 4

    push dword s3
    call _printf
    add esp, 4

    push dword s4
    call _printf
    add esp, 4

    push dword s5
    call _printf
    add esp, 4

    ; ------------------------------
    ; 3. MICROPROGRAMMED CONTROL UNIT
    ; ------------------------------

    push dword h3
    call _printf
    add esp, 4

    push dword m1
    call _printf
    add esp, 4

    push dword m2
    call _printf
    add esp, 4

    push dword m3
    call _printf
    add esp, 4

    push dword m4
    call _printf
    add esp, 4

    ; ------------------------------
    ; 4. CONTROL UNIT ORGANIZATION
    ; ------------------------------

    push dword h4
    call _printf
    add esp, 4

    push dword o1
    call _printf
    add esp, 4

    push dword o2
    call _printf
    add esp, 4

    push dword o3
    call _printf
    add esp, 4

    push dword o4
    call _printf
    add esp, 4

    ; ------------------------------
    ; 5. INSTRUCTION PIPELINING
    ; ------------------------------

    push dword h5
    call _printf
    add esp, 4

    push dword p1
    call _printf
    add esp, 4

    push dword p2
    call _printf
    add esp, 4

    push dword p3
    call _printf
    add esp, 4

    push dword p4
    call _printf
    add esp, 4

    push dword p5
    call _printf
    add esp, 4

    ; ------------------------------
    ; PIPELINE SCHEDULE
    ; ------------------------------

    push dword t1
    call _printf
    add esp, 4

    push dword t2
    call _printf
    add esp, 4

    push dword t3
    call _printf
    add esp, 4

    push dword t4
    call _printf
    add esp, 4

    push dword t5
    call _printf
    add esp, 4

    push dword t6
    call _printf
    add esp, 4

    ; ------------------------------
    ; COMPLETION MESSAGE
    ; ------------------------------

    push dword note
    call _printf
    add esp, 4

    push dword done
    call _printf
    add esp, 4

    ; Return value = 0
    xor eax, eax

    ; Restore preserved registers
    pop edi
    pop esi
    pop ebx

    ; Return to the C runtime
    ret
