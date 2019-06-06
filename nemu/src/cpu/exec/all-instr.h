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
make_EHelper(movsx);
make_EHelper(test);
make_EHelper(jcc);
make_EHelper(sar);
make_EHelper(shr);
make_EHelper(shl);
make_EHelper(inc);
make_EHelper(dec);
make_EHelper(neg);
make_EHelper(not);

make_EHelper(jmp);

make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(mul);
make_EHelper(div);
make_EHelper(idiv);

make_EHelper(cltd);
make_EHelper(cwtl);

make_EHelper(leave);

make_EHelper(in);
make_EHelper(out);
make_EHelper(rol);

make_EHelper(int);
