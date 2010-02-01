#include "package_analyser.h"

#include "sarl_context.h"
#include "sarl_context_formats.h"
#include "sarl_string_table.h"

#include <sstream>

void Analyser::update_install_or_installed()
{
  PackageSet::const_iterator it;
  install_or_installed.clear();
  FOR_EACH(it, installed) {
    install_or_installed.insert(*it);
  }
  FOR_EACH(it, install) {
    install_or_installed.insert(*it);
  }
};

void Analyser::read_available(istream &inp)
{
  while( ! inp.eof() && ! inp.fail() ) {
    Package *package = new Package();
    read_package(inp, *package);
    package->fullname = fullname(package->name, package->version);
    packages_by_name[package->name].insert(package);
    packages_by_fullname[package->fullname] = package;
    package->is_available = true;
    if ( package->version == "" ) {
      package->is_virtual = true;
      virtual_packages.insert(package);
    }
    available.insert(package);
    all.insert(package);
  };
};

void Analyser::construct_virtuals()
{
  // construct a virtual package from every non-virtual package
  {
    PackageSet::const_iterator it;
    FOR_EACH(it, all) {
      Package *p = *it;

      if ( ! p->is_virtual ) {
        TRACE_ASSERT( p->version != "" );
        
        Package *vp = get(p->name, packages_by_fullname);
        if ( vp == 0 ) { 
          vp = new Package();
          vp->is_virtual = true;
          vp->fullname = p->name;
          vp->name = p->name;
          packages_by_name[vp->name].insert(vp);
          packages_by_fullname[vp->fullname] = vp;
          virtual_packages.insert(vp);
        }
        vp->providers.insert(p);
      }
    }
  }

  // construct a virtual package for any virtual package named
  // in the provides field of an existsing package
  {
    PackageSet::const_iterator it;
    FOR_EACH(it, all) {
      Package *p = *it;
      CExpression::const_iterator c_it;
      FOR_EACH(c_it, p->provides) {
        DExpression::const_iterator d_it;
        FOR_EACH(d_it, **c_it) {
          Expression* expr = *d_it;
          Package *vp = get(expr->name, packages_by_fullname);
          if ( vp == 0 ) {
            vp = new Package; 
            vp->name = expr->name;
            vp->version = expr->version;
            vp->fullname = fullname(expr->name, expr->version);
            vp->is_virtual = true;
            packages_by_name[vp->name].insert(vp);
            packages_by_fullname[vp->fullname] = vp;
            virtual_packages.insert(vp);
          }
          vp->providers.insert(p);
        }
      }
    }
  }

  // check virtual status
  {
    PackageSet::const_iterator it;
    FOR_EACH(it, all) {
      Package *p = *it;
      if ( ! p->is_virtual ) {
        TRACE_ASSERT( (p->version == "") == p->is_virtual );
      }
    }
  }

  all |= virtual_packages;
};

void Analyser::read(
  string const& available_filename, 
  string const& status_filename) 
{
  // read the available packages
  ifstream inp1(available_filename.c_str());
  while( ! inp1.eof() && ! inp1.fail() ) {
    Package *package = new Package();
    read_package(inp1, *package);
    package->fullname = fullname(package->name, package->version);
    packages_by_name[package->name].insert(package);
    packages_by_fullname[package->fullname] = package;
    package->is_available = true;
    if ( package->version == "" ) {
      package->is_virtual = true;
      virtual_packages.insert(package);
    }
    available.insert(package);
    all.insert(package);
  };

  // read the status packages
  ifstream inp2(status_filename.c_str());
  while( ! inp2.eof() && ! inp2.fail() ) {
    Package *package = new Package();
    read_package(inp2, *package);

    Package *p = get(package->fullname, packages_by_fullname);

    if ( p == 0 ) {
      packages_by_name[package->name].insert(package);
      packages_by_fullname[package->fullname] = package;
      p = package;
    }
    else {
      p->status = package->status;
      p->priority = package->priority;
    }
    
    all.insert(p);
    if ( is_member("install", p->status) ) {
      install.insert(p);
    }
    if ( is_member("installed", p->status) ) {
      installed.insert(p);
    }
    all.insert(p);
    status.insert(p);

    if ( package->version == "" ) {
      package->is_virtual = true;
      virtual_packages.insert(package);
    }
  };

  construct_virtuals();
};

