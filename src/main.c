#include <stdio.h>
#include <z3.h>
#include "Solving.h"
#include "Z3Tools.h"

int main (void){
    return 0;
    Z3_context c = makeContext();
    Z3_ast a = getNodeVariable(c,1,2,3,4);
    printf(Z3_ast_to_string(c,a));
}