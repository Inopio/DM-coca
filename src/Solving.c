#include "Graph.h"
#include <z3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include "Z3Tools.h"

/**
 * @brief Generates a formula consisting of a variable representing the fact that @p node of graph number @p number is at position @p position of an accepting path.
 * 
 * @param ctx The solver context.
 * @param number The number of the graph.
 * @param position The position in the path.
 * @param k The mysterious k from the subject of this assignment.
 * @param node The node identifier.
 * @return Z3_ast The formula.
 */
Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node){
    Z3_ast x;
    
    int t[4] = {number,position,k,node};
    char s[1024];

    sprintf(s, "X %d,%d,%d,%d", number,position,k,node);
   
    x = mk_bool_var(ctx, s);
    return x;
}

//Phi 1
/**
 * @brief Generates a SAT subformula satisfiable.
 * 
 * @param ctx The solver context.
 * @param graphs An array of graphs.
 * @param numGraphs The number of graphs in @p graphs.
 * @param pathLength The length of the path to check.
 * @return Z3_ast The formula.
 */
Z3_ast subFormulaOne( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength){
    Z3_ast * savephi1 = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    assert(savephi1);
    Z3_ast * args = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    assert(args);
    Z3_ast x1, x2, f, f1;
    int s, t;

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
    free(args);
    free(savephi1);
    return f1;
}

//Phi 2
/**
 * @brief Generates a SAT subformula satisfiable.
 * 
 * @param ctx The solver context.
 * @param graphs An array of graphs.
 * @param numGraphs The number of graphs in @p graphs.
 * @param pathLength The length of the path to check.
 * @return Z3_ast The formula.
 */
Z3_ast subFormulaTwo( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength){
    Z3_ast * savephi2 = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    assert(savephi2);
    Z3_ast * args = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    assert(args);
    Z3_ast f2, tmp, x1, x2;
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
    free(args);
    free(savephi2);
    return f2;
}

//Phi 3
/**
 * @brief Generates a SAT subformula satisfiable.
 * 
 * @param ctx The solver context.
 * @param graphs An array of graphs.
 * @param numGraphs The number of graphs in @p graphs.
 * @param pathLength The length of the path to check.
 * @return Z3_ast The formula.
 */
