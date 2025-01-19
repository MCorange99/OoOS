#include "kernel/kernel_defs.h"
bool checksum(acpi_header* h)
{
    char* c = vaddr_t{ h };
    signed char sum{ 0 };
    for(size_t i = 0; i < h->length; i++) sum += c[i];
    return sum == 0;
}
void* find_system_table(xsdt_t* xsdt, const char* expected_sig)
{
    acpi_header* cur = vaddr_t{ xsdt->sdt_pointers[0] };
    for(size_t i = 0; i < ((xsdt->hdr.length - sizeof(xsdt->hdr)) / 8); cur = vaddr_t{ xsdt->sdt_pointers[++i] }) { if(__builtin_memcmp(cur->signature, expected_sig, 4) == 0 && checksum(cur)) return cur; }
    return nullptr;
}