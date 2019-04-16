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
int set_watchpoint(char *e);
bool delete_watchpoint(int position);
void list_watchpoint();
WP *scan_watchpoint();
#endif
