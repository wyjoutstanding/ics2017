#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

//token的类型，只要每种类型对应的整型不同即可，其余用ASCII码表示
enum {
  TK_NOTYPE = 256,//space
  TK_HEX = 254,//16进制
	TK_DEC = 253,//10进制
	TK_REG = 252,//$eax...

  //数值代表优先级，相差10以内表示同优先级
	//===========单目运算===========
	TK_NOT = 240,// !	
	TK_NEG = 239,// neg
	TK_DEREF = 238,//*
  //=============双目运算=============
	//算数运算符
	TK_MUL = 230,// *
	TK_DIV = 229,// /
	
	TK_PLUS = 220,//+
	TK_SUB = 219,//-
  //逻辑运算符
	TK_L = 210,//<
	TK_LE = 209,//<=
	TK_G = 208,//>
	TK_GE = 207,//>=
  
	TK_EQ = 200,//==
	TK_NEQ = 199,//!=
	
	TK_AND = 190,// &&
	TK_OR = 189,// ||
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
 	{"-", TK_SUB},						// sub
	{"\\*", TK_MUL},					// mul
	{"/", TK_DIV},						// div
  {"\\+", TK_PLUS},         // plus
 
 	{"==", TK_EQ},        // equal
 	{"!=", TK_NEQ},       // not equal
  {"&&", TK_AND},       // and
  {"\\|\\|", TK_OR},    // or
  {"!", TK_NOT},        // not
  {"\\*", TK_DEREF},    // decompose reference
	{"!", TK_NOT},        //not
  {"<=", TK_LE},				//pay attention to sequence
	{">=", TK_GE},	
  {"<", TK_L},
  {">", TK_G},

	{"0x[0-9a-fA-F]{1,8}", TK_HEX},//hexadecimal
	{"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip)", TK_REG},//reg
  {"0|([1-9][0-9]*)",TK_DEC},//decimal，考虑到'('可以紧贴，所以将^,$暂时去除


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

    //    Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
//            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
				//==========负号处理=========
				int type = rules[i].token_type;
				if(type == TK_SUB) {
					int t = -1;
				  if(nr_token != 0)	t = tokens[nr_token-1].type;
					//负号判别：-为首个识别字符或前一个token是+,-,*,/,( 中的一个
					if(nr_token == 0 || (t == TK_PLUS || t == TK_SUB || t == TK_MUL || t == TK_DIV || t == '(')){
						type = TK_NEG ;
					//	break;//跳出for循环，即不记录当前值，识别下一个token时再加入
					}
				}
				else if(type == TK_MUL) {
					int t = -1;
				  if(nr_token != 0)	t = tokens[nr_token-1].type;
					//负号判别：-为首个识别字符或前一个token是+,-,*,/,( 中的一个
					if(nr_token == 0 || (t == TK_PLUS || t == TK_SUB || t == TK_MUL || t == TK_DIV || t == '(')){
						type = TK_DEREF ;
					//	break;//跳出for循环，即不记录当前值，识别下一个token时再加入
					}
				}
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

        switch (type) {
					//case TK_HEX: tokens[nr_token].type = TK_HEX; memset(tokens[nr_token].str,'\0',32); strcpy(tokens[nr_token++].str,substr);break;
				  //case TK_DEC: tokens[nr_token].type = TK_DEC; memset(tokens[nr_token].str,'\0',32); strcpy(tokens[nr_token++].str,substr);break;
         // case TK_REG: tokens[nr_token].type = TK_REG; memset(tokens[nr_token].str,'\0',32); strcpy(tokens[nr_token++].str,substr);break;
				  case TK_NOTYPE: break;//空格，不记录
				 // case TK_EQ: tokens[nr_token].type = TK_EQ; memset(tokens[nr_token].str,'\0',32); strcpy(tokens[nr_token++].str,substr);break;
					default:tokens[nr_token].type = type; memset(tokens[nr_token].str,'\0',32); strcpy(tokens[nr_token++].str,substr);break;//+-*/()等符号均用ASCII码表示类型
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
//	for(int j = 0; j < nr_token; j++) {
//		if(tokens[j].str[0] != '\0')Log("type:%d  token:%s",tokens[j].type,tokens[j].str);
//		else Log("type:%d token2:%c",tokens[j].type,tokens[j].type);
//	}	
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
//寻找分割运算符，q传入时是nr_token-1!!!前提：表达式正确
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
  int top = -1,ans = -1;
  for(int i = p; i <= q; i++) {
    if(tokens[i].type == '('){
			top++;
			continue;
		}
		if(tokens[i].type == ')'){
			top--;
			continue;
		}
		if(tokens[i].type < 241 && top == -1) {//数值型&&非括号内
			if(ans == -1)ans = i;//第一次找到operator
			if(tokens[ans].type-tokens[i].type >= -4) ans = i;//先取优先级低&&同优先取最右
		}
	}
	return ans;
}
//求值递归BNF
uint32_t eval(int p, int q) {
  if(p > q){//处理错误表达式如 04
		assert(0);
	}
	else if(p == q) {//10,16进制转换数值
    uint32_t result = 0;
		if(tokens[p].type == TK_DEC)sscanf(tokens[p].str,"%d",&result);
		else if(tokens[p].type == TK_HEX)sscanf(tokens[p].str,"%x",&result);
		else if(tokens[p].type == TK_REG){//reg
			if(strcmp(tokens[p].str+1,"eip") == 0)return cpu.eip;
		  for(int i = 0; i < 8; i++){
				if(strcmp(tokens[p].str+1,regsl[i]) == 0){
          result = reg_l(i);
					break;
				}
			}
		}
		return result;
	}
	else if(check_parentheses(p,q) == true) {
		return eval(p+1, q-1);//去除最外层括号
	}

	else {//找到分界运算符，递归求子表达式
    int op = find_dominated_op(p,q);
		Log("-------op:%d---------\n",op);
		uint32_t val1 = 0;
		int type = tokens[op].type;
		if(type != TK_NEG && type != TK_DEREF && type != TK_NOT)val1 = eval(p,op-1);
    uint32_t val2 = eval(op+1,q);
		switch(tokens[op].type) {
		  case TK_PLUS: return val1 + val2;break;
		  case TK_SUB: return val1 - val2;break;
      case TK_MUL: return val1 * val2;break;
		  case TK_DIV: return val1 / val2;break;
		  case TK_NEG: return -val2;
	    case TK_DEREF: return vaddr_read(val2,4);//指针取引用
	    case TK_NOT: return !val2;
		  case TK_AND: return val1 && val2;
		  case TK_OR : return val1 || val2;
		 	case TK_NEQ: return val1 != val2;
			case TK_EQ : return val1 == val2;
		  case TK_L  : return val1 < val2;
		  case TK_G  : return val1 > val2;
		  case TK_LE : return val1 <= val2;
		  case TK_GE : return val1 >= val2;
		  default: assert(0);
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
  uint32_t ans = eval(0,nr_token-1);
  printf("Expression result Hexadecimal: 0x%08x\t Decimal:%d\n",ans,ans);
  return ans;
}
