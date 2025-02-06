    .code64
    .section    .text
    .global     task_change
    .global     current_active_task
    .global     user_entry
    .global     enable_fs_gs_insns
    .type       task_change,            @function
    .type       current_active_task,    @function
    .type       user_entry,             @function
    .type       enable_fs_gs_insns,     @function
task_change:
    movq        %rax,           %gs:0x010
    popq        %rax    
    movq        %rax,           %gs:0x090
    popq        %rax
    movw        %ax,            %gs:0x0A4
    popq        %rax
    movq        %rax,           %gs:0x098
    popq        %rax
    movq        %rax,           %gs:0x088
    popq        %rax
    movw        %ax,            %gs:0x0A2
    movw        %ds,            %ax
    movw        %ax,            %gs:0x0A0
    movq        %rbx,           %gs:0x018
    movq        %rcx,           %gs:0x020
    movq        %rdx,           %gs:0x028
    movq        %rdi,           %gs:0x030
    movq        %rsi,           %gs:0x038
    movq        %r8,            %gs:0x040
    movq        %r9,            %gs:0x048
    movq        %r10,           %gs:0x050
    movq        %r11,           %gs:0x058
    movq        %r12,           %gs:0x060
    movq        %r13,           %gs:0x068
    movq        %r14,           %gs:0x070
    movq        %r15,           %gs:0x078
    movq        %rbp,           %gs:0x080
    movq        %cr3,           %rax
    movq        %rax,           %gs:0x0A6
    movq        %gs:0x000,      %rax
    fxsave      0x0F0(%rax)
    movq        0x300(%rax),    %rax
    wrgsbase    %rax
    movq        0x2F8(%rax),    %rax
    wrfsbase    %rax
    movq        %gs:0x000,      %rax    
    fxrstor     0x0E0(%rax)
    movq        %gs:0x018,      %rbx
    movq        %gs:0x020,      %rcx
    movq        %gs:0x028,      %rdx
    movq        %gs:0x030,      %rdi
    movq        %gs:0x038,      %rsi
    movq        %gs:0x040,      %r8
    movq        %gs:0x048,      %r9
    movq        %gs:0x050,      %r10
    movq        %gs:0x058,      %r11
    movq        %gs:0x060,      %r12
    movq        %gs:0x068,      %r13
    movq        %gs:0x070,      %r14
    movq        %gs:0x078,      %r15
    movq        %gs:0x080,      %rbp
    movw        %gs:0x0A2,      %ax
    pushq       %rax
    movq        %gs:0x088,      %rax
    pushq       %rax
    movq        %gs:0x098,      %rax
    pushq       %rax
    movw        %gs:0x0A4,      %ax
    pushq       %rax
    movq        %gs:0x090,      %rax
    pushq       %rax
    movw        %gs:0x0A0,      %ax       
    movw        %ax,            %ds
    movw        %ax,            %es
    movq        %gs:0x0A6,      %rax
    movq        %rax,           %cr3
    movq        %gs:0x010,      %rax
    iretq
    .size       task_change,    .-task_change
test_user_fn:
1:
    jmp 1b
user_entry:
    cli
    wrgsbase                    %rdi
    fxrstor                     0x0E0(%rdi)
    pushfq
    popq                        %r11
    movq    %gs:0x018,          %rbx
    movq    %gs:0x090,          %rcx    # instruction pointer goes in the C register for a sysret
    movq    %gs:0x028,          %rdx
    movq    %gs:0x030,          %rdi
    movq    %gs:0x038,          %rsi
    movq    %gs:0x040,          %r8
    movq    %gs:0x048,          %r9
    movq    %gs:0x050,          %r10
    movq    %gs:0x060,          %r12
    movq    %gs:0x068,          %r13
    movq    %gs:0x080,          %rbp
    movq    %gs:0x088,          %rsp
    movq    %gs:0x0A6,          %rax
    movq    %rax,               %cr3
    movq    %gs:0x000,          %r14
    movq    %gs:0x2F8,          %r15
    movw    $0x001B,            %ax
    movw    %ax,                %ds
    movw    %ax,                %es
    movw    %ax,                %fs
    movw    %ax,                %gs
    wrgsbase                    %r14
    wrfsbase                    %r15
    movq    %gs:0x070,          %r14
    movq    %gs:0x078,          %r15
    movq    %gs:0x010,          %rax
    sysretq
    .size       user_entry,             .-user_entry
enable_fs_gs_insns:
    movq        %cr4,       %rax
    orq         $0x10000,   %rax
    movq        %rax,       %cr4
    ret
    .size   enable_fs_gs_insns,         .-enable_fs_gs_insns