void Analyser::print_new_packages()
{
  hash_set<Package *>::iterator it;
  for(it=install.begin(); it!=install.end(); ++it) {
    if ( ! is_member(*it, installed) ) {
      cout << "New Package: " << (*it)->name << endl;
    }
  }
}

bool Analyser::matches(
  CExpression const& cexpr, PackageSet const& P, DExpression const*& fail_expr)
{
  CExpression::const_iterator c_it;
  bool result = true;

  FOR_EACH(c_it, cexpr) {
    if ( matches(**c_it, P) == 0 ) {
      result = false;
      fail_expr = *c_it;
      break;
    }
  }

  return result;
};

Package* Analyser::matches(DExpression const& dexpr, PackageSet const& P)
{
  DExpression::const_iterator d_it;
  Package *result = 0;

  FOR_EACH(d_it, dexpr) {
    if ( (result = matches(**d_it, P)) != 0 ) {
      break;
    }
  }

  return result;
};

Package* Analyser::matches(Expression const& expr, PackageSet const& P)
{
  PackageSet const& ps = packages_by_name[expr.name];
  PackageSet::const_iterator it;
  Package *result = 0;
  FOR_EACH(it, ps) {
    Package *p = *it;
    if ( is_member(p, P) ) {
      if ( matches(p->version, expr.op, expr.version) ) {
        return p;
      }
    }
  }

  return result;
}; 

bool Analyser::matches(string av, string op, string bv)
{
  if ( op == "==" || op == "=" ) {
    return version_cmp(av.c_str(), bv.c_str()) == 0;
  }
  if ( op == "<=" ) {
    return version_cmp(av.c_str(), bv.c_str()) <= 0;
  }    
  if ( op == ">=" ) {
    return version_cmp(av.c_str(), bv.c_str()) >= 0;
  }    
  if ( op == "<" || op == "<<" ) {
    return version_cmp(av.c_str(), bv.c_str()) < 0;
  }    
  if ( op == ">" || op == ">>" ) {
    return version_cmp(av.c_str(), bv.c_str()) > 0;
  }
  if ( op == "" ) {
    if ( bv != "" ) {
      cout << "Error: version defined but operator not in expression." << endl;
    }
    return true;
  }

  return false;
};

int funny_strcmp(char const* av, char const* bv)
{
  while ( *av == *bv ) {
    if ( *av ) {
      ++av, ++bv;
    }
    else {
      return 0;
    }
  }
  if ( isalnum(*av) && ! isalnum(*bv) ) {
    return -1;
  }
  else if ( ! isalnum(*av) && isalnum(*bv) ) {
    return 1;
  }
  else {
    return (*av) - (*bv);
  }
}

int version_string_cmp(char const* av, char const* bv)
{
  int  ai, bi;
  char av_block[1024];
  char bv_block[1024];
  int  av_value = 0;
  int  bv_value = 0;
  
  bool is_numeric_a = false;
  bool is_numeric_b = false;

  if ( av[0] && ! bv[0] ) {
    return 1;
  }

  if ( ! av[0] && bv[0] ) {
    return -1;
  }

  if ( ! av[0] && ! bv[0] ) {
    return 0;
  }

  if ( isdigit(av[0]) ) {
    for(ai=0; av[ai] && isdigit(av[ai]); ++ai ) { 
      av_value *= 10; 
      av_value += av[ai] - '0'; 
    }
    is_numeric_a = true;
  }
  else {
    for(ai=0; av[ai] && ! isdigit(av[ai]); ++ai ) {
      av_block[ai] = av[ai];
    }
    av_block[ai] = 0;
  }
    
  if ( isdigit(bv[0]) ) {
    for(bi=0; bv[bi] && isdigit(bv[bi]); ++bi ) { 
      bv_value *= 10; 
      bv_value += bv[bi] - '0'; 
    }
    is_numeric_b = true;
  }
  else {
    for(bi=0; bv[bi] && ! isdigit(bv[bi]); ++bi ) {
      bv_block[bi] = bv[bi];
    }
    bv_block[bi] = 0;
  }

  if ( is_numeric_a && ! is_numeric_b ) {
    return -1;
  }

  if ( ! is_numeric_a && is_numeric_b ) {
    return -1;
  }

  if ( is_numeric_a && is_numeric_b ) {
    if ( av_value == bv_value ) {
      return version_string_cmp(av + ai, bv + bi);
    }
    else {
      return av_value - bv_value;
    }
  }
    
  if ( ! is_numeric_a && ! is_numeric_b ) {
    int result = funny_strcmp(av_block, bv_block);
    if ( result == 0 ) {
      return version_string_cmp(av + ai, bv + bi);
    }
    else {
      return result;
    }
  }

  cerr << "Impossible to get to here!" << endl;
  return false;
    
};

