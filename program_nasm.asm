extern pow       
extern sys_table 
extern sys_print 
extern sys_getnum
extern sys_stop  

global main

section .data
    a dq 0
    b dq 0
    c dq 0
    D dq 0
    x1 dq 0
    x2 dq 0
    inf dq 0
    noroots dq 0
    islinear dq 0

section .text

isLinear:
    push rbp
    mov rbp, rsp
    push qword [rbp + 16] ; function argument par1
    push 0
    pop r8       
    pop r9       
    cmp r9, r8   
    sete al     
    movzx rax, al
    push rax     
    pop rax    
    cmp rax, 0 
    je .branch_fail_0

    ; ---- IF ----
    push qword [rbp + 24] ; function argument par2
    push 0
    pop r8       
    pop r9       
    cmp r9, r8   
    sete al     
    movzx rax, al
    push rax     
    pop rax    
    cmp rax, 0 
    je .branch_fail_1

    ; ---- IF ----
    push qword [rbp + 32] ; function argument par3
    push 0
    pop r8       
    pop r9       
    cmp r9, r8   
    sete al     
    movzx rax, al
    push rax     
    pop rax    
    cmp rax, 0 
    je .branch_fail_2
    push qword [inf] ; global variable
    pop rdi
    call sys_print
    jmp .branch_success_1
.branch_fail_2:

    ; ---- ELSE ----
    push qword [noroots] ; global variable
    pop rdi
    call sys_print
.branch_success_1:
    jmp .branch_success_0
.branch_fail_1:

    ; ---- ELSE ----
    push 0
    push 1
    pop r8       
    pop rax       
    sub rax, r8   
    push rax      
    push qword [rbp + 32] ; function argument par3
    pop r8       
    pop rax       
    imul rax, r8   
    push rax      
    push qword [rbp + 24] ; function argument par2
    pop r8       
    pop rax       
    cqo          
    idiv r8      
    push rax      
    pop rdi
    call sys_print
.branch_success_0:
    lea rdi, [islinear] ; global variable
    push rdi
    push 1
    pop  rax    
    pop  rdi    
    mov [rdi], rax
; ------------------------------------------------------
.branch_fail_0:
    mov rsp, rbp
    pop rbp
    ret

calculateDiscriminant:
    push rbp
    mov rbp, rsp
    lea rdi, [D] ; global variable
    push rdi
    push qword [rbp + 24] ; function argument par2
    push qword [rbp + 24] ; function argument par2
    pop r8       
    pop rax       
    imul rax, r8   
    push rax      
    push 4
    push qword [rbp + 16] ; function argument par1
    pop r8       
    pop rax       
    imul rax, r8   
    push rax      
    push qword [rbp + 32] ; function argument par3
    pop r8       
    pop rax       
    imul rax, r8   
    push rax      
    pop r8       
    pop rax       
    sub rax, r8   
    push rax      
    pop  rax    
    pop  rdi    
    mov [rdi], rax
    mov rsp, rbp
    pop rbp
    ret

calculateRoots:
    push rbp
    mov rbp, rsp

    ; ---- IF ----
    push qword [D] ; global variable
    push 0
    pop r8       
    pop r9       
    cmp r9, r8   
    setge al     
    movzx rax, al
    push rax     
    pop rax    
    cmp rax, 0 
    je .branch_fail_3
    lea rdi, [x1] ; global variable
    push rdi
    push 0
    push 1
    pop r8       
    pop rax       
    sub rax, r8   
    push rax      
    push qword [b] ; global variable
    pop r8       
    pop rax       
    imul rax, r8   
    push rax      
    push qword [D] ; global variable
    pop r8             
    cvtsi2sd xmm0, r8  
    sqrtsd xmm0, xmm0  
    cvttsd2si r8, xmm0 
    push r8            
    pop r8       
    pop rax       
    add rax, r8   
    push rax      
    push 2
    push qword [a] ; global variable
    pop r8       
    pop rax       
    imul rax, r8   
    push rax      
    pop r8       
    pop rax       
    cqo          
    idiv r8      
    push rax      
    pop  rax    
    pop  rdi    
    mov [rdi], rax
    lea rdi, [x2] ; global variable
    push rdi
    push 0
    push 1
    pop r8       
    pop rax       
    sub rax, r8   
    push rax      
    push qword [b] ; global variable
    pop r8       
    pop rax       
    imul rax, r8   
    push rax      
    push qword [D] ; global variable
    pop r8             
    cvtsi2sd xmm0, r8  
    sqrtsd xmm0, xmm0  
    cvttsd2si r8, xmm0 
    push r8            
    pop r8       
    pop rax       
    sub rax, r8   
    push rax      
    push 2
    push qword [a] ; global variable
    pop r8       
    pop rax       
    imul rax, r8   
    push rax      
    pop r8       
    pop rax       
    cqo          
    idiv r8      
    push rax      
    pop  rax    
    pop  rdi    
    mov [rdi], rax
