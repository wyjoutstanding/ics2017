#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
	char expr[32];//expression
	int old_val;
	int new_val;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */


} WP;

#endif
