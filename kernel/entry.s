; ==============================================================================
; entry.s - Giant OS Kernel Entry Point and 64-Bit Long Mode Transition
; ==============================================================================
;
; This assembly module serves as the primary entry point for Giant OS.
; It is loaded by a Multiboot-compliant bootloader (such as GRUB) in 32-bit
; protected mode with paging disabled.
;
; Key responsibilities of this file:
;   1. Multiboot Header: Validates kernel image with the bootloader.
;   2. 32-bit Initialization: Configures an initial stack and loads a GDT.
;   3. Hardware Capability Detection: Verifies CPUID and x86-64 Long Mode support.
;   4. Paging Setup: Constructs 4-level paging (PML4, PDPT, PD) with identity
;      mapping for the initial 4 MiB of physical address space using 2 MiB pages.
;   5. Long Mode Activation: Enables PAE, sets EFER.LME, and enables paging (CR0.PG).
;   6. 64-bit Entry: Performs a far jump into the 64-bit code segment, reloads
;      segment selectors, aligns the stack, and calls kernel_main().
; ==============================================================================

; ------------------------------------------------------------------------------
; Multiboot Specification Header (Multiboot 1)
; ------------------------------------------------------------------------------
; GRUB searches for this magic header in the first 8 KiB of the kernel binary.
; The header must be 4-byte (32-bit) aligned.
; ------------------------------------------------------------------------------
section .multiboot
align 4
	dd      0x1BADB002                  ; Magic number identifying a Multiboot header
	dd      0                           ; Feature flags (0 = no special requirements from bootloader)
	dd      -(0x1BADB002 + 0)           ; Checksum: (magic + flags + checksum == 0 mod 2^32)

; ------------------------------------------------------------------------------
; 32-Bit Protected Mode Text Section
; ------------------------------------------------------------------------------
section .text
global _start
bits 32

; ------------------------------------------------------------------------------
; _start: Kernel entry point invoked directly by the Multiboot bootloader.
; State upon entry:
;   - CPU is in 32-bit Protected Mode.
;   - Paging is DISABLED.
;   - Interrupts are not necessarily disabled (CLI must be called).
;   - EAX contains the Multiboot magic value (0x2BADB002).
;   - EBX contains the physical address of the Multiboot info structure.
; ------------------------------------------------------------------------------
_start:
	mov     esp, stack_top              ; Set up initial 32-bit stack pointer
	mov     edi, 0xB8000                ; Address of VGA text buffer (top-left character cell)
	mov     esi, message                ; Pointer to initial boot banner string
	;call    print_string

	cli                                 ; Disable hardware interrupts during CPU mode setup

	lgdt    [gdt_descriptor]            ; Load the temporary Global Descriptor Table (GDT)

	; Reload data segment registers with the 32-bit data selector (0x10)
	mov     ax, 0x10                    ; Selector 0x10 -> GDT entry 2 (gdt_data: index 2 * 8 bytes)
	mov     ds, ax                      ; Reload Data Segment (DS)
	mov     es, ax                      ; Reload Extra Segment (ES)
	mov     fs, ax                      ; Reload FS Segment
	mov     gs, ax                      ; Reload GS Segment
	mov     ss, ax                      ; Reload Stack Segment (SS)

	; Far jump to reload Code Segment (CS) register with 0x08 (gdt_code)
	; and flush the 32-bit processor instruction prefetch queue/pipeline
	jmp     0x08:protected_mode_entry

; ------------------------------------------------------------------------------
; protected_mode_entry: Post-GDT initialization in 32-bit protected mode.
; Prepares page tables and transitions the CPU into 64-bit Long Mode.
; ------------------------------------------------------------------------------
protected_mode_entry:
	call    check_long_mode             ; Test if processor supports x86-64 / Long Mode

	cmp     eax, 0                      ; Check return code (1 = supported, 0 = unsupported)
	je      .unsupported                ; If not supported, halt with error message

	mov     esi, message3               ; "Long mode supported!" message
	;call    print_string
	call    setup_page_tables           ; Construct initial identity page tables

        ; Load the physical address of the PML4 into CR3.
	mov     eax, pml4                   ; Load base address of PML4 (Page Map Level 4)
	mov     cr3, eax                    ; Store PML4 address in CR3 (Page Directory Base Register)

    ; Enable Physical Address Extension (PAE).
	mov     eax, cr4                    ; Read Control Register 4 (CR4)
	or      eax, 1 << 5                 ; Set bit 5 (PAE: Physical Address Extension), required for x86-64
	mov     cr4, eax                    ; Write updated value back to CR4

    ; Enable Long Mode in the EFER MSR.
	mov     ecx, 0xC0000080             ; IA32_EFER MSR (Extended Feature Enable Register) address
	rdmsr                               ; Read MSR into EDX:EAX
	or      eax, 1 << 8                 ; Set bit 8 (LME: Long Mode Enable)
	wrmsr                               ; Write updated value back to IA32_EFER MSR

    ; Enable paging.
	mov     eax, cr0                    ; Read Control Register 0 (CR0)
	or      eax, 1 << 31                ; Set bit 31 (PG: Paging Enable) -> activates IA-32e mode
	mov     cr0, eax                    ; Write updated value back to CR0

    ; Jump to the 64-bit code segment.
	jmp     0x18:long_mode_entry        ; Far jump to 64-bit code selector (0x18 = gdt_long_code),
	                                    ; transitioning from compatibility mode to full 64-bit mode
	jmp     .done                       ; Fallback safeguard (unreachable)

