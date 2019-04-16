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

	// 获取该watchpoint的值
	bool success = true;
    uint32_t value = expr(e, &success);
    free_->old_val = value;

	WP *temp = head;
	head = free_;
	free_ = free_->next;
	head->next = temp;

	return head;
}

void free_wp(WP *wp) {
	WP *p = head;
	if (head == wp) {
		head = wp->next;
	} else {
		while (p) {
			if (p == wp) {
				p = wp->next;
				break;
			}
			p = p->next;
		}
	}
	wp->next = free_->next;
	free_ = wp;
}

int set_watchpoint(char *e) {
	WP *cur_w = new_wp(e);

	printf("Set watchpoint #%d\n", cur_w->NO);
	printf("expr   \t  = %s\n", cur_w->expr);
	printf("old value = %d\n", cur_w->old_val);

	return cur_w->NO;
}

bool delete_watchpoint(int position) {
	// WP *temp;
	WP *p = head;
	// WP *pre = head;

	if (head == NULL) {
		return false;
	}
	while(p) {
		if (p->NO == position) {
			break;
		}
		p = p->next;
		// pre = p; 
	}

	free_wp(p);
	// temp = p->next;
	// p->next = free_->next;
	// free_ = p;
	// if (head == p)
	// 	head = NULL;
	// else {
	// 	pre->next = temp;
	// }

	// printf("Delete the %s, NO is #%d\n", free_->expr, free_->NO);
	return true;
}

void list_watchpoint() {
	WP *p = head;
	if (p)
		printf("NO  Expr\t\tOld Value\n");
	while(p) {
		printf("%-4d%s\t\t%d\n", p->NO, p->expr, p->old_val);
		p = p->next;
	}
}

WP *scan_watchpoint() {
	WP *p = head;
    while (p) {
      bool success = true;
      uint32_t value = expr(p->expr, &success);
      // printf("%s is %d\n", p->expr, value);
      p->new_val = value;
      if (p->new_val != p->old_val) {
        return p;
      }
      p = p->next;
    }
    return NULL;
}