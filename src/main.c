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
    
    Graph graph[10];
    graph[0] = getGraphFromFile("graphs/assignment-instance/triangle.dot");
    graph[1] = getGraphFromFile("graphs/assignment-instance/triangle.dot");
   // graph[2] = getGraphFromFile("graphs/generic-instances/positive-instances/Taille10_pos/instance1/G3.dot");
    //graph[3] = getGraphFromFile("graphs/generic-instances/positive-instances/Taille10_pos/instance1/G4.dot");

    printf("Graph formula : %s\n",Z3_ast_to_string(c,graphsToPathFormula(c,graph,2,2)));
}
