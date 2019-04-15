#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
#include "expr.h"

typedef struct watchpoint {
  int NO;
  char expr[30];
  int new_val;
  int old_val;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */


} WP;

WP *new_wp(char *expr);
void free_wp(int position);
#endif
