#include "sarl_finalization.h"
#include <stdio.h>

void trace_finalization(GC_PTR obj, GC_PTR client_data)
{
  fprintf(stderr, "Finalize: p=%p, s=%s\n", obj, (char *)client_data);
};
