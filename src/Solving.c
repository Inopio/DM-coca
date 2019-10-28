#include "Graph.h"
#include <z3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Z3Tools.h"

#define min(a,b) (a<=b?a:b)

Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node){
    
    Z3_sort sort;
    Z3_ast x,f;
    
    int t[4] = {number,position,k,node};
    char s[1024];

    sprintf(s, "X %d,%d,%d,%d", number,position,k,node);
   
    x = mk_bool_var(ctx, s);
    return x;
}

Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength){
    Z3_ast x1,x2;                       //variables construction des clauses
    Z3_ast negX1,negX2;                 //négation des variables
    Z3_ast f,f1, f2, f3, f4, f5, tmp;   //variables stockage des formules
    Z3_ast args[10];                    //tableaux de construction des formules
    Z3_ast save[1000];
    Z3_ast save2[1000];
    Z3_ast fargs[10];

   // negX1 = Z3_mk_not(ctx,x);

   Z3_ast savephi1[2];
   //Phi 1

    for(int i=0; i<numGraphs; i++){
        orderG(graphs[i]);
        x1 = getNodeVariable(ctx, i, 0, pathLength, 0);
        x2 = getNodeVariable(ctx, i, pathLength, pathLength, pathLength);
        args[0] = x1;
        args[1] = x2;
        f1 = Z3_mk_and(ctx, 2, args);
        
        if(i == 0){
        savephi1[0] = f1;
        }else{
        args[0] = savephi1[0];
        args[1] = f1;
            f1 = Z3_mk_and(ctx, 2, args);
            savephi1[0] = f1;
        }
    }

  //Phi 2
    for(int i=0; i<numGraphs; i++){
        for(int j=0; j<pathLength; j++){
            for(int q =0; q<orderG(graphs[i]); q++){
                for(int r =0; r<orderG(graphs[i]); r++){
                    if(q != r)
                        x1 = getNodeVariable(ctx, i, j, pathLength, q);
                        x2 = getNodeVariable(ctx, i, j, pathLength, r);

                        args[0] = Z3_mk_not(ctx,x1);
                        args[1] = Z3_mk_not(ctx,x2);

                        tmp = Z3_mk_or(ctx, 2, args);
                        save[0] = tmp;
                        if(save2[0] == NULL){
                                f2 = tmp;
                                save2 [0] = tmp;
                                save2[1] = f2;
                        }else{
                                save2[1] = tmp;
                                f2=Z3_mk_and(ctx, 2, save2);
                                save2[0] = f2;
                        }
                }
            }
        }
    } 
    
    //Phi 3
   Z3_ast savephi3[2];
    for(int i = 0; i < numGraphs; i++){
        for(int q = 0; q < orderG(graphs[i]); q++){
            for(int j = 0; j < pathLength; j++){
                x1 = getNodeVariable(ctx, i, j, pathLength, q);
                if(savephi3[0]==NULL){
                    f3 = x1;
                    savephi3[0] = x1;
                    savephi3[1] = f3;
                }else{
                    savephi3[1] = x1;
                    f3 = Z3_mk_and(ctx, 2, savephi3);
                    savephi3[0] = f3;
                }
            }
        }
    }
    
    //Phi 4

    Z3_ast savephi4[2];
    for(int i = 0; i < numGraphs; i++){
        for(int q = 0; q < orderG(graphs[i]); q++){
            for(int j = 0; j < pathLength; j++){
                for(int l = 0; l < pathLength; l++){
                    if(j != l){
                        x1 = getNodeVariable(ctx, i, j, pathLength, q);
                        x2 = getNodeVariable(ctx, i, l, pathLength, q);
                        args[0] = Z3_mk_not(ctx,x1);
                        args[1] = Z3_mk_not(ctx,x2);
                        tmp = Z3_mk_or(ctx, 2, args);
                        if(savephi4[0] == NULL){
                                f4 = tmp;
                                savephi4[0] = tmp;
                        }else{
                                savephi4[1] = tmp;
                                f4=Z3_mk_and(ctx, 2, savephi4);
                                savephi4[0] = f4;
                        }
                    }
                }
            }
        }
    }
    
    
    Z3_ast args1[10];
    Z3_ast save3[10];
    
    //Phi 5
    for(int i=0; i<numGraphs; i++){
        for(int j=0; j<pathLength-1; j++){
            for(int q =0; q<orderG(graphs[i]); q++){
                for(int r =0; r<orderG(graphs[i]); r++){
                    if(isEdge(graphs[i],q,r)){
                        args1[0] = getNodeVariable(ctx, i, j, pathLength, q);
                        args1[1] = getNodeVariable(ctx, i, j+1, pathLength, r);

                        tmp = Z3_mk_and(ctx, 2, args1);
                        if(save3[0] == NULL){
                                f5 = tmp;
                                save3[0] = tmp;
                                save3[1] = f5;
                        }else{
                                save3[0] = tmp;
                                f3=Z3_mk_and(ctx, 2, save3);
                                save3[1] = f5;
                        }
                    }
                }
            }
        }
    }
    
    fargs[0] = f1;
    //fargs[1] = f2;
    fargs[1] = f3;
    fargs[2] = f4;
    fargs[3] = f5;
    f = Z3_mk_and(ctx,4,fargs);
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
Z3_ast graphsToFullFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs){
    Z3_ast f;
    //Pour obtenir la limite de k, majorée par le plus petit des graphes 
    int min_size;
    
    min_size = orderG(graphs[0]);
    for(int i = 1; i < numGraphs; i++){
        if(min_size < orderG(graphs[i])){
            min_size = orderG(graphs[i]);
        }
    }
    
    printf("minsize %d\n",min_size);
    for(int j=1; j<min_size; j++){
        f = graphsToPathFormula(ctx,graphs,numGraphs,j);   //longueur commune de taille j
        if(isFormulaSat(ctx,f)) { //si on a une longueur commune de taille j (f satisfiable)
           break;
        }     
    }
    return f;
}

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