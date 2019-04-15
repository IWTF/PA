#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

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
WP *new_wp(char *e) {
	if (free_ == NULL) {
		printf("no free watchpoint can be used!\n");
		assert(0);
	}

	// 获取该watchpoint的表达式
	strcpy(free_->expr, e);
	printf("watchpoint no is%s\n", e);

	// 获取该watchpoint的值
	bool success = true;
    uint32_t value = expr(e, &success);
    printf("value is %d\n", value);
    free_->old_val = value;

	WP *temp = head ? head->next : NULL;
	head = free_;
	free_ = free_->next;
	head->next = temp;

	return head;
}

void free_wp(int position) {
	if (head == NULL) {
		printf("No watchpoints exist\n");
		return;
	}
	
	WP *p = head;
	WP *pre = head;
	while(p) {
		if (p->NO == position) {
			break;
		}
		p = p->next;
		pre = p; 
	}

	pre->next = p->next;
	p->next = free_->next;
	free_ = p;

	printf("Delete the %s, NO is #%d\n", free_->expr, free_->NO);
}

