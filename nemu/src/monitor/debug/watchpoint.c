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

	// regex_t *re = NULL;
	// if (regcomp(re, "0x[0-9a-f]{1,8}", REG_EXTENDED) != 0) {
	// 	printf("  正则表达式编译失败\n");
	// 	assert(0);
	// }
	// regmatch_t pmatch;
	// if(regexec(re, e, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
	if(e[0]=='0' && e[1]=='x') {
		free_->type = 1;

		char a[20];
		strcpy(a, "$eip == ");
		strcat(a, e);
		strcpy(free_->expr, e);
		printf("  匹配成功\n");

		// 将指令取出，存入str
		// ...
	} else {
		free_->type = 0;

		// 获取该watchpoint的表达式
		strcpy(free_->expr, e);

		// 获取该watchpoint的值
		bool success = true;
	    uint32_t value = expr(e, &success);
	    free_->old_val = value;
	}

	

	WP *temp = head;
	head = free_;
	free_ = free_->next;
	head->next = temp;

	return head;
}

void free_wp(WP *p) {
	WP *pre = head;
	
	if(head == p) {
		head = p->next;
	} else {
		while(pre) {
			if(pre->next == p){
				pre->next = p->next;
				break;
			}
			pre = pre->next;
		}
	}
	p->next = free_->next;
	free_ = p;
}

int set_watchpoint(char *e) {
	WP *cur_w = new_wp(e);

	printf("  Set watchpoint #%d\n", cur_w->NO);
	printf("  expr   \t  = %s\n", cur_w->expr);
	printf("  old value = %d\n", cur_w->old_val);

	return cur_w->NO;
}

int set_breakpoint(char *e) {
	WP *cur_w = new_wp(e);
	return cur_w->NO;
}

bool delete_watchpoint(int position) {
	WP *p = head;

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

	return true;
}

void list_watchpoint() {
	WP *p = head;
	if (p)
		printf("NO  Expr\t\tOld Value\n");
	while(p) {
		if (p->type == 0)
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