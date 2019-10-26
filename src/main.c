#include <stdio.h>
#include <z3.h>
#include "Solving.h"
#include "Z3Tools.h"
#include <Graph.h>
#include <Parsing.h>

int main (void){
    Z3_context c = makeContext();
    Z3_ast a = getNodeVariable(c,1,2,3,4);
    printf("Formula : %s\n",Z3_ast_to_string(c,a));


      FILE* file = fopen("square.dot","r");
    if(file == NULL){
        printf("test.\n");
    }
    Graph graph[2];
    graph[0] = getGraphFromFile("src/G1.dot");
    graph[1] = getGraphFromFile("src/square.dot");

    printf("Graph formula : %s\n",Z3_ast_to_string(c,graphsToPathFormula(c,graph,2,2)));
}
