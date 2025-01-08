#include "direct_text_render.hpp"
#include "arch/idt_amd64.h"
#include "heap_allocator.hpp"
#include "rtc.h"
#include "bits/icxxabi.h"
#include "keyboard_driver.hpp"
#include "stdlib.h"
#include "bits/stdexcept.h"
#include "fs/data_buffer.hpp"
#include "arch/com_amd64.h"
#include "generic_binary_buffer.hpp"
extern psf2_t* __startup_font;
extern "C" uint64_t errinst;
static direct_text_render startup_tty;
static bool can_print = false;
static serial_driver_amd64* com;
static sysinfo_t* __sysinfo;
static char dbgbuf[19]{'0', 'x'};
constexpr static const char digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
void buffer_test()
{
    data_buffer<char> tb{};
    tb.sputn("Eleventeenology hath been studied", 33);
    startup_tty.print_line(tb.__str());
    char test_sink[29];
    test_sink[28] = 0;
    generic_binary_buffer<uint16_t> twide{15};
    twide.rputn<char>("Derpleblerps the merpleyerps", 28);
    twide.rgetn<char>(test_sink, 28);
    startup_tty.print_line(test_sink);
}
__isr_registers 
void debug_ecode(byte idx, qword ecode)
{
    if(ecode) 
    {
        startup_tty.print_text("INT# ");
        debug_print_num(idx, 2);
        startup_tty.print_text(", ECODE ");
        debug_print_num(ecode);
        startup_tty.print_text(", RIP@ ");
        debug_print_num(errinst);
    }
}
void run_tests() throw()
{
    interrupt_table::add_interrupt_callback(debug_ecode);
    can_print = true;
    srand(syscall_time(0));
    startup_tty.print_line("Hello world!");
    startup_tty.print_line(std::to_string(42));
    startup_tty.print_line(std::to_string(__sysinfo));
    startup_tty.print_line(std::to_string(3.14159265358L));
    startup_tty.print_line(std::to_string(rand()));
    buffer_test();
    if(com)
    {
        interrupt_table::add_irq_handler(4, [&]() -> void __isr_registers { size_t n = com->in_avail(); char buf[n + 1]; com->sgetn(buf, n); buf[n] = 0; startup_tty.print_text(buf); });
        com->sputn("Hello Serial!\n", 14);
        com->pubsync();
    }
}
extern "C" void _init();
extern "C"
{
    void direct_write(const char* str) { startup_tty.print_text(str); }
    void debug_print_num(uintptr_t num, int lenmax) { for(size_t i = lenmax + 1; i > 1; i--, num >>= 4) { dbgbuf[i] = digits[num & 0xF]; } dbgbuf[lenmax + 2] = 0; startup_tty.print_text(dbgbuf); }
    [[noreturn]] void abort() { startup_tty.endl(); startup_tty.print_line("ABORT"); if(com) { com->sputn("ABORT\n", 6); com->pubsync(); } while(1) { asm volatile("hlt" ::: "memory"); } }
    void panic(const char* msg) { std::string estr{"ERROR: "}; estr.append(msg); startup_tty.print_line(estr); if(com) { com->sputn(estr.c_str(), estr.size()); com->pubsync(); } }
    extern void* isr_table[];
    extern void gdt_setup();
    void kmain(sysinfo_t* sysinfo, mmap_t* mmap, pagefile* pg)
    {
        cli();
        nmi_disable();
        // This initializer is freestanding by necessity. It's called before _init because some global constructors invoke the heap allocator (e.g. the serial driver).
        heap_allocator::init_instance(pg, mmap); 
        // Because we are linking a barebones crti.o and crtn.o into the kernel, we can control the invocation of global constructors by calling _init. 
        _init();
        // The GDT is only used to set up the IDT (as well as enabling switching rings), so setting it up after the heap allocator is fine.
        gdt_setup();
        // The actual setup code for the IDT just fills the table with the same trampoline routine that calls the dispatcher for interrupt handlers.
        idt_init();
        fadt_t* fadt = nullptr;
        if(sysinfo->xsdt) fadt = find_fadt(sysinfo->xsdt);
        if(fadt) rtc_driver::init_instance(fadt->century_register);
        else rtc_driver::init_instance();
        // The startup "terminal" just directly renders text to the screen using a font that's stored in a data section linked in from libk.
        new (&startup_tty) direct_text_render{ sysinfo, __startup_font, 0x00FFFFFF, 0 };
        startup_tty.cls();
        // The base keyboard driver object abstracts out low-level initialization code that could theoretically change for different implementations of keyboards.
        keyboard_driver_base* kb = get_kb_driver();
        kb->initialize();
        if(serial_driver_amd64::init_instance()) com = serial_driver_amd64::get_instance();
        __sysinfo = sysinfo;
        nmi_enable();
        sti();
        // Any theoretical exceptions encountered in the test methods will propagate out to here. std::terminate essentially does the same thing as this, but the catch block also prints the exception's message.
        try
        {
            run_tests();
            while(1);  
        } 
        catch(std::exception& e)
        {
            panic(e.what());
            abort();
            __builtin_unreachable();
        }
        __cxa_finalize(0);
        __builtin_unreachable();
    }
}
