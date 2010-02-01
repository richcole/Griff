#include "package_analyser.h"

int main(int argc, char **argv)
{
  if ( argc < 3 ) {
    cerr << "Usage dpkg available status [extras]" << endl;
    exit(-1);
  }

  Analyser analyser;
  analyser.read(argv[1], argv[2]);

  PackageSet target = analyser.installed;

  int i;
  for(i=3;i<argc;++i) {
    PackageSet const& ps = analyser.packages_by_name[argv[i]];
    
    if ( ps.size() == 0 ) {
      cout << "Error: couldn't locate '" << argv[i] << "'" << endl;
    }
    else if ( ps.size() == 1 ) {
      target.insert(*ps.begin());
    }
    else {
      cout << "Warning: multiple versions for package '" 
           << argv[i] << "'" << endl;
      PackageSet::const_iterator it;
      Package *cand = 0;
      FOR_EACH(it, ps) {
        Package *curr_p = *it;
        cout << "  Potential Package: " << curr_p->name << " ";
        cout << curr_p->version << endl;
        if ( 
          cand == 0 || 
          version_cmp(
            cand->version.c_str(), 
            curr_p->version.c_str()) < 0 
        ) {
          cand = curr_p;
        }
      }
      cout << "  Choosen Latest Version: " << cand->name << " ";
      cout << cand->version << endl;
      target.insert(cand);
    }
  }

  PackageSet *pr_target = analyser.provides_closure(target);

//  cout << "Checking for multiple packages." << endl;
//  analyser.check_multiple_versions();

  cout << "Checking dependencies for files to install..." << endl;
  analyser.check_dependencies(*pr_target);

  cout << "Checking conflicts for files to install..." << endl;
  analyser.check_conflicts(*pr_target);

  delete pr_target;

  return 0;
};