int version_cmp(char const* av, char const* bv)
{
  char epoch_a[MAX_LINE_LEN], epoch_b[MAX_LINE_LEN];
  char sva[MAX_LINE_LEN], svb[MAX_LINE_LEN];
  char pva[MAX_LINE_LEN], pvb[MAX_LINE_LEN];

  int ai, bi, i, result;

  // read the epochs
  for(ai=0;av[ai] && av[ai] != ':';++ai) epoch_a[ai] = av[ai];
  for(bi=0;bv[bi] && bv[bi] != ':';++bi) epoch_b[bi] = bv[bi];
  if ( av[ai] == ':' ) { 
    epoch_a[ai] = 0; ai++; 
  } 
  else { 
    epoch_a[0] = 0; ai = 0; 
  };
  if ( bv[bi] == ':' ) { 
    epoch_b[bi] = 0; bi++; 
  } else { 
    epoch_b[0] = 0; bi = 0; 
  };

  // read the software version
  for(i=0;av[ai] && av[ai] != '-'; ++ai,++i) sva[i] = av[ai];
  sva[i] = 0;
  for(i=0;bv[bi] && bv[bi] != '-'; ++bi,++i) svb[i] = bv[bi];
  svb[i] = 0;
  if ( av[ai] == '-' ) ai++;
  if ( bv[bi] == '-' ) bi++;

  // read the package version
  for(i=0;av[ai]; ++ai,++i) pva[i] = av[ai];
  pva[i] = 0;
  for(i=0;bv[bi]; ++bi,++i) pvb[i] = bv[bi];
  pvb[i] = 0;

  result = version_string_cmp(epoch_a, epoch_b);
  if ( result == 0 ) {
    result = version_string_cmp(sva, svb);
  }
  if ( result == 0 ) {
    result = version_string_cmp(pva, pvb);
  }
  return result;
};

PackageSet *Analyser::provides_closure(Package *p)
{
  PackageSet *result = new PackageSet();
  DExpression::const_iterator d_it;
  CExpression::const_iterator c_it;
  
  FOR_EACH(c_it, p->provides) {
    FOR_EACH(d_it, **c_it) {
      Expression *e = *d_it;
      Package *vp = 0;
      if ( e->version == "" ) {
        vp = get(e->name, packages_by_fullname);
        if ( vp ) {
          result->insert(vp);
        }
        else {
          cout << "Error: unable to find virtual package '" 
               << e->name << "'" << endl;
        }
      }
      else {
        cout << "Error: virtual packages shouldn't have versions." << endl;
      }
    }
  }

  return result;
};

PackageSet *Analyser::provides_closure(PackageSet const& P)
{
  PackageSet *result = new PackageSet();
  PackageSet::const_iterator it;
  PackageSet::const_iterator ps_it;
  FOR_EACH(it, P) {
    result->insert(*it);
    PackageSet *ps = provides_closure(*it);
    FOR_EACH(ps_it, *ps) {
      result->insert(*ps_it);
    }
    delete ps;
  }

  return result;
};

