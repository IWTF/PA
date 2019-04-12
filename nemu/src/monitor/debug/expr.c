#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  TK_HEX,
  TK_OCT,
  TK_REG,
  TK_BLANK

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},                       // spaces
  {"\\+", '+'},                            // plus
  {"-", '-'},                              // subtract
  {"\\*", '*'},                            // Multiply
  {"/", '/'},                              // except
  {"==", TK_EQ},                           // equal
  {"\\(", '('},                            // Left parenthesis
  {"\\)", ')'},                            // Right parenthesis
  {"0x[0-9a-f]{1,8}", TK_HEX},             // equal
  {"[0-9]+", TK_OCT},                      // equal
  {"\\$[e][a-dsi][xpi]", TK_REG},          // equal \\$[e][a-dsi][xpi] | \\$[E][A-DSI][XPI]
  {"", TK_BLANK},                          // equal

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    /* regcomp是c语言提供的一个编译正则表达式的函数
     * 作用： 
     *   这个函数把指定的正则表达式pattern编译成一种特定的数据格式compiled，这样可以使匹配更有效。
     *   函数regexec 会使用这个数据在目标文本串中进行模式匹配。执行成功返回０。
     */
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      // c语言提供的一个regcomp或regexec执行错误后的一个返回错误字符串的函数
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  /* regmatch_t类型的结构体数组，存放匹配文本串的位置信息。
   * typedef struct
   * {
   *   regoff_t rm_so;
   *   regoff_t rm_eo;
   * } regmatch_t;
   * 成员rm_so 存放匹配文本串在目标串中的开始位置，rm_eo 存放结束位置
   */
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          // default: TODO();s
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // TODO();

  return 0;
}
