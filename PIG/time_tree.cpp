/* 
Copyright 2004, Richard Cole

This file is part of Griff.

Griff is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.

Griff is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Griff; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "tree.h"
#include "tree_node_malloc_factory.h"
#include "tree_keys.h"

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <set>
#include <sys/time.h>

#define TUPLES_LEN (1024 * 1024)

using namespace std;

class TupleWrap 
{
public:
  Tuple t;

  TupleWrap(Tuple const& a_t) 
  {
    assign_eq(t, a_t);
  };

  TupleWrap& operator=(TupleWrap const& a_t) 
  {
    assign_eq(t, a_t.t);
    return *this;
  };
  
  TupleWrap(TupleWrap const& a_t) 
  {
    *this = a_t;
  };
};

class leq {
public:
  bool operator ()(TupleWrap const& s, TupleWrap const& t)
  {
    return cmp(s.t,t.t) < 0;
  };
};

typedef std::set<TupleWrap,leq> Set;

bool check_tree(TreePtr tree, mmap_file *mp, Set& s)
{
  int count = 0;
  TreeNodeIterator it_t = tree.ptr(mp)->begin(mp);
  Set::const_iterator it_s = s.begin();
  while( (! it_t.is_null()) && it_s != s.end()) {
    if ( cmp(it_t.val(),it_s->t) != 0 ) {
      return false;
    }
    ++count;
    
    it_t.next(); ++it_s;
  };

  return true;
};

timeval operator -(timeval const& t_end, timeval const& t_start)
{
  timeval t_diff;
  
  t_diff.tv_sec = t_end.tv_sec - t_start.tv_sec;
  if ( t_start.tv_usec > t_end.tv_usec ) {
    t_diff.tv_sec -= 1;
    t_diff.tv_usec = (t_end.tv_usec + 1000000) - t_start.tv_usec;
  }
  else {
    t_diff.tv_usec = t_end.tv_usec - t_start.tv_usec;
  }

  return t_diff;
};

ostream& operator<<(ostream& out, timeval const& t)
{
  size_t hours   = t.tv_sec / (60 * 60);
  size_t minutes  = (t.tv_sec % (60 * 60)) / 60;
  float seconds = (t.tv_sec % (60 * 60)) + ((t.tv_usec / 1000) / 1000.0);
  
  out << hours << ":" << minutes << ":" << seconds;
  return out;
};




int main() 
{
  TreePtr tree;

  int        fd = -1;
  mmap_file* mp = 0;

  unlink("test.tree");
  fd = open("test.tree", O_RDWR, 0666);
  if ( fd == -1 ) {
    fd = open("test.tree", O_RDWR | O_CREAT, 0666);

    mp = mmap_init(fd);
    mmap_setkey(mp, PAYLOAD_SIZE_KEY, 64);
    tree = TreePtr::alloc(mp);
    tree.ptr(mp)->_root = TreeNodePtr::null();
    tree.setkey(mp, TREE_KEY);
  }
  else {
    mp = mmap_init(fd);
    tree = TreePtr::getkey(mp, TREE_KEY);
  };
  
  Set s;
  
  int i, k, j = 0;

  size_t count;

  timeval t_start, t_end, t_diff;
  Tuple t;
  
  srand(128);
  gettimeofday(&t_start, 0);
  for(i=0;i<TUPLES_LEN;++i) {
    for(k=0;k<4;++k) {
      t[k] = (rand() >> 2) % 100;
    }
    tree.ptr(mp)->insert(mp, t);
  }
  gettimeofday(&t_end, 0);
  
  cout << "tree insert:\t" << (t_end - t_start) << endl;

  count = 0;
  gettimeofday(&t_start, 0);
  TreeNodeIterator it_t = tree.ptr(mp)->begin(mp);
  while( ! it_t.is_null() ) {
    it_t.next(); 
    ++count;
  }
  gettimeofday(&t_end, 0);
  
  cout << "tree iterate:\t" << (t_end - t_start) << endl;
  cout << "tree count:\t" << count << endl;

  srand(128);
  gettimeofday(&t_start, 0);
  for(i=0;i<TUPLES_LEN;++i) {
    for(k=0;k<4;++k) {
      t[k] = (rand() >> 2) % 100;
    }
    s.insert(t);
  }
  gettimeofday(&t_end, 0);

  cout << "stl insert:\t" << (t_end - t_start) << endl;

  count = 0;
  gettimeofday(&t_start, 0);
  Set::const_iterator it_s = s.begin();
  while( it_s != s.end() ) {
    ++it_s;
    ++count;
  }
  gettimeofday(&t_end, 0);
  
  cout << "stl iterate:\t" << (t_end - t_start) << endl;
  cout << "stl count:\t" << count << endl;

  

};
