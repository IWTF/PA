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

	if(e[0]=='0' && e[1]=='x') {
		free_->type = 1;

		// 构造 $eip == 0x100006表达式，模拟断点
		char a[20];
		strcpy(a, "$eip == ");
		strcat(a, e);
		strcpy(free_->expr, a);
		// 将指令取出，存入str
		
		int b;
		sscanf(e, "%8x", &b);
		unsigned char *bp = (unsigned char *)&b;
		bp[0] = 0xcc;
		printf("byte is:%.2x\n", bp[0]);
	} else {
		free_->type = 0;

		// 获取该watchpoint的表达式
		strcpy(free_->expr, e);
	}

	// 获取该watchpoint的值 && 储存地址e，方便以后输出
	bool success = true;
	uint32_t value = 0;
	if (free_->type == 0) {
		value = expr(e, &success);
	}
    free_->old_val = value;

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
	WP *rehead = NULL;
	int flag = 0;
	WP *temp;
	WP *p = head;
	WP *pre = head;
	while (p) {
		bool success = true;
  		p->new_val = expr(p->expr, &success);

	    if (p->new_val != p->old_val && flag == 0) {
	        rehead = p;
	        flag = 1;
	    } else if (p->new_val == p->old_val && flag == 1) {
	    	temp = head->next;
	    	head = p;
	    	pre->next = p->next;
	    	head->next = temp;
	    }
	    pre = p;
	    p = p->next;
	}
	return rehead;
}