#include "package_analyser.h"

int main()
{
  cout << "Compare result: " << version_cmp("4.1.0", "6.8.2.dfsg.1-4") << endl;
  cout << "Compare result: " << version_cmp("1.9.0", "1.10.1-2") << endl;
  cout << "Compare result: " << version_cmp("1:2.10.2-1", "1:2.8.1") << endl;
  cout << "Compare result: " << version_cmp("3.3.3", "3:3.3.4-3") << endl;
  cout << "Compare result: " << version_cmp("4:3.3.2-1", "4:3.3.2-1") << endl;
  cout << "Compare result: " << version_cmp("4:3.3.2-4", "4:3.1.4-2") << endl;

  return 0;
};