; ------------------------------------------------------
    jmp .branch_success_2
.branch_fail_3:

    ; ---- ELSE ----
    lea rdi, [x1] ; global variable
    push rdi
    push -1
    pop  rax    
    pop  rdi    
    mov [rdi], rax
    lea rdi, [x2] ; global variable
    push rdi
    push -1
    pop  rax    
    pop  rdi    
    mov [rdi], rax
; ------------------------------------------------------
.branch_success_2:
    mov rsp, rbp
    pop rbp
    ret

printRoots:
    push rbp
    mov rbp, rsp
    push qword [rbp + 16] ; function argument root1
    pop rdi
    call sys_print
    push qword [rbp + 24] ; function argument root2
    pop rdi
    call sys_print
; ------------------------------------------------------
    mov rsp, rbp
    pop rbp
    ret



main:
    push rbp
    mov rbp, rsp

    lea rdi, [a] ; global variable
    push rdi
    push 0
    pop  rax    
    pop  rdi    
    mov [rdi], rax
    lea rdi, [b] ; global variable
    push rdi
    push 0
    pop  rax    
    pop  rdi    
    mov [rdi], rax
; ------------------------------------------------------
    lea rdi, [c] ; global variable
    push rdi
    push 0
    pop  rax    
    pop  rdi    
    mov [rdi], rax
; ------------------------------------------------------
    lea rdi, [D] ; global variable
    push rdi
    push 0
    pop  rax    
    pop  rdi    
    mov [rdi], rax
; ------------------------------------------------------
    lea rdi, [x1] ; global variable
    push rdi
    push 0
    pop  rax    
    pop  rdi    
    mov [rdi], rax
; ------------------------------------------------------
    lea rdi, [x2] ; global variable
    push rdi
    push 0
    pop  rax    
    pop  rdi    
    mov [rdi], rax
; ------------------------------------------------------
    lea rdi, [inf] ; global variable
    push rdi
    push 8
    pop  rax    
    pop  rdi    
    mov [rdi], rax
; ------------------------------------------------------
    lea rdi, [noroots] ; global variable
    push rdi
    push -1
    pop  rax    
    pop  rdi    
    mov [rdi], rax
; ------------------------------------------------------
    lea rdi, [a] ; global variable
    call sys_getnum
; ------------------------------------------------------
    lea rdi, [b] ; global variable
    call sys_getnum
; ------------------------------------------------------
    lea rdi, [c] ; global variable
    call sys_getnum
; ------------------------------------------------------
    lea rdi, [islinear] ; global variable
    push rdi
    push 0
    pop  rax    
    pop  rdi    
    mov [rdi], rax
; ------------------------------------------------------
; ------------------------------------------------------
; ------------------------------------------------------
; ------------------------------------------------------
; ------------------------------------------------------
    push qword [c] ; global variable
    push qword [b] ; global variable
    push qword [a] ; global variable
    call isLinear
    add rsp, 24       ; clear the stack of parameters after returning
; ------------------------------------------------------
    push qword [islinear] ; global variable
    push 0
    pop r8       
    pop r9       
    cmp r9, r8   
    sete al     
    movzx rax, al
    push rax     
    pop rax    
    cmp rax, 0 
    je .branch_fail_4
    push qword [c] ; global variable
    push qword [b] ; global variable
    push qword [a] ; global variable
    call calculateDiscriminant
    add rsp, 24       ; clear the stack of parameters after returning
    push qword [D] ; global variable
    push qword [b] ; global variable
    push qword [a] ; global variable
    call calculateRoots
    add rsp, 24       ; clear the stack of parameters after returning
; ------------------------------------------------------
    push qword [x2] ; global variable
    push qword [x1] ; global variable
    call printRoots
    add rsp, 16       ; clear the stack of parameters after returning
; ------------------------------------------------------
.branch_fail_4:
; ------------------------------------------------------
    mov rsp, rbp
    pop rbp
    call sys_stop  

section .note.GNU-stack noalloc noexec nowrite progbits
