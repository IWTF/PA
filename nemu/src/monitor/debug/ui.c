#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

// my own function
static int cmd_si(char *args){
    // TODO: 利用 strtok 读取出 N
    char *arg = strtok(NULL, " ");

    uint64_t N;
    if (arg == NULL) {
      N = 1;
    } else {
      N = strtoul(arg, NULL, 0);   // strtoul将字符串转化为无符号长整形
    }
    // printf("参数为%lu\n", N);

    // TODO: 然后根据 N 来执行对应的 cpu_exec(N) 操作
    if (N == 0xffffffff) {
      cpu_exec(-1);
    } else {
      cpu_exec(N);
    }
    return 0;
}

static int cmd_info(char *args) {
  // 分割字符串
  char *arg = strtok(NULL, " ");

  // 判断子命令类型
  if (strcmp(arg, "r") == 0) {
    for (int i=0; i<8; i++) {
       printf("%s:\t0x%8x\t%u\n", regsl[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
    }
  } else {
    if (strcmp(arg, "w") == 0) {
      printf("尚未实现...\n");
    }
  }
  return 0;
}

/*
 * 将字符转换为数值
 * */
int c2i(char ch) {
        if(isdigit(ch))
                return ch - 48;
 
        if( ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z' )
                return -1;

        if(isalpha(ch))
                return isupper(ch) ? ch - 55 : ch - 87;
 
        return -1;
}
 
/*
 * 功能：将十六进制字符串转换为整型(int)数值
 * */
int hex2dec(char *hex) {
        int len;
        int num = 0;
        int temp;
        int bits;
        int i;
        
        len = strlen(hex);
 
        for (i=2, temp=0; i<len; i++, temp=0) {
                temp = c2i( *(hex + i) );
                bits = (len - i - 1) * 4;
                temp = temp << bits;
 
                num = num | temp;
        }
 
        return num;
}

static int cmd_x(char *args) {
  // 分割字符串，得到起始位置和要读取的次数
  int count = atoi(strtok(NULL, " "));

  char *addr_char = strtok(NULL, " ");
  uint32_t addr = 0;
  for (int i=strlen(addr_char)-1; i>=0; i--) {
    if(addr_char[i] == 'x')
      break;
    int a = addr_char[i] - '0';
    addr += a<<(strlen(addr_char)-i-1)*4;
  }
  printf("addr is %u\n", addr);
  printf("addr hex is %.8x\n", addr);
  return 0;
  // 字符串转数字
  // uint32_t addr = hex2dec(addr_char);

  // 验证获取数据对正确性
  // printf("addr is %u\n", addr);
  // printf("addr hex is %.8x\n", addr);
  // printf("count is %d\n", count);

  // 输入提示信息
  printf("Address\t\tDword block ... Byte sequence\n");
  // 循环使用 vaddr_read 函数来读取内存
  for(int i=0; i<count; i++) {
      uint32_t addr_n = addr + 4*i;
      uint32_t a = vaddr_read(addr_n, 4);
      // uint32_t a = vaddr_read(addr, 1 + i);

      // 每次循环将读取到的数据(Dword block)用 printf 打印出来
      printf("0x%.8x\t 0x%.8x ... ", addr_n, a);

      // 通过循环将Byte sequence打印出来
      for (int j=0; j<4; j++) {
        uint32_t byte_addr = addr_n + j;
        uint32_t byte = vaddr_read(byte_addr, 1);

        printf("%.2x ", byte);
      }

      printf("\n");
  }
  return 0;
} // my function end


static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Single step execution", cmd_si },
  { "info", "Printer status", cmd_info },
  { "x", "Scan memory", cmd_x }

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
