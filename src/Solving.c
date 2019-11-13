#include "Graph.h"
#include <z3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Z3Tools.h"

#define min(a,b) (a<=b?a:b)

Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node){
    Z3_ast x;
    
    int t[4] = {number,position,k,node};
    char s[1024];

    sprintf(s, "X %d,%d,%d,%d", number,position,k,node);
   
    x = mk_bool_var(ctx, s);
    return x;
}

Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength){

    if(pathLength == 0){ // Non satisfiable pour le moment
        return Z3_mk_false(ctx);
    }

    for(int i=0; i<numGraphs; i++){
        if(sizeG(graphs[i]) <= pathLength || orderG(graphs[i]) <= pathLength){
            return Z3_mk_false(ctx);
        }
    }
    int s,t;                            //indices des sommets s et t du graphe
    Z3_ast x1,x2;                       //variables construction des clauses
    Z3_ast negX1,negX2;                 //négation des variables
    Z3_ast f, f1, f2, f3, f4, f5, tmp;  //variables stockage des formules
    Z3_ast args[2];                     //tableaux de construction des formules

   //Phi 1
    Z3_ast * savephi1 = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    savephi1[0] = NULL;
    for(int i = 0; i<numGraphs; i++){
        for(s=0;s<orderG(graphs[i]) && !isSource(graphs[i],s);s++);
        for(t=0;t<orderG(graphs[i]) && !isTarget(graphs[i],t);t++);
        x1 = getNodeVariable(ctx, i, 0, pathLength, s);
        x2 = getNodeVariable(ctx, i, pathLength, pathLength, t);

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
    free(savephi1);
    
    //Phi 2
    Z3_ast * savephi2 = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    savephi2[0] = NULL;
    for(int i=0; i<numGraphs; i++){
        for(int j=0; j<=pathLength; j++){
            for(int q =0; q<orderG(graphs[i]); q++){
                for(int r =0; r<orderG(graphs[i]); r++){
                    if(q != r){
                        x1 = getNodeVariable(ctx, i, j, pathLength, q);
                        x2 = getNodeVariable(ctx, i, j, pathLength, r);
                        args[0] = Z3_mk_not(ctx,x1);
                        args[1] = Z3_mk_not(ctx,x2);

                        tmp = Z3_mk_or(ctx, 2, args);
                        if(savephi2[0] == NULL){
                            f2 = tmp;
                            savephi2[0] = f2;
                        }else{
                            savephi2[1] = tmp;
                            f2 = Z3_mk_and(ctx, 2, savephi2);
                            savephi2[0] = f2;
                        }
                    }
                }
            }
        }
    }
    free(savephi2);

    //Phi 3
    Z3_ast * savephi3_or = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    savephi3_or[0] = NULL;
    Z3_ast * savephi3_and = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    savephi3_and[0] = NULL;
   for(int i = 0; i < numGraphs; i++){
       for(int j = 0; j <= pathLength; j++){
           Z3_ast f3_or;
           for(int q = 0; q < orderG(graphs[i]); q++){
               x1 = getNodeVariable(ctx, i, j, pathLength, q);
               if(savephi3_or[0]==NULL){
                   f3_or = x1;
                   savephi3_or[0] = f3_or;
               }else{
                   savephi3_or[1] = x1;
                   f3_or = Z3_mk_or(ctx, 2, savephi3_or);
                   savephi3_or[0] = f3_or;
               }
           }
           savephi3_or[0] = NULL;
           savephi3_or[1] = NULL;
            if(savephi3_and[0] == NULL){
                savephi3_and[0] = f3_or;
                f3 = f3_or;
            }else{
                savephi3_and[1] = f3_or;
                f3 = Z3_mk_and(ctx, 2, savephi3_and);
                savephi3_and[0] = f3;
            }
       }
   }
   free(savephi3_or);
   free(savephi3_and);

    //Phi 4
    Z3_ast * savephi4 = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    savephi4[0] = NULL;
    for(int i = 0; i < numGraphs; i++){
        for(int q = 0; q < orderG(graphs[i]); q++){
            for(int j = 0; j <= pathLength; j++){
                for(int l = 0; l <= pathLength; l++){
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
    free(savephi4);

    //Phi 5
    Z3_ast * args1 = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    Z3_ast * savephi5_or = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    Z3_ast * savephi5_and = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    savephi5_or[0] = NULL;
    savephi5_and[0] = NULL;
        for(int i=0; i<numGraphs; i++){
            for(s=0;s<orderG(graphs[i]) && !isSource(graphs[i],s);s++);
            for(t=0;t<orderG(graphs[i]) && !isTarget(graphs[i],t);t++);
            x1 = getNodeVariable(ctx, i, 0, pathLength, s);
            x2 = getNodeVariable(ctx, i, pathLength, pathLength, t);
            if(pathLength > 1){
                for(int j=0; j<pathLength; j++){
                    Z3_ast f5_or;
                    for(int q =0; q<orderG(graphs[i]); q++){
                        for(int r =0; r<orderG(graphs[i]); r++){
                            if(isEdge(graphs[i],q,r)){
                                args1[0] = getNodeVariable(ctx, i, j, pathLength, q);
                                args1[1] = getNodeVariable(ctx, i, j+1, pathLength, r);
                                tmp = Z3_mk_and(ctx, 2, args1);
                                if(savephi5_or[0] == NULL){
                                    f5_or = tmp;
                                    savephi5_or[0] = f5_or;
                                }else{
                                    savephi5_or[1] = tmp;
                                    f5_or = Z3_mk_or(ctx, 2, savephi5_or);
                                    savephi5_or[0] = f5_or;
                                }
                            }
                        }
                    }
                    savephi5_or[0] = NULL;
                    savephi5_or[1] = NULL;
                    if(savephi5_and[0] == NULL){
                        savephi5_and[0] = f5_or;
                        f5 = f5_or;
                    }else{
                        savephi5_and[1] = f5_or;
                        f5 = Z3_mk_and(ctx, 2, savephi5_and);
                        savephi5_and[0] = f5;
                    }
                }
            }else{
                if(isEdge(graphs[i],s,t)){
                    x1 = getNodeVariable(ctx, i, 0, pathLength, s);
                    x2 = getNodeVariable(ctx, i, pathLength, pathLength, t);
                    tmp = Z3_mk_and(ctx, 2, args1);
                    if(savephi5_and[0] == NULL){
                            f5 = tmp;
                            savephi5_and[0] = tmp;
                    }else{
                            savephi5_and[0] = tmp;
                            f5=Z3_mk_and(ctx, 2, savephi5_and);
                            savephi5_and[1] = f5;
                    }
                }else{
                    return Z3_mk_false(ctx);
                }
            }
        }
        free(args1);
        free(savephi5_or);
        free(savephi5_and);

    Z3_ast fargs[5];

    fargs[0] = f1;
    fargs[1] = f2;
    fargs[2] = f3;
    fargs[3] = f4;
    fargs[4] = f5;
    
    f = Z3_mk_and(ctx,5,fargs);
    return  f;
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
    Z3_ast f = Z3_mk_false(ctx);
    Z3_ast tmp;

    //Pour obtenir la limite de k, majorée par le plus petit des graphes 
    int min_size;
    
    min_size = orderG(graphs[0]);
    for(int i =0; i < numGraphs; i++){
        if(min_size > orderG(graphs[i])){
            min_size = orderG(graphs[i]);
        }
    }
    
    for(int j=0; j<min_size; j++){
        tmp = graphsToPathFormula(ctx,graphs,numGraphs,j);   //longueur commune de taille j
        if(isFormulaSat(ctx,tmp)==1) { //si on a une longueur commune de taille j (f satisfiable)
            f = tmp;
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
int getSolutionLengthFromModel(Z3_context ctx, Z3_model model, Graph *graphs){

    for(int k=1; k<orderG(graphs[0]); k++){
        for(int j=0; j<orderG(graphs[0]); j++){
            for(int q=0; q<orderG(graphs[0]); q++){
                if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,0,j,k,q)) == true){
                    return k;
                }
            }
        }
    }
    
    return 0;
}

/**
 * @brief Displays the paths of length @p pathLength of each graphs in @p graphs described by @p model.
 * 
 * @param ctx The solver context.
 * @param model A variable assignment.
 * @param graphs An array of graphs.
 * @param numGraph The number of graphs in @p graphs.
 * @param pathLength The length of path.
 */
void printPathsFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength){
    //printf("Model = %s\n",Z3_model_to_string(ctx,model));
    int s,t;
    for(int i=0; i<numGraph; i++){
        printf("Path from graph %d: ",i);

        for(s=0;s<orderG(graphs[i]) && !isSource(graphs[i],s);s++); //Find the source s in the graph i
        for(t=0;t<orderG(graphs[i]) && !isTarget(graphs[i],t);t++); //Find the target t in the graph i

        //Source
        if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,0,pathLength,s)) == true){
            printf("%s ",getNodeName(graphs[i],s),i,pathLength);
        }
        //Build the path between the source and the destination
        for(int j=1; j<pathLength; j++){
            for(int q=0; q<orderG(graphs[i]); q++){
                if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,j,pathLength,q)) == true){
                    printf("%s ",getNodeName(graphs[i],q));
                }
            }
        }
        //Destination
        if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,pathLength,pathLength,t)) == true){
              printf("%s ",getNodeName(graphs[i],t));
        }
        printf("\n");
    }
}

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
void createDotFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength, char* name){
    char s[1024];
    int source, target;

    if(name == NULL){
        sprintf(s,"result-l%d.dot",pathLength);
    }else{
        sprintf(s,"%s-l%d.dot",name,pathLength);
    }
    FILE *f = fopen(s,"w+");

    sprintf(s,"digraph %s {\n",name);
    fputs(s, f);

    for(int i=0; i<numGraph; i++){

        for(source=0;source<orderG(graphs[i]) && !isSource(graphs[i],source);source++); //Find the source in the graph i
        for(target=0;target<orderG(graphs[i]) && !isTarget(graphs[i],target);target++); //Find the target in the graph i

        //Set the source --- coloration
        if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,0,pathLength,source)) == true){
            sprintf(s,"_%d_%s ",i,getNodeName(graphs[i],source));
            fputs(s,f);
            fputs("[initial=1,color=green];\n",f);
        }
        //Set the destination --- coloration
        if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,pathLength,pathLength,target)) == true){
            sprintf(s,"_%d_%s ",i,getNodeName(graphs[i],target));
            fputs(s,f);
            fputs("[final=1,color=red];\n",f);
        }

        //Place at first the source at the start of the path
        if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,0,pathLength,source)) == true){
            sprintf(s,"_%d_%s -> ",i,getNodeName(graphs[i],source));
            fputs(s,f);
        }
        //Build the path between the source and the destination
        for(int j=1; j<pathLength; j++){
            for(int q=0; q<orderG(graphs[i]); q++){
                if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,j,pathLength,q)) == true){
                    sprintf(s,"_%d_%s -> ",i,getNodeName(graphs[i],q));
                    fputs(s,f);
                }
            }
        }
        //Place at last the destination at the end of the path
        if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,pathLength,pathLength,target)) == true){
            sprintf(s,"_%d_%s;",i,getNodeName(graphs[i],target));
            fputs(s,f);
        }
        fputs("\n",f);
    }
    fputs("}",f);
    fclose(f);
}