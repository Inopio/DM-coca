#include "Graph.h"
#include <z3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Z3Tools.h"
/**
 * @brief Generates a formula consisting of a variable representing the fact that @p node of graph number @p number is at position @p position of an accepting path.

 * @param ctx The solver context.
 * @param number The number of the graph.
 * @param position The position in the path.
 * @param k The mysterious k from the subject of this assignment.
 * @param node The node identifier.
 * @return Z3_ast The formula.
 */


Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node){
    
    Z3_sort sort;
    Z3_ast x,f;
    
    int t[4] = {number,position,k,node};
    char * s;

    sprintf(s, "X %d,%d,%d,%d", number,position,k,node);
   
    x = mk_bool_var(ctx, s);
    
    Z3_ast args[1] = {x};
    f = Z3_mk_and(ctx, 1, args); 

    return x;
}


/**
 * @brief Generates a SAT formula satisfiable if and only if all graphs of @p graphs contain an accepting path of length @p pathLength.
 * 
 * @param ctx The solver context.
 * @param graphs An array of graphs.
 * @param numGraphs The number of graphs in @p graphs.
 * @param pathLength The length of the path to check.
 * @return Z3_ast The formula.
 */
Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength){
    Z3_ast x1,x2;
    Z3_ast negX1,negX2;
    Z3_ast f,tmp;
    Z3_ast *args;

   // negX1 = Z3_mk_not(ctx,x);

   //Phi 1
    for(int i=0; i<numGraphs; i++){
        x1 = getNodeVariable(ctx, i, 2, pathLength, 0);
        x2 = getNodeVariable(ctx, i, 2, pathLength, pathLength);
        args[0] = x1;
        args[1] = x2;
        f = Z3_mk_and(ctx, 2, args);

    }
/*
    //Phi 2
    for(int i=0; i<numGraphs; i++){
        for(int j=0; j<pathLength; j++){
            x1 = getNodeVariable(ctx, i, 2, pathLength, 0);
            x2 = getNodeVariable(ctx, i, 2, pathLength, pathLength);
            args[0] = Z3_mk_not(ctx,x1);
            args[1] = Z3_mk_not(ctx,x2);
            tmp = Z3_mk_or(ctx, 2, args);
            args[0] = Z3_mk_not(ctx,tmp);
            args[1] = Z3_mk_not(ctx,f);
            f = Z3_mk_and(ctx, 2, args);
        }
    }

    //Phi 3
    for(int i=0; i<numGraphs; i++){
        for(int j=0; j<pathLength; j++){
            x1 = getNodeVariable(ctx, i, 2, pathLength, j);
            args[0] = Z3_mk_not(ctx,f);
            args[1] = Z3_mk_not(ctx,x1);
            f = Z3_mk_and(ctx, 2, args);
        }
    }
*/
    return f;

}

/**
 * @brief Generates a SAT formula satisfiable if and only if all graphs of @p graphs contain an accepting path of common length.
 * 
 * @param ctx The solver context.
 * @param graphs An array of graphs.
 * @param numGraphs The number of graphs in @p graphs.
 * @return Z3_ast The formula.
 */
Z3_ast graphsToFullFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs);

/**
 * @brief Gets the length of the solution from a given model.
 * 
 * @param ctx The solver context.
 * @param model A variable assignment.
 * @param graphs An array of graphs.
 * @return int The length of a common simple accepting path in all graphs from @p graphs.
 */ 
int getSolutionLengthFromModel(Z3_context ctx, Z3_model model, Graph *graphs);

/**
 * @brief Displays the paths of length @p pathLength of each graphs in @p graphs described by @p model.
 * 
 * @param ctx The solver context.
 * @param model A variable assignment.
 * @param graphs An array of graphs.
 * @param numGraph The number of graphs in @p graphs.
 * @param pathLength The length of path.
 */
void printPathsFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength);

/**
 * @brief Creates the file ("%s-l%d.dot",name,pathLength) representing the solution to the problem described by @p model, or ("result-l%d.dot,pathLength") if name is NULL.
 * 
 * @param ctx The solver context.
 * @param model A variable assignment.
 * @param graphs An array of graphs.
 * @param numGraph The number of graphs in @p graphs.
 * @param pathLength The length of path.
 * @param name The name of the output file.
 */
void createDotFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength, char* name);