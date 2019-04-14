#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;//both not contain head,so pay attention to the NULL list

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
//return a free node form free list
WP* new_wp(){
  if(free_ == NULL)assert(0);
  else{
		WP* wp = free_;
		free_ = free_->next;
		return wp;
	}
}
//delete a node from head list and insert into free list 
void free_wp(WP* wp) {
	if(free_ == NULL){
		wp->next = NULL;
		free_ = wp;
	}
	else {
		wp->next = free_;
		free_ = wp;
	}
}
//set watchpoint
int set_watchpoint(char* e) {
  WP* wp = new_wp();
	bool success = true;
	wp->old_val = expr(e,&success);
	memset(wp->expr,'\0',32);
	strcpy(wp->expr,e);
	wp->next = NULL;
	//Link to head list in order
	if(head == NULL){
		wp->next = NULL;
		head = wp;
	}
	else {
	  WP* pre = NULL,*pcur=head;
		while(pcur != NULL && wp->NO > pcur->NO){
			pre = pcur;
			pcur = pcur->next;
		}
		if(pre == NULL) {//before the first node
		  wp->next = head;
	  	head = wp;
		}
		else {
			wp->next = pcur;
			pre->next = wp;
		}
	}
	printf("Set watchpoint #%d\nexpr %s\nold_val ox%08x\n",wp->NO,wp->expr,wp->old_val);
  return wp->NO;
}
//delete watchpoint by specify NO
void delete_watchpoint(int NO) {
	if(head == NULL){
		printf("There is no watchpoint!\n");
		return;
	}
	else {
		WP* wp = head,*pre=NULL;
		while(wp != NULL){
			if(wp->NO == NO){
				if(pre == NULL)head = head->next;//only one node or the first node is object
				else pre->next = wp->next;
				free_wp(wp);
				printf("Watchpoint %d deleted!\n",wp->NO);
				return;
			}
			pre = wp;
			wp = wp->next;
		}
		printf("There is no such watchpoint NO!\n");
	}
}
//display list of watchpoints
void list_watchpoint() {
	if(head == NULL)printf("There is no watchpoint!\n");
	else {
		printf("NO   expr       Old Value\n");
		WP* wp = head;
		while(wp != NULL){
			printf("%-8d   %-10s       0x%08x\n",wp->NO,wp->expr,wp->old_val);
			wp = wp->next;
		}
	}
}
