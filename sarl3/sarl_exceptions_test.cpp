#include "sarl_exceptions.h"
#include "sarl_assert.h"

int main(int argc, char **argv)
{
  sarl_test_assert(sarl_errno == 0);
  return 0;
};