bool Analyser::check_multiple_versions()
{
  bool result;

  PackageSetMap::const_iterator it;
  FOR_EACH(it, packages_by_name) {
    PackageSet const& ps = it->second;
    if ( ps.size() == 0 ) {
      cout << "Warning: package name has no packages, '";
      cout << *ps.begin() << endl;
    }
    else if ( ps.size() == 1 ) {
    }
    else if ( ps.size() == 2 ) {
      PackageSet::const_iterator ps_it = ps.begin();
      Package *a = *ps_it;
      Package *b = *(++ps_it);
      if ( 
        (a->version == "" && b->version != "" ) ||
        (a->version != "" && b->version == "" ) ) {
      }
      else {
        cout << "Warning: Two packages with different versions.";
        cout << it->first << endl;
        cout << "  " << a->name << " " << a->version << endl;
        cout << "  " << b->name << " " << b->version << endl;
      }
    }
    else {
      cout << "Warning: More than two version for package." << endl;
      PackageSet::const_iterator ps_it;
      FOR_EACH(ps_it, ps) {
        cout << "  " << (*ps_it)->name << " " << (*ps_it)->version << endl;
      }
    }
  }

  return result;
};


bool Analyser::check_dependencies(PackageSet const& target)
{
  bool result = true; // FIXME: not being set

  PackageSet::const_iterator it;
  FOR_EACH(it, target) {
    Package *p = *it;
    DExpression const* fail_expr;
    if ( ! matches(p->depends, target, fail_expr) ) {
      cout << "Missing dependency for " << p->name << ": ";
      cout << *fail_expr << endl;
      DExpression::const_iterator e_it;
      FOR_EACH(e_it, *fail_expr) {
        PackageSet const& dps = packages_by_name[(*e_it)->name];
        PackageSet::const_iterator it_dps;
        FOR_EACH(it_dps, dps) {
          Package *dp = *it_dps;
          cout << "  available version is: " << dp->name << " " 
               << dp->version << endl;
          cout << "    version is not in";
          if ( ! is_member(dp, target) ) cout << " target";
          if ( ! is_member(dp, installed) ) cout << " installed";
          if ( ! is_member(dp, install) ) cout << " install";
          if ( ! is_member(dp, status) ) cout << " status";
          if ( ! is_member(dp, all) ) cout << " all";
          if ( ! is_member(dp, available) ) cout << " available";
          cout << endl;
          cout << "    version is in";
          if ( is_member(dp, target) ) cout << " target";
          if ( is_member(dp, installed) ) cout << " installed";
          if ( is_member(dp, install) ) cout << " install";
          if ( is_member(dp, status) ) cout << " status";
          if ( is_member(dp, all) ) cout << " all";
          if ( is_member(dp, available) ) cout << " available";
          cout << endl;
        }
        if ( dps.size() == 0 ) {
          cout << "  " << (*e_it)->name << " is not available." << endl;
        }
      }
    }
  }
  
  return result;
};

bool Analyser::check_conflicts(PackageSet const& target)
{
  PackageSet::const_iterator it;
  PackageSet tmp = target;
  FOR_EACH(it, target) {
    Package *p = *it;
    if ( p->conflicts.size() > 0 ) {

      CExpression::const_iterator c_it;
      DExpression::const_iterator d_it;
      FOR_EACH(c_it, p->conflicts) {
        FOR_EACH(d_it, **c_it) {
          Expression *expr = *d_it;
          Package *cp = 0;
          if ( (cp = matches(*expr, target)) != 0 ) {

            // check if cp is virtual it is supported by a package
            // other than outseleves
            if ( cp->is_virtual ) {
              PackageSet::const_iterator cp_p_it;
              Package *ok_supporter = 0;
              FOR_EACH(cp_p_it, cp->providers) {
                Package *supporter = *cp_p_it;
                if ( supporter != p && is_member(supporter, target) ) {
                  ok_supporter = supporter;
                  break;
                }
              }
              if ( ok_supporter ) {
                cout << "Conflict for package " << p->fullname << ":" << endl;
                cout << "  " << ok_supporter->fullname << " supports " 
                     << cp->fullname << endl;
              }
              else {
                cout << "Warning: Conflict for package " << p->fullname 
                     << " being ignored." << endl;
                cout << "  " << cp->fullname << " is only supported by " 
                     << p->fullname << endl;
              }
            }
            else {
              cout << "Warning: Non virtual package in conflict expression";
              cout << endl;
              cout << "  package " << p->fullname << " conflicts with ";
              cout << cp->fullname << endl;
              if ( cp->name == p->name ) {
                cout << "  non virtual package is conflicting with itself.";
                cout << endl;
              }
            }
          }
        }
      }
    }
  }

  return false; // FIXME: not being calculated
};

