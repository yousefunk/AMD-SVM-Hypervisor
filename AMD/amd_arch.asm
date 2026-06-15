.code

EXTERN HandleVmexit:PROC


VCPU_VMCB           EQU 0
VCPU_VMCB_PA        EQU 8
VCPU_HOST_SAVE_PA   EQU 24
VCPU_HOST_STACK_TOP EQU 72

VMCB_STATE_RFLAGS EQU 570h
VMCB_STATE_RIP    EQU 578h
VMCB_STATE_RSP    EQU 5D8h





SvmLaunch PROC
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15

    mov rax, [rcx + VCPU_VMCB]

    lea rdx, GuestResume
    mov [rax + VMCB_STATE_RIP], rdx

    mov rdx, rsp
    mov [rax + VMCB_STATE_RSP], rdx

    pushfq
    pop rdx
    mov [rax + VMCB_STATE_RFLAGS], rdx

    mov rax, [rcx + VCPU_HOST_SAVE_PA]
    vmsave rax

    mov rsp, [rcx + VCPU_HOST_STACK_TOP]
    push rcx
    sub rsp, 8

RunLoop:
    mov rax, [rsp + 8]
    mov rax, [rax + VCPU_VMCB_PA]
    vmload rax
    vmrun  rax
    vmsave rax

    
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    
    
    
    
    
    
    
    
    
    
    sub rsp, 208h
    fxsave  [rsp]

    
    mov rax, [rsp + 288h]
    mov rax, [rax + VCPU_HOST_SAVE_PA]
    vmload rax

    mov rcx, [rsp + 288h]
    lea rdx, [rsp + 208h]       
    sub rsp, 28h
    call HandleVmexit
    add rsp, 28h

    
    fxrstor [rsp]
    add rsp, 208h

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    jmp RunLoop

GuestResume:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    xor eax, eax
    ret
SvmLaunch ENDP






HvVerify PROC
    mov rcx, 0C0FFEED00DBEEFh
    xor rax, rax
    vmmcall
    ret
HvVerify ENDP

END