; Error handler: Hardware does not support 64-bit Long Mode
.unsupported:
	mov     esi, message4               ; "Long mode NOT supported!" message
	call    print_string                ; Print error message to VGA screen

; Terminal halt loop
.done:
	cli                                 ; Disable interrupts
	hlt                                 ; Halt processor until next reset

; ------------------------------------------------------------------------------
; check_long_mode: Checks if the CPU supports the x86-64 Long Mode instruction set.
; Output:
;   EAX = 1 if Long Mode is supported.
;   EAX = 0 if Long Mode is not supported.
; ------------------------------------------------------------------------------
check_long_mode:
    ; Check whether CPUID is available.
    ; CPUID is supported if the ID flag (bit 21) in EFLAGS can be flipped by software.
	pushfd                              ; Push EFLAGS to stack
	pop     eax                         ; Store original EFLAGS in EAX

	mov     ecx, eax                    ; Save a copy in ECX for later comparison
	xor     eax, 1 << 21                ; Flip bit 21 (ID flag)

	push    eax                         ; Push modified flags
	popfd                               ; Write modified value to EFLAGS

	pushfd                              ; Read back EFLAGS
	pop     eax                         ; Store updated EFLAGS in EAX

	xor     eax, ecx                    ; Compare updated EFLAGS with original copy
	test    eax, 1 << 21                ; Test whether bit 21 was successfully changed
	jz      .unsupported                ; If bit 21 did not flip, CPUID is not supported

    ; Check the highest extended CPUID leaf.
	mov     eax, 0x80000000             ; CPUID leaf 0x80000000: Query max extended leaf supported
	cpuid

	cmp     eax, 0x80000001             ; Must support at least leaf 0x80000001
	jb      .unsupported                ; If lower than 0x80000001, Long Mode bit cannot be queried

    ; Check the Long Mode bit.
	mov     eax, 0x80000001             ; CPUID leaf 0x80000001: Extended Processor Info and Feature Bits
	cpuid

	test    edx, 1 << 29                ; Bit 29 of EDX is the LM (Long Mode) bit
	jz      .unsupported                ; If 0, CPU does not support 64-bit Long Mode

	mov     eax, 1                      ; Return 1: Long Mode is supported
	ret

.unsupported:
	xor     eax, eax                    ; Return 0: Long Mode is unsupported
	ret

; ------------------------------------------------------------------------------
; print_string: Prints a null-terminated string to VGA text buffer in 32-bit mode.
; Inputs:
;   ESI: Pointer to null-terminated ASCII string.
;   EDI: Pointer to target location in VGA text buffer (0xB8000 + offset).
; Clobbers:
;   AL, ESI, EDI
; ------------------------------------------------------------------------------
print_string:
.loop:
	lodsb                               ; Load byte from [ESI] into AL, increment ESI
	cmp     al, 0                       ; Check for null terminator
	je      .done                       ; If null terminator reached, finish

	mov     [edi], al                   ; Store character byte at VGA buffer [EDI]
	mov     byte [edi + 1], 0x07        ; Store attribute byte: light grey (0x07) on black
	add     edi, 2                      ; Advance to next 2-byte VGA cell (char + attribute)

	jmp     .loop                       ; Process next character

.done:
	ret