PackageSet* Analyser::provides(Package const& p)
{
  CExpression::const_iterator c_it;
  DExpression::const_iterator d_it;
  PackageSet *result = new PackageSet();

  FOR_EACH(c_it, p.provides) {
    FOR_EACH(d_it, **c_it) {
      Expression *expr = *d_it;
      Package *providee = 
        get(fullname(expr->name, expr->version), packages_by_fullname);
      result->insert(providee);
    }
  }

  return result;
};

template<class T>
  void append_to(T const& depends, T &ce)
{
  typename T::const_iterator it;
  FOR_EACH(it, depends) {
    ce.push_back(*it);
  }
}

template<class T>
  void pop_from(T const& depends, T &ce)
{
  typename T::const_iterator it;
  FOR_EACH(it, depends) {
    ce.pop_back();
  }
}

void Analyser::update_conflict_packages(Package& p)
{
  CExpression::const_iterator c_it;
  DExpression::const_iterator d_it;
  PackageSet::const_iterator cp_it;

  FOR_EACH(c_it, p.conflicts) {
    FOR_EACH(d_it, **c_it) {
      PackageSet *mps = matching_packages(**d_it);
      // cout << "Conflicts for: " << p.fullname << endl;
      FOR_EACH(cp_it, *mps) {
        Package *c = *cp_it;
        if ( c != &p ) {
          // cout << "  conflict with: " << c->fullname << endl;
          p.conflict_packages.insert(c);
        }
      }
      delete mps;
    }
  }
}

void Analyser::make_conflict_reflexive(Package& p)
{
  PackageSet::const_iterator it;
  FOR_EACH(it, p.conflict_packages) {
    Package *cp = *it;
    cp->conflict_packages.insert(&p);
  }
}

void Analyser::dump_conflicts()
{
  PackageSet::const_iterator it;
  PackageSet::const_iterator c_it;
  FOR_EACH(it, all) {
    Package *p = *it;
    cout << "Dump Conflicts for: " << p->fullname << endl;
    FOR_EACH(c_it, p->conflict_packages) {
      Package *c = *c_it;
      cout << "  conflict with: " << c->fullname << endl;
    }
  }
};

PackageSet* Analyser::matching_packages(Expression const& expr)
{
  //
  // returns only concrete packages that match the expression
  // possibly by providing something in expr
  //

  PackageSet const& cand = packages_by_name[expr.name];
  PackageSet *result = new PackageSet();
  PackageSet::const_iterator it;
  PackageSet::const_iterator p_it;
  FOR_EACH(it, cand) {
    Package *p = *it;
    if ( p->is_virtual ) {
      if ( expr.op == "" ) {  // match a virtual expression only
        FOR_EACH(p_it, p->providers) {
          Package* provider = *p_it;
          TRACE_ASSERT(! provider->is_virtual);
          result->insert(provider);
        }
      }
    }
    else {
      if ( matches(p->version, expr.op, expr.version) ) {
        result->insert(p);
      }
    }
  }

  return result;
};

PackageSet* Analyser::conflicts(Package *p, PackageSet const& ps)
{
  PackageSet::const_iterator it;
  PackageSet *result = new PackageSet(p->conflict_packages);
  *result &= ps;
  result->erase(p);
  return result;
};

void Analyser::update_package_conflicts()
{
  PackageSet::const_iterator it;
  FOR_EACH(it, all) {
    update_conflict_packages(**it);
  }
  FOR_EACH(it, all) {
    make_conflict_reflexive(**it);
  }
}