Z3_ast subFormulaThree( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength){
    Z3_ast * savephi3_or = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    assert(savephi3_or);
    savephi3_or[0] = NULL;
    Z3_ast * savephi3_and = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    assert(savephi3_and);
    savephi3_and[0] = NULL;

    Z3_ast f3, x1, x2;

    for(int i = 0; i < numGraphs; i++){
       for(int j = 0; j <= pathLength; j++){
           Z3_ast f3_or;
           for(int q = 0; q < orderG(graphs[i]); q++){
               x1 = getNodeVariable(ctx, i, j, pathLength, q);
               if(savephi3_or[0]==NULL){
                   f3_or = x1;
                   savephi3_or[0] = f3_or;
               }
               else{
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
            }
            else{
                savephi3_and[1] = f3_or;
                f3 = Z3_mk_and(ctx, 2, savephi3_and);
                savephi3_and[0] = f3;
            }
       }
   }
   free(savephi3_or);
   free(savephi3_and);
   return f3;
}


//Phi 4
/**
 * @brief Generates a SAT subformula satisfiable.
 * 
 * @param ctx The solver context.
 * @param graphs An array of graphs.
 * @param numGraphs The number of graphs in @p graphs.
 * @param pathLength The length of the path to check.
 * @return Z3_ast The formula.
 */
Z3_ast subFormulaFour( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength){
    
    Z3_ast * savephi4 = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    assert(savephi4);
    Z3_ast * args = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    assert(args);
    savephi4[0] = NULL;

    Z3_ast f4,tmp;
    Z3_ast x1, x2;
    int s,t;

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
    free(args);
    return f4;
}

//Phi 5
/**
 * @brief Generates a SAT subformula satisfiable.
 * 
 * @param ctx The solver context.
 * @param graphs An array of graphs.
 * @param numGraphs The number of graphs in @p graphs.
 * @param pathLength The length of the path to check.
 * @return Z3_ast The formula.
 */
Z3_ast subFormulaFive( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength){
    Z3_ast f5, x1, x2, tmp;
    int s,t;
    Z3_ast * args1 = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    assert(args1);
    Z3_ast * savephi5_or = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    assert(savephi5_or);
    Z3_ast * savephi5_and = (Z3_ast*) malloc(2*sizeof(Z3_ast));
    assert(savephi5_and);
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
    return f5;
}

/**
 * @brief Generates a SAT formula satisfiable if and only if all graphs of @p graphs contain an accepting path of length @p pathLength.
 * 
 * @param ctx The solver context.
 * @param graphs An array of graphs.
 * @param numGraphs The number of graphs in @p graphs.
 * @param pathLength The length of the path to check.
 * @return Z3_ast The formula.
 */
Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength){

    if(pathLength == 0){ // Non satisfiable
        return Z3_mk_false(ctx);
    }

    for(int i=0; i<numGraphs; i++){
        if(sizeG(graphs[i]) < pathLength || orderG(graphs[i]) <= pathLength){
            return Z3_mk_false(ctx);
        }
    }
    
    Z3_ast f1 = subFormulaOne(ctx,graphs,numGraphs,pathLength);     //Phi 1
    Z3_ast f2 = subFormulaTwo(ctx,graphs,numGraphs,pathLength);     //Phi 2
    Z3_ast f3 = subFormulaThree(ctx,graphs,numGraphs,pathLength);   //Phi 3
    Z3_ast f4 = subFormulaFour(ctx,graphs,numGraphs,pathLength);    //Phi 4
    Z3_ast f5 = subFormulaFive(ctx,graphs,numGraphs,pathLength);    //Phi 5
    
    Z3_ast fargs[5] = {f1,f2,f3,f4,f5};                             //Phi

    return Z3_mk_and(ctx,5,fargs);
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

    //Based on every graph, min_size is the number of vertices of the smallest graph
    int min_size;
    
    min_size = orderG(graphs[0]);
    for(int i =0; i < numGraphs; i++){
        if(min_size > orderG(graphs[i])){
            min_size = orderG(graphs[i]);
        }
    }
    
    //For every pathLength j, we call graphsToPathFormula with this pathLength and
    //we verify if this is a SAT Formula which can be satisfied
    for(int j=0; j <= min_size; j++){
        tmp = graphsToPathFormula(ctx,graphs,numGraphs,j);   //common path of length j
        if(isFormulaSat(ctx,tmp)==1) { //if we have a common path of length j, f can be satisfied
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

    //Checking every variable in the model to get k, the common path in all graphs
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
    int s,t;
    for(int i=0; i<numGraph; i++){
        printf("Path from graph %d\n",i);
        for(s=0;s<orderG(graphs[i]) && !isSource(graphs[i],s);s++); //Find the source s in the graph i
        for(t=0;t<orderG(graphs[i]) && !isTarget(graphs[i],t);t++); //Find the target t in the graph i

        //Source
        if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,0,pathLength,s)) == true){
            printf("%d: pos 0: %s -> ",i,getNodeName(graphs[i],s),i,pathLength);
        }

        //Build the path between the source and the destination
        for(int j=1; j<pathLength; j++){
            for(int q=0; q<orderG(graphs[i]); q++){
                if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,j,pathLength,q)) == true){
                    printf("%d: pos %d: %s -> ",i,j,getNodeName(graphs[i],q));
                }
            }
        }

        //Destination
        if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,pathLength,pathLength,t)) == true){
              printf("%d: pos %d: %s",i,pathLength,getNodeName(graphs[i],t));
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
    char s[1024];                       //Buffer to store our strings with sprintf and so we can fputs them in the file
    int TabSolutionPath[pathLength+1];  //Tab of every vertice in the path
    int source, target;
    bool IsEdgePath = false;

    if(name == NULL){
        name = "result";
    }

    //Create the folder sol, where there are every .dot generetated
    mkdir("sol", S_IRUSR | S_IWUSR | S_IXUSR);
    sprintf(s,"sol/%s-l%d.dot",name,pathLength);
    FILE *f = fopen(s,"w+");

    sprintf(s,"digraph %s {\n",name);
    fputs(s, f);

    for(int i=0; i<numGraph; i++){

        for(source=0;source<orderG(graphs[i]) && !isSource(graphs[i],source);source++); //Find the source in the graph i
        for(target=0;target<orderG(graphs[i]) && !isTarget(graphs[i],target);target++); //Find the target in the graph i

        //Set the source --- coloration
        if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,0,pathLength,source)) == true){
            TabSolutionPath[0] = source;
            sprintf(s,"_%d_%s ",i,getNodeName(graphs[i],source));
            fputs(s,f);
            fputs("[initial=1,color=green][style=filled,fillcolor=lightblue];\n",f);
        }

        //Set the destination --- coloration
        if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,pathLength,pathLength,target)) == true){
            TabSolutionPath[pathLength] = target;
            sprintf(s,"_%d_%s ",i,getNodeName(graphs[i],target));
            fputs(s,f);
            fputs("[final=1,color=red][style=filled,fillcolor=lightblue];\n",f);
        }

        //Build the .dot with all vertices, testing if they are in the path or not (color them in lightblue if they are in the path)
        for(int j=1; j<pathLength; j++){
            for(int q=0; q<orderG(graphs[i]); q++){
                if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,i,j,pathLength,q)) == true){
                    TabSolutionPath[j] = q;
                    sprintf(s,"_%d_%s [style=filled,fillcolor=lightblue];\n",i,getNodeName(graphs[i],q));
                    fputs(s,f);
                }
                else{
                    sprintf(s,"_%d_%s;\n",i,getNodeName(graphs[i],q));
                    fputs(s,f);
                }
            }
        }

        //Build the .dot with all edges, testing if they are in the path or not (color them in blue if they are in the path)
		for(int j = 0; j < orderG(graphs[i]); j++){
			for(int q = 0; q < orderG(graphs[i]); q++){
				if(isEdge(graphs[i],j,q)){
                    //Checking if j and q are both vertices in TabSolutionPath, if yes, they form an edge in the path
					for(int x = 0; x<pathLength; x++){
						if(j==TabSolutionPath[x] && q ==TabSolutionPath[x+1]){
							IsEdgePath = true;
							sprintf(s,"_%d_%s -> _%d_%s [color=blue];\n",i,
								getNodeName(graphs[i],TabSolutionPath[x]),i,getNodeName(graphs[i],TabSolutionPath[x+1]));
							fputs(s,f);
						}
					}
                    //If j and q are not both in TabSolutionPath, the edge is not in the path
					if(IsEdgePath == false){
							sprintf(s,"_%d_%s -> _%d_%s;\n",i,
						getNodeName(graphs[i],j),i,getNodeName(graphs[i],q));
							fputs(s,f);
					}
					IsEdgePath = false;
				}
			} 
		}
        fputs("\n",f);
    }
    fputs("}",f);
    fclose(f);
}
