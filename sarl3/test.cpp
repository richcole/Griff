#include <stdio.h>

int main()
{
  fprintf(stdout, "%d\n", 0xffffffff);  
  fprintf(stdout, "%d\n", 0xfffffffe);
  fprintf(stdout, "%u\n", -2147483647);
  fprintf(stdout, "%x\n", -2147483647);
  fprintf(stdout, "%d\n", 0x80000001);
  fprintf(stdout, "%d\n", 0x80000000);

  fprintf(stdout, "%u\n", (unsigned int)-1);
  fprintf(stdout, "%d\n", (int)(unsigned int)-1);
};
