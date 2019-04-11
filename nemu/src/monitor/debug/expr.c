#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

//token的类型，只要每种类型对应的整型不同即可，其余用ASCII码表示
enum {
  TK_NOTYPE = 256,//space
 	TK_EQ = 255,//==
	TK_HEX = 254,//16进制
	TK_DEC = 253,//10进制
	TK_REG = 252,//$eax...

  /* TODO: Add more token types */

};
//检测不同类型的规则
static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more ruleis.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal

	{"0x[0-9A-F]{1,8}", TK_HEX},//hexadecimal
	{"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi)", TK_REG},//reg
  {"0|([1-9][0-9]*)",TK_DEC},//decimal，考虑到'('可以紧贴，所以将^,$暂时去除

	{"-", '-'},						// sub
	{"\\*", '*'},					// mul
	{"/", '/'},						// div
  
	{"\\(", '('},					//brakets is special in regex !!!
	{"\\)", ')'},
};
//规则条数
#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )
//regex.h使用必备
static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
//对所有规则编译并检查
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: at:%d\n %s\n%s",i, error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;
//提取并按顺序存储token于tokens
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
//    Log("e_pos:%c\n",e[position]);
		for (i = 0; i < NR_REGEX; i ++) {
     if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
				//取子串
				char* substr = (char*)malloc(32*sizeof(char));
				strncpy(substr,substr_start,substr_len);
				substr[substr_len] = '\0';
				assert(substr_len <= 32);//溢出时提示
			//	if(substr_len >= 32)Log("substr_len is overflow 32!!in make_token");//以后处理
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
					case TK_HEX: tokens[nr_token].type = TK_HEX; memset(tokens[nr_token].str,'\0',32); strcpy(tokens[nr_token++].str,substr);break;
				  case TK_DEC: tokens[nr_token].type = TK_DEC; memset(tokens[nr_token].str,'\0',32); strcpy(tokens[nr_token++].str,substr);break;
          case TK_REG: tokens[nr_token].type = TK_REG; memset(tokens[nr_token].str,'\0',32); strcpy(tokens[nr_token++].str,substr);break;
				  case TK_NOTYPE: break;//空格，不记录
				  case TK_EQ: tokens[nr_token].type = TK_EQ; memset(tokens[nr_token].str,'\0',32); strcpy(tokens[nr_token++].str,substr);break;
					default:tokens[nr_token].type = rules[i].token_type; tokens[nr_token++].str[0] = '\0';break;//+-*/()等符号均用ASCII码表示类型
				}
        break;

		 }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  //Printing tokens to test 
	for(int j = 0; j < nr_token; j++) {
		if(tokens[j].str[0] != '\0')Log("type:%d  token:%s",tokens[j].type,tokens[j].str);
		else Log("type:%d token2:%c",tokens[j].type,tokens[j].type);
	}	
  return true;
}
//检查括号匹配合法&&最外层是否有括号:p,q为起止位置
bool check_parentheses(int p, int q) {
  bool isMatch;//合法，最外层有括号
	int top = -1;//判断是否合法
	if(tokens[p].type == 40)isMatch = true;//第一个是否为(
	else isMatch = false;
	//遍历所有token
	for(int i = p; i < q+1; i++) {
		assert(top >= -1);//判断错误，处理左右括号数目相等的情况
		if(i != p && i < q+1 && top == -1)isMatch = false;//最外层无括号
	  if(tokens[i].type == 40)top++;
		else if(tokens[i].type == 41)top--;
	}
	assert(top == -1);//不合法报错，直接停止
	return isMatch;//合法表达式，判断是否有最外层括号
}

int pri[256][256]={0};//优先级a-b>,<=0
//寻找分割运算符，q传入时是nr_token-1!!!
int find_dominated_op(int p, int q) {
  //为什么只能在函数内部赋值？？
	pri['+']['+'] = 0;
	pri['+']['-'] = 0;
	pri['+']['*'] = -1;
	pri['+']['/'] = -1;

	pri['-']['+'] = 0;
	pri['-']['-'] = 0;
	pri['-']['*'] = -1;
	pri['-']['/'] = -1;

	pri['*']['+'] = 1;
	pri['*']['-'] = 1;
	pri['*']['*'] = 0;
	pri['*']['/'] = 0;

	pri['/']['+'] = 1;
	pri['/']['-'] = 1;
	pri['/']['*'] = 0;
	pri['/']['/'] = 0;
  int top = -1,ans = p+1;
  for(int i = p+2; i <= q; i++) {
    if(tokens[i].type == '('){
			top++;
			continue;
		}
		if(tokens[i].type == ')'){
			top--;
			continue;
		}
		if(tokens[i].type < 150 && top == -1) {//数值型&&非括号内
			if(pri[tokens[ans].type][tokens[i].type] >= 0) ans = i;//先取优先级低&&同优先取最右
		}
	}
	return ans;
}
//求值递归BNF
uint32_t eval(int p, int q) {
  if(p > q){//处理错误表达式如 04
		assert(0);
	}
	else if(p == q) {
		uint32_t result;
		if(tokens[p].type == TK_DEC)sscanf(tokens[p].str,"%u",&result);
		else if(tokens[p].type == TK_HEX)sscanf(tokens[p].str,"%x",&result);
		return result;
	}
	else if(check_parentheses(p,q) == true) {
		return eval(p+1, q-1);
	}
	else {
    int op = find_dominated_op(p,q);
		Log("-------op:%d---------\n",op);
		uint32_t val1 = eval(p,op-1);
    uint32_t val2 = eval(op+1,q);
		switch(tokens[op].type) {
		  case '+': return val1 + val2;break;
		  case '-': return val1 - val2;break;
      case '*': return val1 * val2;break;
		  case '/': return val1 / val2;break;
		  default: assert(0);
		//	return 1;
		}
  }
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
//  if(check_parentheses(0,nr_token))Log("Check Result: expression is contained by brackets!\n");
//	else Log("Check Result: expression is not contained by brackets!\n");
//  Log("Total num: %d    Find the dominant pos: %d\n",nr_token,find_dominated_op(0,nr_token-1));
	//  Log("to do before\n");
  /* TODO: Insert codes to evaluate the expression. */
//  TODO();
//  Log("to do after\n");
  Log("Expression result: %u\n",eval(0,nr_token-1));
  return 0;
}
