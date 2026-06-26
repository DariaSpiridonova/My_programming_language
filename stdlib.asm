; =================================================================
;                       STANDARD LIBRARY
; =================================================================

section .data
    newline        db 10   
    buffer_counter dq 0     
    pi             dq 3.14159265358979323846 

section .rodata
align 8
    global sys_table
    
    sys_table:
        dq sys_print    ; Индекс 0
        dq sys_getnum   ; Индекс 1
        dq sys_stop     ; Индекс 2


section     .bss
    buffer         resb 1024 
    num_buf        resb 32

section .text
    global sys_print
    global sys_getnum
    global sys_stop

; *****************************************************************
;                        PRINT function 
;         displays the value of a variable on the screen
; Expects:
;        RDI - the argument to display on the screen
; Returns: 
;        - 
; Spoils:
;        RAX, RBX, RCX, RDX, RDI, XMM0, XMM1, XMM2
; *****************************************************************

sys_print:
    push rbp
    mov rbp, rsp

    mov rax, rdi  
    test rax, rax           ; test a sign
    jns .positive           ; check SF

    call print_minus                

    neg rax  
    
.positive:
    mov rbx, 10         ; divider
    xor rcx, rcx        ; the number counter

divide_loop:
    xor rdx, rdx        ; 
    div rbx             ; EAX / 10 -> result in EAX, remainder in EDX
    
    add rdx, '0'        ; number to symbol 
    push rdx            ; symbol in stack
    inc rcx             
    
    test rax, rax      
    jnz divide_loop

output_loop:
    mov rdi, rsp       ; RDI = the address of the symbol 
    mov rdx, rcx
    call write_char_to_str_buffer    ; print digit 
    
    pop rax            ; go to the next digit
    loop output_loop   ; repeat RCX times

    lea rsi, [buffer]
    mov rdx, [buffer_counter]
    mov byte [buffer + rdx], 10 
    inc rdx                     
    mov [buffer_counter], rdx
    call print_buffer_content
    
    mov qword [buffer_counter], 0

    mov rsp, rbp
    pop rbp 

    ret

;***********************************************************************************************
; Function for displaying an integer number to the screen
;
; Expects:
;        RSI - pointer to the first buffer symbol
;        RDX - buffer length
; Returns: 
;        -
; Spoils:
;        RAX, RDI
;***********************************************************************************************

print_buffer_content:
    mov rax, 0x01      
    mov rdi, 1                 ; stdout
    syscall

    ret

;***********************************************************************************************
; Function for writing one byte to the string buffer
;
; Expects:
;        RDI - address of the symbol
; Returns: 
;        -
; Spoils:
;        RBX
;***********************************************************************************************

write_char_to_str_buffer:
    push rax
    push rdi
    
    mov rbx, [buffer_counter]   
    mov al, [rdi] 

    mov [buffer + rbx], al ; (-1)^S * 2^(E - 127) * 1.M
    inc rbx                     
    
    mov [buffer_counter], rbx
 
    pop rdi
    pop rax
    
    ret

;***********************************************************************************************
; Function for outputting a minus
;
; Expects:
;        -
; Returns: 
;        -
; Spoils:
;        -
;***********************************************************************************************

print_minus:
    push rax
    mov rax, '-'
    push rax
    mov rdi, rsp
    call write_char_to_str_buffer       
    pop rax
    pop rax
    ret



; *****************************************************************
;                        GETNUM function
;      writes the entered value to the variable passed to it
; Expects:
;        RDI - pointer to the target variable for writing value to it
; Returns: 
;        -
; Spoils:
;        RAX, RBX, RCX, RDX, RDI, RSI, R8
; *****************************************************************

sys_getnum:
    push rbp
    mov rbp, rsp

    push rdi

    ; syscall read
    mov rax, 0          ; sys_read
    mov rdi, 0          ; stdin
    mov rsi, num_buf    ; destination
    mov rdx, 32         ; byte number 
    syscall             

    xor rbx, rbx        ; for result
    xor rcx, rcx        ; counter
    mov r8, 1           ; set a sign (1 или -1)

    ; checking for minus
    mov al, [num_buf]
    cmp al, '-'
    jne .parse_loop
    mov r8, -1          ; negative number
    inc rcx             ; skip minus

.parse_loop:
    movzx rdx, byte [num_buf + rcx]
    cmp rdx, 10         ; checking for '\n'
    je .done
    cmp rdx, '0'
    jl .done            ; if not a digit — exit
    cmp rdx, '9'
    jg .done

    sub rdx, '0'        ; ASCII -> digit
    imul rbx, 10
    add rbx, rdx
    inc rcx
    jmp .parse_loop

.done:
    imul rbx, r8        ; set a sign
    
    pop rdi

    mov [rdi], rbx
    mov rsp, rbp
    pop rbp 

    ret

; *****************************************************************
;                        STOP function
;                    completes the program
; Expects:
;        RDI - pointer to the integer number to display on the screen
; Returns: 
;        -
; Spoils:
;        RAX, RBX, RCX, RDX, RDI, XMM0, XMM1, XMM2
; *****************************************************************

sys_stop:
    mov rax, 60         
    xor rdi, rdi        
    syscall

section .note.GNU-stack noalloc noexec nowrite progbits