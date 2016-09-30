#include <asm.h>

# Start the CPU: switch to 32-bit protected mode, jump into C.
# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0 %ip=7c00.

.set PROT_MODE_CSEG,        0x8                     # kernel code segment selector
.set PROT_MODE_DSEG,        0x10                    # kernel data segment selector
.set CR0_PE_ON,             0x1                     # protected mode enable flag
.set SMAP,                  0x534d4150

# start address should be 0:7c00, in real mode, the beginning address of the running bootloader
.globl start
start:
.code16                                             # Assemble for 16-bit mode
    cli                                             # Disable interrupts
    cld                                             # String operations increment

    # Set up the important data segment registers (DS, ES, SS).
    xorw %ax, %ax                                   # Segment number zero
    movw %ax, %ds                                   # -> Data Segment
    movw %ax, %es                                   # -> Extra Segment
    movw %ax, %ss                                   # -> Stack Segment

    # Enable A20:
    #  For backwards compatibility with the earliest PCs, physical
    #  address line 20 is tied low, so that addresses higher than
    #  1MB wrap around to zero by default. This code undoes this.
seta20.1:
    inb $0x64, %al                                  # Wait for not busy
    testb $0x2, %al
    jnz seta20.1

    movb $0xd1, %al                                 # 0xd1 -> port 0x64
    outb %al, $0x64

seta20.2:
    inb $0x64, %al                                  # Wait for not busy
    testb $0x2, %al
    jnz seta20.2

    movb $0xdf, %al                                 # 0xdf -> port 0x60
    outb %al, $0x60
	
# search memory
#对 0x8000 处的 32 位单元清零,即给位于 0x8000 处的
#struct e820map 的结构域 nr_map 清零
probe_memory:	
    movl $0, 0x8000
    xorl %ebx, %ebx
    movw $0x8004, %di	#表示设置调用 INT 15h BIOS 中断后,BIOS 返回的映射地址描述符的起始地址
start_probe:
    movl $0xE820, %eax		# INT 15 的中断调用参数
    movl $20, %ecx 	#设置地址范围描述符的大小为 20 字节,其大小等于 struct e820map 的结构域 map 的大小
    movl $SMAP, %edx	#设置 edx 为 534D4150h (即 4 个 ASCII 字符“SMAP”),这是一个约定
    int $0x15	#调用 int 0x15 中断,要求 BIOS 返回一个用地址范围描述符表示的内存段信息
    jnc cont	#如果 eflags 的 CF 位为 0,则表示还有内存段需要探测
    movw $12345, 0x8000	#探测有问题,结束探测
    jmp finish_probe
cont:
    addw $20, %di	#设置下一个 BIOS 返回的映射地址描述符的起始地址
    incl 0x8000	#递增 struct e820map 的结构域 nr_map
    cmpl $0, %ebx #如果 INT0x15 返回的 ebx 为零,表示探测结束,否则继续探测
    jnz start_probe
#上述代码正常执行完毕后,在 0x8000 地址处保存了从 BIOS 中获得的内存分布信息,此信息按照 struct e820map 的设置来进行填充。
#这部分信息将在 bootloader 启动 ucore 后,由ucore 的 page_init 函数来根据 struct e820map 的 memmap(定义了起始地址为 0x8000)
finish_probe:

    # Switch from real to protected mode, using a bootstrap GDT
    # and segment translation that makes virtual addresses
    # identical to physical addresses, so that the
    # effective memory map does not change during the switch.
    lgdt gdtdesc
    movl %cr0, %eax
    orl $CR0_PE_ON, %eax
    movl %eax, %cr0

    # Jump to next instruction, but in 32-bit code segment.
    # Switches processor into 32-bit mode.
    ljmp $PROT_MODE_CSEG, $protcseg

.code32                                             # Assemble for 32-bit mode
protcseg:
    # Set up the protected-mode data segment registers
    movw $PROT_MODE_DSEG, %ax                       # Our data segment selector
    movw %ax, %ds                                   # -> DS: Data Segment
    movw %ax, %es                                   # -> ES: Extra Segment
    movw %ax, %fs                                   # -> FS
    movw %ax, %gs                                   # -> GS
    movw %ax, %ss                                   # -> SS: Stack Segment

    # Set up the stack pointer and call into C. The stack region is from 0--start(0x7c00)
    movl $0x0, %ebp
    movl $start, %esp
    call bootmain

    # If bootmain returns (it shouldn't), loop.
spin:
    jmp spin

.data
# Bootstrap GDT
.p2align 2                                          # force 4 byte alignment
gdt:
    SEG_NULLASM                                     # null seg
    SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)           # code seg for bootloader and kernel
    SEG_ASM(STA_W, 0x0, 0xffffffff)                 # data seg for bootloader and kernel

gdtdesc:
    .word 0x17                                      # sizeof(gdt) - 1
    .long gdt                                       # address gdt
