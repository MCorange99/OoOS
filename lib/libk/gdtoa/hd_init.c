#include "gdtoa.h"
unsigned char __hexdig_D2A[256];
static void htinit(unsigned char* h, unsigned char* s, int inc)
{
  int i, j;
  for (i = 0; (j = s[i]) != 0; i++)
    h[j] = i + inc;
}
void __hexdig_init_D2A(void)
{
  htinit(__hexdig_D2A, (unsigned char*)"0123456789", 0x10);
  htinit(__hexdig_D2A, (unsigned char*)"abcdef", 0x10 + 10);
  htinit(__hexdig_D2A, (unsigned char*)"ABCDEF", 0x10 + 10);
}