void Analyser::print_solutions(
  vector<pair<CExpression *, PackageSet *> > &solutions
)
{
  ifstream solutions_file("solutions.cxt");
  
  Lexicon lex;

  sarl_context_t *context = sarl_context_new();
  sarl_string_table_t *gs = sarl_string_table_default_new();
  sarl_string_table_t *ms = sarl_string_table_default_new();

  vector<pair<CExpression *, PackageSet *> >::const_iterator it;
  sarl_unsigned_int g, m;

  sarl_bit_set_t* package_attributes = sarl_bit_set_default_new();

  g = 0;
  FOR_EACH(it, solutions) {
    ++g;
    sarl_context_insert_object(context, g);
    CExpression *cexpr = it->first;
    CExpression::const_iterator it_c;
    FOR_EACH(it_c, *cexpr) {
      DExpression *dexpr = *it_c;
      ostringstream buf; buf << "Unsolved: " << *dexpr;
      m = lex.index(buf.str());
      sarl_context_insert(context, g, m);
      sarl_context_insert_attribute(context, m);
    }
    PackageSet* ps = it->second;
    PackageSet::const_iterator it_ps;
    FOR_EACH(it_ps, *ps) {
      Package *package = *it_ps;
      ostringstream buf; buf << package->fullname;
      m = lex.index(buf.str());
      sarl_context_insert(context, g, m);
      sarl_context_insert_attribute(context, m);
      sarl_bit_set_set(package_attributes, m);
    }
  }

  sarl_string_table_populate(gs, 1, g);
  sarl_unsigned_int i;
  for(i=0;i<lex.words.size();++i) {
    sarl_string_table_set(ms, i+1, (char *)lex.word(i+1).c_str());
  }
  {
    FILE *outp = fopen("solutions.cxt", "w");
    sarl_context_print_to_cxt(context, gs, ms, outp);
    fclose(outp);
  }
    
  sarl_context_t *sub = sarl_context_attribute_subcontext(
    context, package_attributes);

  {
    FILE *outp = fopen("solutions_packages.cxt", "w");
    sarl_context_print_to_cxt(sub, gs, ms, outp);
    fclose(outp);
  }
};

bool Analyser::solve(
  CExpression& ce, PackageSet & ps, CExpression &unsolved, 
  vector<pair<CExpression *, PackageSet *> > &solutions)
{
  // return true if at least one solution was found

  bool result = false;

  if ( ce.size() > 0 ) {
    DExpression *de = ce.front();
    ce.pop_front();
    {
      DExpression::const_iterator it;
      cout << "Trace: solving expression: " << *de << endl;
      FOR_EACH(it, *de) {
        Expression *expr = *it;
        cout << "  expression: " << *expr << endl;
        if ( matches(*expr, ps) ) {
          cout << "    already solved" << endl;
          result = result || solve(ce, ps, unsolved, solutions);
        }
        else {
          cout << "    not solved, seeking matches" << endl;
          PackageSet *match_ps = matching_packages(**it);
          PackageSet::const_iterator p_it;
          FOR_EACH(p_it, *match_ps) {
            Package *p = *p_it;
            cout << "      candidate: " << p->fullname << endl;
            PackageSet *cps = conflicts(p, ps);
            if ( cps->size() == 0 ) {
              cout << "        no conflict " << p->fullname << endl;
              bool is_p_inserted = false;
              if ( ! is_member(p, ps) ) {
                ps.insert(p);
                is_p_inserted = true;
              }
              append_to(p->depends, ce);
              result = result || solve(ce, ps, unsolved, solutions);
              pop_from(p->depends, ce);
              if ( is_p_inserted ) ps.erase(p);
            }
            else {
              cout << "        conflict: " << p->fullname << endl;
              PackageSet::const_iterator cps_it;
              FOR_EACH(cps_it, *cps) {
                cout << "          with: " << (*cps_it)->fullname << endl;
              }
            }
            delete cps;
          }

          if ( ! result ) {
            cout << "    Skipping, no solution found for:" << *de << endl;
            unsolved.push_back(de);
            cout << "** Parial Solution: unsolved=" << unsolved.size();
            cout << ", packages=" << ps.size() << endl;
            solutions.push_back(
              pair<CExpression *, PackageSet *>(
                new CExpression(unsolved), new PackageSet(ps)
              )
            );
            print_solutions(solutions);
            // solve(ce, ps, unsolved, solutions);
            unsolved.pop_back();
          }        
        }
      }
    }
    ce.push_front(de);
  }
  else {
    cout << "** Solution: unsolved=" << unsolved.size();
    cout << ", packages=" << ps.size() << endl;
    solutions.push_back(
      pair<CExpression *, PackageSet *>(
        new CExpression(unsolved), new PackageSet(ps)
      )
    );
    print_solutions(solutions);
    result = true;
  }

  return result;
    
};

