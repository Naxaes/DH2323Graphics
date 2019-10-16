; https://www.youtube.com/watch?v=BWRR3Hecjao

; varible    == address
; [variable] == content

%define SYSCALL_EXIT  0x2000001
%define SYSCALL_READ  0x2000003
%define SYSCALL_WRITE 0x2000004

%define FILEDESC_STDIN         0
%define FILEDESC_STDOUT        1

%macro print 2
    mov     rax, SYSCALL_WRITE
    mov     rdi, STDOUT
    mov     rsi, %1
    mov     rdx, %2
    syscall
%endmacro

%macro input 2
    mov     rax, SYSCALL_READ
    mov     rdi, 0
    mov     rsi, %1
    mov     rdx, %2
    syscall
%endmacro


section .data
    message:        db "What's your name? ", 10, 0
        .length     equ $ - message
    true:           db "It's true!", 10, 0
        .length     equ $ - true
    false:          db "It's false!", 10, 0
        .length     equ $ - false
    ted:            db "ted", 10, 0
        .length     equ $ - ted

section .bss
    answer:         resb ted.length
        .length     equ $ - answer

section .text
    global start

start:
    
    mov     rax, message
    call    print_rax
    call    get_input
    mov     rax, answer
    call    print_rax

    mov     rax, ted
    mov     rbx, answer
    call    compare_rax_rbx_bytewise

    cmp     dl, 1
    jne     print_false
    je      print_true

    jmp      exit


get_input:
    mov     rax, SYSCALL_READ
    mov     rdi, FILEDESC_STDIN
    mov     rsi, answer
    mov     rdx, answer.length
    syscall
    ret

print_true:
    mov     rax, SYSCALL_WRITE
    mov     rdi, FILEDESC_STDOUT
    mov     rsi, true
    mov     rdx, true.length
    syscall
    jmp     exit

print_false:
    mov     rax, SYSCALL_WRITE
    mov     rdi, FILEDESC_STDOUT
    mov     rsi, false
    mov     rdx, false.length
    syscall
    jmp     exit


print_rax:
    push    rax
    mov     rbx, 0

    .loop:
        inc rbx
        inc rax
        mov cl, [rax]
        cmp cl, 0
        jne .loop

    mov rax, SYSCALL_WRITE
    mov rdi, FILEDESC_STDOUT
    pop rsi
    mov rdx, rbx
    syscall

    ret

; Compares each byte of rax with rbx
; If true dl == 1, else dl == 0
compare_rax_rbx_bytewise:
    mov dl,  1

    push rax
    push rbx

    .loop:
        mov cl, [rax]
        cmp cl, 0
        je  .exit

        cmp al, bl
        inc rax
        inc rbx
        je  .loop
        mov dl, 0

    .exit:
        pop rbx
        pop rax
        ret

exit:
    mov     rax, SYSCALL_EXIT
    mov     rdi, 0
    syscall