; ------------------------------------------------------------------------------
; setup_page_tables: Sets up initial 4-level paging structures.
;
; Memory mapping configured:
;   Identity maps the first 4 MiB of physical memory:
;   - PML4[0] points to PDPT
;   - PDPT[0] points to PD
;   - PD[0] maps 0x00000000 - 0x001FFFFF (first 2 MiB) as a huge page
;   - PD[1] maps 0x00200000 - 0x003FFFFF (second 2 MiB) as a huge page
;
; Entry bit flags used:
;   Bit 0 (P)  = 1: Present
;   Bit 1 (RW) = 1: Read/Write allowed
;   Bit 7 (PS) = 1: Page Size (in PD: 2 MiB huge page instead of pointing to PT)
; ------------------------------------------------------------------------------
setup_page_tables:

    ; PML4[0] -> PDPT
	mov     eax, pdpt                   ; Base physical address of PDPT
	or      eax, 0b11                   ; Flags: Present (bit 0) | Writable (bit 1)
	mov     [pml4], eax                 ; Store entry into PML4 index 0

    ; PDPT[0] -> PD
	mov     eax, pd                     ; Base physical address of Page Directory (PD)
	or      eax, 0b11                   ; Flags: Present (bit 0) | Writable (bit 1)
	mov     [pdpt], eax                 ; Store entry into PDPT index 0

    ; PD[0] -> first 2 MiB
    ; Present + writable + 2 MiB page
	mov     dword [pd], 0x00000083      ; 0x83 = Present (0x01) | Writable (0x02) | Huge Page (0x80)
	                                    ; Base address = 0x00000000 (maps 0x00000000 - 0x001FFFFF)

    ; PD[1] -> second 2 MiB
    ; Present + writable + 2 MiB page
	mov     dword [pd + 8], 0x00200083  ; Entry index 1 is at offset +8 bytes
	                                    ; Base address = 0x00200000 (maps 0x00200000 - 0x003FFFFF)

	ret

; ------------------------------------------------------------------------------
; 64-Bit Long Mode Code Section
; ------------------------------------------------------------------------------
bits 64

; ------------------------------------------------------------------------------
; long_mode_entry: Entry point executed once processor enters 64-bit Long Mode.
; ------------------------------------------------------------------------------
long_mode_entry:
    ; Establish a known 64-bit stack.
    mov rsp, stack_top                  ; Initialize 64-bit stack pointer (RSP)

    ; Clear the direction flag.
    cld                                 ; DF = 0: string operations increment index registers

    ; Load the data segment selector.
    ; In 64-bit mode, segmentation is mostly disabled (base=0, limit ignored),
    ; but segment registers DS, ES, SS should contain a valid data descriptor.
    mov ax, 0x10                        ; Selector 0x10 (gdt_data)
    mov ds, ax                          ; Set Data Segment
    mov es, ax                          ; Set Extra Segment
    mov ss, ax                          ; Set Stack Segment

    extern kernel_main                  ; Declare C kernel entry function
    call kernel_main                    ; Call kernel_main() in kernel/kernel.c

    ; Safety catch: halt CPU if kernel_main() ever returns
    cli                                 ; Disable interrupts
    hlt                                 ; Halt processor

; ------------------------------------------------------------------------------
; print_string_64: Prints a null-terminated string to VGA buffer in 64-bit mode.
; Inputs:
;   RSI: Pointer to null-terminated ASCII string.
;   RDI: Pointer to target location in VGA text buffer (e.g. 0xB8000).
; Clobbers:
;   AL, RSI, RDI
; ------------------------------------------------------------------------------
print_string_64:

.loop:
    lodsb                               ; Load byte from [RSI] into AL, increment RSI
    test al, al                         ; Test for null terminator (\0)
    jz .done                            ; Finish if end of string

    mov [rdi], al                       ; Write ASCII character to VGA buffer
    mov byte [rdi + 1], 0x07            ; Write attribute byte (light grey on black)

    add rdi, 2                          ; Advance to next VGA character cell (2 bytes)
    jmp .loop                           ; Process next character

.done:
    ret

; ------------------------------------------------------------------------------
; Read-Only Data Section (.rodata)
; ------------------------------------------------------------------------------
section .rodata

message:
	db      "Hello from Sokoban OS! ", 0
message3:
	db      "Long mode supported!", 0

message4:
	db      "Long mode NOT supported!", 0
message_long_mode:
    db "We are now in 64-bit long mode and linking elf64!", 0

; ------------------------------------------------------------------------------
; Data Section (.data): Global Descriptor Table (GDT)
; ------------------------------------------------------------------------------
section .data

; Base label for GDT descriptor calculations
gdt_start:

; ------------------------------------------------------------------------------
; Selector 0x00: Mandatory Null Descriptor (64 bits of 0)
; ------------------------------------------------------------------------------
gdt_null:
	dq      0

; ------------------------------------------------------------------------------
; Selector 0x08: 32-Bit Protected Mode Code Segment Descriptor (Ring 0)
; Base = 0x00000000, Limit = 0xFFFFF (with 4 KiB granularity = 4 GiB)
; Access byte (10011010b):
;   Bit 7: Present (1)
;   Bits 6-5: DPL Ring 0 (00)
;   Bit 4: Descriptor type = code/data (1)
;   Bit 3: Executable = code segment (1)
;   Bit 2: Conforming = non-conforming (0)
;   Bit 1: Readable = read/execute (1)
;   Bit 0: Accessed = 0
; Flags (11001111b):
;   Bit 7: Granularity = 4 KiB pages (1)
;   Bit 6: Size = 32-bit protected mode (1)
;   Bit 5: Long mode = 32-bit (0)
;   Bit 4: AVL = 0
;   Bits 3-0: Segment limit [19:16] = 0xF
; ------------------------------------------------------------------------------
gdt_code:
	dw      0xffff                      ; Segment limit [15:0]
	dw      0                           ; Base address [15:0]
	db      0                           ; Base address [23:16]
	db      10011010b                   ; Access byte (Present, Ring 0, Code, Readable)
	db      11001111b                   ; Flags (4 KiB granularity, 32-bit) + Limit [19:16]
	db      0                           ; Base address [31:24]

