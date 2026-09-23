section .multiboot
align 4
	dd      0x1BADB002
	dd      0
	dd      -(0x1BADB002 + 0)

section .text
global _start
bits 32
_start:
	mov     esp, stack_top
	mov     edi, 0xB8000
	mov     esi, message
	;call    print_string

	cli

	lgdt    [gdt_descriptor]

	mov     ax, 0x10
	mov     ds, ax
	mov     es, ax
	mov     fs, ax
	mov     gs, ax
	mov     ss, ax

	jmp     0x08:protected_mode_entry

protected_mode_entry:
	call    check_long_mode

	cmp     eax, 0
	je      .unsupported

	mov     esi, message3
	;call    print_string
	call    setup_page_tables
        ; Load the physical address of the PML4 into CR3.
	mov     eax, pml4
	mov     cr3, eax

    ; Enable Physical Address Extension (PAE).
	mov     eax, cr4
	or      eax, 1 << 5
	mov     cr4, eax

    ; Enable Long Mode in the EFER MSR.
	mov     ecx, 0xC0000080
	rdmsr
	or      eax, 1 << 8
	wrmsr

    ; Enable paging.
	mov     eax, cr0
	or      eax, 1 << 31
	mov     cr0, eax

    ; Jump to the 64-bit code segment.
	jmp     0x18:long_mode_entry
	jmp     .done

.unsupported:
	mov     esi, message4
	call    print_string

.done:
	cli
	hlt

check_long_mode:
    ; Check whether CPUID is available.
	pushfd
	pop     eax

	mov     ecx, eax
	xor     eax, 1 << 21

	push    eax
	popfd

	pushfd
	pop     eax

	xor     eax, ecx
	test    eax, 1 << 21
	jz      .unsupported

    ; Check the highest extended CPUID leaf.
	mov     eax, 0x80000000
	cpuid

	cmp     eax, 0x80000001
	jb      .unsupported

    ; Check the Long Mode bit.
	mov     eax, 0x80000001
	cpuid

	test    edx, 1 << 29
	jz      .unsupported

	mov     eax, 1
	ret

.unsupported:
	xor     eax, eax
	ret

print_string:
.loop:
	lodsb
	cmp     al, 0
	je      .done

	mov     [edi], al
	mov     byte [edi + 1], 0x07
	add     edi, 2

	jmp     .loop

.done:
	ret

setup_page_tables:

    ; PML4[0] -> PDPT
	mov     eax, pdpt
	or      eax, 0b11
	mov     [pml4], eax

    ; PDPT[0] -> PD
	mov     eax, pd
	or      eax, 0b11
	mov     [pdpt], eax

    ; PD[0] -> first 2 MiB
    ; Present + writable + 2 MiB page
	mov     dword [pd], 0x00000083

    ; PD[1] -> second 2 MiB
    ; Present + writable + 2 MiB page
	mov     dword [pd + 8], 0x00200083

	ret

bits 64

long_mode_entry:
    ; Establish a known 64-bit stack.
    mov rsp, stack_top

    ; Clear the direction flag.
    cld

    ; Load the data segment selector.
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    extern kernel_main
    call kernel_main

    cli
    hlt

print_string_64:

.loop:
    lodsb
    test al, al
    jz .done

    mov [rdi], al
    mov byte [rdi + 1], 0x07

    add rdi, 2
    jmp .loop

.done:
    ret

section .rodata

message:
	db      "Hello from Sokoban OS! ", 0
message3:
	db      "Long mode supported!", 0

message4:
	db      "Long mode NOT supported!", 0
message_long_mode:
    db "We are now in 64-bit long mode and linking elf64!", 0

section .data

gdt_start:

gdt_null:
	dq      0

gdt_code:
	dw      0xffff
	dw      0
	db      0
	db      10011010b
	db      11001111b
	db      0

gdt_data:
	dw      0xffff
	dw      0
	db      0
	db      10010010b
	db      11001111b
	db      0

gdt_long_code:
	dw      0xffff
	dw      0
	db      0
	db      10011010b
	db      10100000b
	db      0

gdt_end:

gdt_descriptor:
	dw      gdt_end - gdt_start - 1
	dd      gdt_start

section .text

section .bss

align 16
stack_bottom:
	resb    16384
stack_top:

align 4096

pml4:
	resb    4096

pdpt:
	resb    4096

pd:
	resb    4096