PackageSet* Analyser::maching_packages(DExpression const& de) 
{
  DExpression::const_iterator d_it;
  PackageSet *result = new PackageSet;
  FOR_EACH(d_it, de) {
    Expression *e = *d_it;
    PackageSet *match_ps = matching_packages(*e);
    PackageSet::const_iterator m_it;
    FOR_EACH(m_it, *match_ps) {
      result->insert(*m_it);
    }
  }
  return result;
};  
      
void Analyser::package_closure(CExpression cs, PackageSet &ps)
{
  while ( cs.size() != 0 ) {
    DExpression *de = cs.front();
    cs.pop_front();
    PackageSet *match_ps = maching_packages(*de);
    PackageSet::const_iterator m_it;
    FOR_EACH(m_it, *match_ps) {
      if ( ! is_member(*m_it, ps) ) {
        ps.insert(*m_it);
        append_to((*m_it)->depends, cs);
      }
    }
    delete match_ps;
  }
};

bool Analyser::dump_cnf(CExpression const& ce, Lexicon &lex, ostream& out)
{
  bool result = true;

  CExpression::const_iterator c_it;
  FOR_EACH(c_it, ce) {
    PackageSet *match_ps = maching_packages(**c_it);
    if ( match_ps->size() > 0 ) {
        out << "c target dependencies " << endl;
        ostringstream buf;
        PackageSet::const_iterator m_it;
        FOR_EACH(m_it, *match_ps) {
          buf << (*m_it)->fullname << " ";
          out << lex.index((*m_it)->fullname) << " ";
        }
        out << endl;
        out << "c deps were " << buf.str() << endl;
    }
    else {
      cerr << "Unresolved dependancy:" << endl;
      cerr << "  Depends: " << **c_it << endl;
      result = false;
    }
    delete match_ps;
  }
  
  return result;
};

void Analyser::dump_cnf(PackageSet &ps, Lexicon &lex, ostream& out, ostream &outp_reason)
{
  PackageSet::const_iterator ps_it;
  FOR_EACH(ps_it, ps) {
    Package *p = *ps_it;
    CExpression *ce = &p->depends;
    CExpression::const_iterator c_it;
    FOR_EACH(c_it, *ce) {
      PackageSet *match_ps = maching_packages(**c_it);
      if ( match_ps->size() > 0 ) {
        out << "c dependencies of " << p->fullname << endl;
        out << "-" << lex.index(p->fullname);
        ostringstream buf;
        PackageSet::const_iterator m_it;
        FOR_EACH(m_it, *match_ps) {
          buf << " " << (*m_it)->fullname;
          out << " " << lex.index((*m_it)->fullname);
        }
        out << endl;
        out << "c deps were " << buf.str() << endl;
      }
      else {
        out << "c UNMATCHED DEPENDS FOR " << p->fullname << endl;
        out << "c   DEPENDENCY IS: " << **c_it << endl;
        out << "-" << lex.index(p->fullname) << endl;
        outp_reason << p->fullname << " " << **c_it << endl;
      }
      delete match_ps;
    };

    {
      PackageSet *c_ps = &p->conflict_packages; // conflicts(p, ps);
      // cout << "Conflicts for: " << p->fullname << endl;
      PackageSet::const_iterator c_it;
      FOR_EACH(c_it, p->conflict_packages) {
        // cout << "  " << (*c_it)->fullname << is_member(*c_it, ps) << endl;
      }
      if ( c_ps->size() > 0 ) {
        FOR_EACH(c_it, *c_ps) {
          out << "c conflict " << p->fullname << " and " << (*c_it)->fullname;
          out << endl;
          out << "-" << lex.index(p->fullname) << " ";
          out << "-" << lex.index((*c_it)->fullname) << endl;
        }
      }
      else {
        out << "c no conflicts for " << p->fullname << endl;
      }
      // delete c_ps; RJ: no longer being allocated
    }
  }
}
    
      
  