; ------------------------------------------------------------------------------
; Selector 0x10: 32-Bit / Flat Data Segment Descriptor (Ring 0)
; Base = 0x00000000, Limit = 0xFFFFF (with 4 KiB granularity = 4 GiB)
; Access byte (10010010b):
;   Bit 7: Present (1)
;   Bits 6-5: DPL Ring 0 (00)
;   Bit 4: Descriptor type = code/data (1)
;   Bit 3: Executable = data segment (0)
;   Bit 2: Direction = expand up (0)
;   Bit 1: Writable = read/write (1)
;   Bit 0: Accessed = 0
; Flags (11001111b):
;   Bit 7: Granularity = 4 KiB pages (1)
;   Bit 6: Size = 32-bit (1)
;   Bit 5: Long mode = 0
;   Bit 4: AVL = 0
;   Bits 3-0: Segment limit [19:16] = 0xF
; ------------------------------------------------------------------------------
gdt_data:
	dw      0xffff                      ; Segment limit [15:0]
	dw      0                           ; Base address [15:0]
	db      0                           ; Base address [23:16]
	db      10010010b                   ; Access byte (Present, Ring 0, Data, Writable)
	db      11001111b                   ; Flags (4 KiB granularity, 32-bit) + Limit [19:16]
	db      0                           ; Base address [31:24]

; ------------------------------------------------------------------------------
; Selector 0x18: 64-Bit Long Mode Code Segment Descriptor (Ring 0)
; Access byte (10011010b):
;   Bit 7: Present (1)
;   Bits 6-5: DPL Ring 0 (00)
;   Bit 4: Descriptor type = code/data (1)
;   Bit 3: Executable = code segment (1)
;   Bit 2: Conforming = non-conforming (0)
;   Bit 1: Readable = read/execute (1)
;   Bit 0: Accessed = 0
; Flags (10100000b):
;   Bit 7: Granularity (1)
;   Bit 6: Size = D/B bit must be 0 in 64-bit mode (0)
;   Bit 5: Long mode code segment flag (L = 1) -> specifies 64-bit code segment
;   Bit 4: AVL = 0
;   Bits 3-0: Limit [19:16] = 0x0
; ------------------------------------------------------------------------------
gdt_long_code:
	dw      0xffff                      ; Segment limit [15:0] (ignored in 64-bit mode)
	dw      0                           ; Base address [15:0] (ignored in 64-bit mode)
	db      0                           ; Base address [23:16] (ignored in 64-bit mode)
	db      10011010b                   ; Access byte (Present, Ring 0, Code, Readable)
	db      10100000b                   ; Flags: L=1 (64-bit code), D=0 + Limit [19:16]
	db      0                           ; Base address [31:24] (ignored in 64-bit mode)

gdt_end:

; ------------------------------------------------------------------------------
; GDT Descriptor: Structure loaded into GDTR register via LGDT instruction
; ------------------------------------------------------------------------------
gdt_descriptor:
	dw      gdt_end - gdt_start - 1     ; Size of GDT in bytes minus 1 (limit)
	dd      gdt_start                   ; 32-bit linear base address of GDT

section .text

; ------------------------------------------------------------------------------
; BSS Section: Uninitialized Data (Stack and Early Page Tables)
; ------------------------------------------------------------------------------
section .bss

; Initial kernel stack (16 KiB, 16-byte aligned per System V ABI)
align 16
stack_bottom:
	resb    16384                       ; Reserve 16 KiB for the stack
stack_top:

; Page table structures (must be 4 KiB / 4096-byte aligned for hardware MMU)
align 4096

; Page Map Level 4 Table (PML4): Root of the 4-level paging hierarchy
pml4:
	resb    4096                        ; Reserve 4096 bytes (512 entries x 8 bytes)

; Page Directory Pointer Table (PDPT): Level 3 paging table
pdpt:
	resb    4096                        ; Reserve 4096 bytes (512 entries x 8 bytes)

; Page Directory (PD): Level 2 paging table (configured with 2 MiB large pages)
pd:
	resb    4096                        ; Reserve 4096 bytes (512 entries x 8 bytes)
