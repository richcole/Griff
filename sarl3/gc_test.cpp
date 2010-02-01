#include <gc/gc.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>


typedef struct _Fred Fred;
typedef struct _Chucky Chucky;
typedef struct _Greg Greg;

struct _Fred {
  char pad[1024];
};

struct _Chucky {
  char pad[1024];
  Fred *fred;
};

struct _Greg {
  Chucky* chucky;
};

static unsigned int finalized_called = 0;

static void finalize_chucky(GC_PTR ptr, GC_PTR data)
{
  fprintf(stderr, "Finalize p=%p\n", ptr);
  finalized_called = 1;
};

Chucky* chucky_new() {
  Chucky* chucky = (Chucky *)GC_MALLOC_UNCOLLECTABLE(sizeof(Chucky));
  GC_REGISTER_FINALIZER_NO_ORDER(chucky, finalize_chucky, 0, 0, 0);
  memset(chucky->pad, 1, sizeof(chucky->pad));
  chucky->fred = (Fred *)GC_MALLOC_ATOMIC(sizeof(Fred));
  memset(chucky->fred->pad, 1, sizeof(chucky->fred->pad));
  return chucky;
}

Greg* greg_new() {
  Greg *greg = (Greg *)malloc(sizeof(Greg));
  greg->chucky = chucky_new();
}

main() {
  Greg *greg = greg_new();
  GC_gcollect();

  if (finalized_called) {
    fprintf(stdout, "Error, finalized called!\n");
  }
  else {
    fprintf(stdout, "Test succeeded.\n");
  }

  return finalized_called ? -1 : 0;
};
