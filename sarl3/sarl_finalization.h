#ifndef SARL_FINALIZATION_H
#define SARL_FINALIZATION_H

#include <gc/gc.h>

void trace_finalization(GC_PTR obj, GC_PTR client_data);

#endif
