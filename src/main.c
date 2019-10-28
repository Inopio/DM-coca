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
   // graph[2] = getGraphFromFile("graphs/assignment-instance/triangle.dot");
   // graph[3] = getGraphFromFile("graphs/assignment-instance/triangle.dot");
   // graph[0] = getGraphFromFile("/net/cremi/sportejoie/espaces/travail/S7/Complexité Calculabilité/assignment/src/G1.dot");
   // graph[1] = getGraphFromFile("/net/cremi/sportejoie/espaces/travail/S7/Complexité Calculabilité/assignment/src/G1.dot");
   // graph[0] = getGraphFromFile("graphs/generic-instances/negative-instances/Taille10_neg/instance1/G1.dot");

    //printGraph(graph[0]);

    
    Z3_ast f = graphsToPathFormula(c,graph,2,2);
    printf("Graph formula : %s\n",Z3_ast_to_string(c,f));

    /*
    Z3_lbool isSat = isFormulaSat(c,f);
    printf("Is sat ? : %d\n",isSat);

    
    Z3_ast f2 = graphsToFullFormula(c,graph,2);
    printf("Graph formula : %s\n",Z3_ast_to_string(c,f2));
    */
    Z3_del_context(c);
}
