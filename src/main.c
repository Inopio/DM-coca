#include <stdio.h>
#include <z3.h>
#include "Solving.h"
#include "Z3Tools.h"
#include <Graph.h>
#include <Parsing.h>

int main (){
    Z3_context c = makeContext();
    Z3_ast a = getNodeVariable(c,1,2,3,4);
    
    Graph graph[2];
    //graph[0] = getGraphFromFile("graphs/assignment-instance/G1.dot");
    //graph[0] = getGraphFromFile("graphs/assignment-instance/triangle.dot");
    //graph[0] = getGraphFromFile("graphs/assignment-instance/triangle.dot");
    //graph[0] = getGraphFromFile("graphs/generic-instances/positive-instances/Taille10_pos/instance1/G1.dot");
    graph[0] = getGraphFromFile("graphs/generic-instances/positive-instances/Taille10_pos/instance1/G1.dot");
    graph[1] = getGraphFromFile("graphs/generic-instances/positive-instances/Taille10_pos/instance1/G1.dot");
   // graph[0] = getGraphFromFile("graphs/generic-instances/negative-instances/Taille10_neg/instance2/G3.dot");   // longeur 0 oui

    printGraph(graph[1]);

    /*  OK
    Z3_ast f = graphsToPathFormula(c,graph,2,2);
    printf("Graph formula from : %s\n",Z3_ast_to_string(c,f));
    

    Z3_lbool isSat = isFormulaSat(c,f);
    printf("Is sat ? : %d\n",isSat);

    */
    
    //Z3_ast f2 = graphsToFullFormula(c,graph,2);
    //int isSat2 = isFormulaSat(c,f2);
    //printf("Is sat ? : %d\n",isSat2);
    Z3_ast f = graphsToPathFormula(c, graph,2, 6);
    int isSat2 = isFormulaSat(c,f);
    printf("Is sat ? : %d\n",isSat2);
    
    if(isSat2 ==1){
        //Z3_model model = getModelFromSatFormula(c,f2);
        Z3_model model = getModelFromSatFormula(c,f);
        //int k = getSolutionLengthFromModel(c,model,graph);
        //printf("Longueur commune %d\n",2);
        printPathsFromModel(c,model,graph,2,6);
        //Z3_del_model(c,model);
        createDotFromModel(c,model,graph,2,6,"toto");
    }
    
    
    Z3_del_context(c);
}