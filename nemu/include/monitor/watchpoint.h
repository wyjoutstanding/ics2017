#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
	char expr[32];//expression
	uint32_t old_val;
	uint32_t new_val;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */


} WP;

#endif
