.code





read_cs PROC
    xor rax, rax
    mov ax, cs
    ret
read_cs ENDP

read_ss PROC
    xor rax, rax
    mov ax, ss
    ret
read_ss ENDP

read_ds PROC
    xor rax, rax
    mov ax, ds
    ret
read_ds ENDP

read_es PROC
    xor rax, rax
    mov ax, es
    ret
read_es ENDP

read_fs PROC
    xor rax, rax
    mov ax, fs
    ret
read_fs ENDP

read_gs PROC
    xor rax, rax
    mov ax, gs
    ret
read_gs ENDP

read_tr PROC
    xor rax, rax
    str ax
    ret
read_tr ENDP

read_ldtr PROC
    xor rax, rax
    sldt ax
    ret
read_ldtr ENDP

store_gdt PROC
    sgdt fword ptr [rcx]
    ret
store_gdt ENDP

store_idt PROC
    sidt fword ptr [rcx]
    ret
store_idt ENDP

END
