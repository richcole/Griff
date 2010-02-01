#ifndef SARL_RB_GET_OBJECT_H
#define SARL_RB_GET_OBJECT_H

#include <ruby.h>

void sarl_free_ruby_object(void *ptr);
VALUE sarl_get_ruby_object(VALUE klass, void *ptr);
void sarl_set_ruby_object(VALUE klass, void *ptr);
int sarl_exists_ruby_object(void *ptr);

#endif
     
