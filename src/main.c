#include <stdio.h>
#include <z3.h>
#include "Solving.h"
#include "Z3Tools.h"
#include <Graph.h>
#include <Parsing.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>



/**
 * @brief Print if the length is a solution to the formula. Based on the function graphsToFullFormula.
 * 
 * @param ctx The solver context.
 * @param graphs An array of graphs.
 * @param numGraphs The number of graphs in @p graphs.
 * @param hasOption_a Boolean to know if option -a is given.
 * @param hasOption_d Boolean to know if option -d is given.
 */
void printAllLength( Z3_context ctx, Graph *graphs,unsigned int numGraphs, bool hasOption_a, bool hasOption_d){
    Z3_ast f;

    int min_size;   //limit of k
    min_size = orderG(graphs[0]);
    for(int i =0; i < numGraphs; i++){
        if(min_size > orderG(graphs[i])){
            min_size = orderG(graphs[i]);
        }
    }

    if(hasOption_d == false){
        for(int j=0; j<=min_size; j++){ //
            f = graphsToPathFormula(ctx,graphs,numGraphs,j);
            if(isFormulaSat(ctx,f)==1) {
                printf("There is a simple valid path of length %d in all graphs\n",j);
                if(hasOption_a == false){   // no "-a" means we stop at the first solution
                    break;
                }
            }else if(isFormulaSat(ctx,f)==-1){
                printf("No simple valid path of length %d in all graphs\n",j);
            }     
        }
    }else{
        for(int j=min_size; j>=0; j--){ // "-d" option so we go backward
            f = graphsToPathFormula(ctx,graphs,numGraphs,j);
            if(isFormulaSat(ctx,f)==1) {
                printf("There is a simple valid path of length %d in all graphs\n",j);
                if(hasOption_a == false){    // no "-a" means we stop at the first solution
                    break;
                }
            }else if(isFormulaSat(ctx,f)==-1){
                printf("No simple valid path of length %d in all graphs\n",j);
            }     
        }
    }
}

bool isPathGraph(char * str){
    if(strlen(str)-4 < 0){
		return false;
	}
    char * tab = (char*) malloc(4*sizeof(char));
    int indextab = 0;
    for(int i = strlen(str)-4; i <= strlen(str); i++){ //to examine the last 4 char, ".dot" extension
		tab[indextab] = str[i];
        indextab++;
	}
    if(!strcmp(tab, ".dot")){
        return true;
    }
    return false;
}

int main (int argc, char **argv){
    Z3_ast f;
    Z3_model model;
    Z3_context c = makeContext();

    int k; //solution length
    int isSat;  //boolean to verify sat formula
    int arraySize = 100;  //to start the array of graphs, will be increased if needed
    int nbGraphs = 0;
    bool hasOption_s = false;
    bool hasOption_a = false;
    bool hasOption_d = false;
    char * fileName = NULL; //for the name of the file

    Graph * graph = (Graph *) malloc(arraySize*sizeof(Graph));
    assert(graph);

    //Initialisation of graphs 
    for(int i = 1; i < argc; i++){
        if(isPathGraph(argv[i])){    //comparison between t
            graph[nbGraphs] = getGraphFromFile(argv[i]);
            nbGraphs++;
        }
    }

    for (int i =1; i< argc; i++){
        if(!strcmp(argv[i], "-h")){
            printf ("Options:\n"); 
            printf ("-h         Displays this help\n");
            printf ("-v         Activate verbose mode (displays parsed graphs\n");
            printf ("-F         Displays the formula computed\n");
            printf ("-s         Tests separately all formula by depth [if not present: uses the global formula]\n");
            printf ("-d         Only if -s is present. Explore the length in decreasing order. [if not present: in increasing order]\n");
            printf ("-a         Only if -s is present. Computes a result for every length instead of stopping at the first positive result (default behaviour)\n");
            printf ("-t         Displays the paths found on the terminal [if not present, only displays the existence of the path].\n");
            printf ("-f         Writes the result with colors in a .dot file. See next option for the name. These files will be produced in the folder 'sol'.\n");
            printf ("-o NAME    Writes the output in 'NAME-lLENGTH.dot' where LENGTH is the length of the solution. [if not present: 'result-lLENGTH.dot']\n");
    
        }
        
        else if (!strcmp (argv[i], "-v" ) && argc > 2){
            for(int j =0; j<nbGraphs; j++){
                printGraph(graph[j]);       //display parsed graphs
            }
        }
      
        else if (!strcmp (argv[i], "-F")){
            f = graphsToFullFormula(c,graph,nbGraphs);  
            printf("Path formula: %s\n",Z3_ast_to_string(c,f)); //display the formula 
        }
        
        else if (!strcmp (argv[i], "-s")){
            hasOption_s = true;
            for (int j = 1; j < argc; j++){
                if(!strcmp (argv[j], "-a") ){
                    hasOption_a = true;         //set option
                }
                if(!strcmp (argv[j], "-d")){
                    hasOption_d = true;         //set option
                }
            }
            /**
            *  boolean case options to avoid duplication of printing
            **/
            if(hasOption_a == true && hasOption_d == true){ 
                printAllLength(c,graph,nbGraphs,true,true); 
            }
            else if(hasOption_a == true && hasOption_d == false){
                printAllLength(c,graph,nbGraphs,true,false);
            }
            else if(hasOption_a == false && hasOption_d == true){
                printAllLength(c,graph,nbGraphs,false,true);
            }
            else{
                printAllLength(c,graph,nbGraphs,false,false);
            }
        }

        else if (!strcmp (argv[i], "-t")){
            f = graphsToFullFormula(c,graph,nbGraphs);
            model = getModelFromSatFormula(c,f);
            k = getSolutionLengthFromModel(c,model,graph);
            printPathsFromModel(c,model,graph,nbGraphs,k);  //display the path
        }

        else if (!strcmp (argv[i], "-f")){
            if(!strcmp (argv[i+1], "-o")){
                fileName = argv[i+2];
            }
            f = graphsToFullFormula(c,graph,nbGraphs);
            model = getModelFromSatFormula(c,f);
            k = getSolutionLengthFromModel(c,model,graph);
            if(fileName == NULL)
                createDotFromModel(c,model,graph,nbGraphs,k,NULL);  //create the file with NULL name
            createDotFromModel(c,model,graph,nbGraphs,k,fileName);  //create the file using the name
        }
    }

    //Default behavior
    if(hasOption_s == false){
        f = graphsToFullFormula(c,graph,nbGraphs);
        isSat = isFormulaSat(c,f);
        if(isSat == 1){
            model = getModelFromSatFormula(c,f);
            k = getSolutionLengthFromModel(c,model,graph);      //get all solutions
            printf("There is a simple valid path of length %d in all graphs\n",k);
        }else{
            printf("No simple valid path of equal length in all graphs\n");
        }
    }

    free(graph);
    Z3_del_context(c);
}
