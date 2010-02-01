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
#include "tree_node.h"
#include "tree_node_factory.h"
#include "payload.h"


inline bool is_black(TreeNode *node)
{
  return node && node->is_black();
};

inline bool is_red(TreeNode *node)
{
  return node && ! node->is_black();
};

inline TreeNode* ballance_ll(TreeNode* n) 
{
  if (is_black(n) && is_red(n->left()) && is_red(n->left()->left())) {
    TreeNode *t = n->left();
    TreeNode *l = n->left()->left();
    TreeNode *r = n;
    r->set_left(n->left()->right());
    t->set_left(l);
    t->set_right(r);
    t->set_black(false); l->set_black(true); r->set_black(true);
    if (t->right()->left() == 0) {
      t->left()->set_left_thread(t);
    };
    return t;
  }
  else {
    return n;
  };
};

inline TreeNode* ballance_rr(TreeNode* n) 
{
  if (is_black(n) && is_red(n->right()) && is_red(n->right()->right())) {
    TreeNode *t = n->right();
    TreeNode *l = n;
    TreeNode *r = n->right()->right();
    l->set_right(n->right()->left());
    t->set_left(l);
    t->set_right(r);
    t->set_black(false); l->set_black(true); r->set_black(true);
    if (t->left()->right() == 0) {
      t->left()->set_right_thread(t);
    };
    return t;
  }
  else {
    return n;
  };
};

inline TreeNode* ballance_lr(TreeNode* n) 
{
  if (is_black(n) && is_red(n->left()) && is_red(n->left()->right())) {
    TreeNode *t = n->left()->right();
    TreeNode *l = n->left();
    TreeNode *r = n;
    TreeNode *b = n->left()->right()->left();
    TreeNode *c = n->left()->right()->right();
    l->set_right(b);
    r->set_left(c);
    t->set_left(l);
    t->set_right(r);
    t->set_black(false); l->set_black(true); r->set_black(true);
    if (t->left()->right() == 0) {
      t->left()->set_right_thread(t);
    }
    if (t->right()->left() == 0) {
      t->right()->set_left_thread(t);
    }
    return t;
  }
  else {
    return n;
  };
};

inline TreeNode* ballance_rl(TreeNode* n) 
{
  if (is_black(n) && is_red(n->right()) && is_red(n->right()->left())) {
    TreeNode *t = n->right()->left();
    TreeNode *l = n;
    TreeNode *r = n->right();
    TreeNode *b = n->right()->left()->left();
    TreeNode *c = n->right()->left()->right();
    l->set_right(b);
    r->set_left(c);
    t->set_left(l);
    t->set_right(r);
    t->set_black(false); l->set_black(true); r->set_black(true);
    if (t->left()->right() == 0) {
      t->left()->set_right_thread(t);
    }
    if (t->right()->left() == 0) {
      t->right()->set_left_thread(t);
    }
    return t;
  }
  else {
    return n;
  };
};

TreeNode* ballance(TreeNode *n)
{
  n = ballance_ll(n);
  n = ballance_rr(n);
  n = ballance_lr(n);
  n = ballance_rl(n);
  return n;
};
