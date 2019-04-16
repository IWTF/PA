#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
#include "expr.h"

typedef struct watchpoint {
  int NO;
  char expr[30];
  int new_val;
  int old_val;
  int type;  // 区分是监视点(0)还是断点(1)
  int str;   // int3时，储存原内存中的数据
  struct watchpoint *next;

  /* TODO: Add more members if necessary */


} WP;

WP *new_wp(char *expr);
int set_watchpoint(char *e);
bool delete_watchpoint(int position);
void list_watchpoint();
WP *scan_watchpoint();
int set_breakpoint(char *e);
#endif
