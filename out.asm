section .text
global _start
_start:
    mov rax, 1
    push rax
    mov rax, 8
    push rax
    mov rax, 9
    push rax
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    pop rbx
    add rax, rbx
    push rax
    push QWORD [rsp + 0]

    push QWORD [rsp + 0]

    push QWORD [rsp + 8]

    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
