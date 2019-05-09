#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

make_EHelper(call);
make_EHelper(push);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(pop);
make_EHelper(ret);

make_EHelper(lea);//frame is done

make_EHelper(add);
make_EHelper(or);//call adc
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(and);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(cmp);

make_EHelper(inc);
make_EHelper(dec);//call adc
//make_EHelper(call);
make_EHelper(call_rm);
make_EHelper(jmp);
make_EHelper(jmp_rm);
make_EHelper(pusha);

//make_EHelper(xchg);
make_EHelper(nop);

//make_EHelper(2byte_esc);
make_EHelper(setcc);
make_EHelper(movzx);
make_EHelper(test);
make_EHelper(jcc);
make_EHelper(sar);
make_EHelper(shr);
make_EHelper(shl);
make_EHelper(inc);
make_EHelper(dec);
