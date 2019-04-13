#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

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
  // {"", TK_BLANK},                          // equal

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
        if (substr_len > 32) {
          printf("data is too long to calculate\n");
          return false;
        }

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          default: 
            tokens[nr_token].type = rules[i].token_type;
            // strcpy(tokens[nr_token].str, substr_start);
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            // printf("保存的字符串为:%s\n\n", tokens[nr_token].str);
            nr_token++;
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

bool check_parentheses(int p, int q) {
  if(!strcmp(tokens[p].str, "(") && !strcmp(tokens[q].str, ")")) {
    uint32_t num = 0;
    for (int i=p+1; i<q; i++) {
      if(strcmp(tokens[i].str, "(") == 0) {
        num--;
      }
      if (strcmp(tokens[i].str, ")") == 0) {
        num++;
      }
      if(num == 1) {
        printf("Parentheses do not match\n");
        return false;
      }
    }
    return true;
  }
  return false;
}

uint32_t find_dominated_op(int p, int q) {
  int temp = -1;
  int flag = 0;
  for (int i=p; i<=q; i++) {
    if(tokens[i].type == TK_HEX || tokens[i].type == TK_OCT || (flag&&tokens[i].type != ')')) 
      continue;
    else if(tokens[i].type == '(')
      flag = 1;
    else if(tokens[i].type == ')')
      flag = 0;
    else {
      if(temp == -1 || tokens[temp].type == '*' || tokens[temp].type == '/') {
        temp = i;
      } else if(tokens[i].type == '+' || tokens[i].type == '-') {
        temp = i;
      }
    }
  }
  return temp;
}

uint32_t eval(int p, int q) {
    if (p > q) {
        assert(0);
    }
    else if (p == q) {
        /* Single token.
        * For now this token should be a number.
        * Return the value of the number.
        */
      int result = 0;

      if (tokens[p].type == TK_HEX)  // 十六进制数
        sscanf(tokens[p].str, "%x", &result);
      else if (tokens[p].type == TK_OCT)  // 10进制数
        result = atoi(tokens[p].str);
      else if (tokens[p].type == TK_REG)  // 寄存器
        for (int i=0; i<8; i++) {
          char nreg[5];
          strcpy(nreg, tokens[p].str+1);
          if (strcmp(regsl[i], nreg) == 0) {
            // printf("匹配reg is：%s\n", regsl[i]);
            result = cpu.gpr[i]._32;
            break;
          }
        }

      return result;
    }
    else if (check_parentheses(p, q) == true) {
        /* The expression is surrounded by a matched pair of parentheses.
        * If that is the case, just throw away the parentheses.
        */
      // printf("脱括号\n");
      return eval(p + 1, q - 1);
    }
    else {
      /* We should do more things here. */
      uint32_t op = find_dominated_op(p, q);
      // printf("dominated operation position at:%d\n", op);  // 判断匹配位置是否正确
      if ((op-p)%2 == 0) {
        if(tokens[op].type != '-') {
          printf("Operator error\n");
          assert(0);
        }
        int negative = 0;
        if (tokens[op+1].type == TK_OCT || tokens[op+1].type == TK_HEX) {
          tokens[op].type = tokens[op+1].type;
          strcat(tokens[op].str, tokens[op+1].str);
          sscanf(tokens[op].str, "%x", &negative);
          printf("负数为：%d\n", negative);
          return eval(p, op);
        } else if (tokens[op+1].type == TK_REG) {
          for (int i=0; i<8; i++) {
            char nreg[5];
            strcpy(nreg, tokens[p].str+1);
            if (strcmp(regsl[i], nreg) == 0) {
              // printf("匹配reg is：%s\n", regsl[i]);
              negative = cpu.gpr[i]._32;
              negative = ~negative+1;
              tokens[op].type = TK_OCT;
              // itoa(negative, tokens[op].str, 10);
              printf("负数为：%d\n", negative);
              // return eval(p, op);
            }
          }
        } else {
          printf("Operator error\n");
          assert(0);
        }
      }
      uint32_t val1 = eval(p, op - 1);
      uint32_t val2 = eval(op + 1, q);
      switch (tokens[op].type) {
          case '+': return val1 + val2;
          case '-': return val1 - val2;
          case '*': return val1 * val2;
          case '/': return val1 / val2;
          default: assert(0);
      }
      return 0;
    }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */

  return eval(0, nr_token - 1);
}
