/* Sursa principala a alocatorului de memorie */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "functions.h"

int ARENA_SIZE, ALLOC_SIZE, FREE_INDEX, FILL_INDEX, FILL_SIZE, FILL_VALUE;
char SHOW_PARAM[13];
char line[50];
unsigned char *ARENA;

/* Functia executa comenzile date de la tastatura cu parametrii 
 * setati de functia getParams() prin apelarea functiilor din 
 * fisierul functions.c */

void executeCommand(int index) {
    if(index == 1) {
       INITIALIZE(ARENA_SIZE);
       return;
    }

    if(index == 3) {
       DUMP();
       return;
    }

    if(index == 4) {
       ALLOC(ALLOC_SIZE);
       return;
    }

    if(index == 5) {
       FREE(FREE_INDEX);
       return;
    }

    if(index == 6) {
       FILL(FILL_INDEX, FILL_SIZE, FILL_VALUE);
       return;
    }

    if(index == 7) {
        if(strcmp(SHOW_PARAM, "FREE") == 0) {
                SHOW_FREE();
                return;
        }

        if(strcmp(SHOW_PARAM, "USAGE") == 0) {
                SHOW_USAGE();
                return;
        }

        if(strcmp(SHOW_PARAM, "ALLOCATIONS") == 0) { 
                SHOW_ALLOCATIONS();
                return;
        }
    }
    return;
}

int main() {
    int commandIndex;   
    /* Citesc comenzi pana la introducerea comenzii FINALIZE */
    while(strcmp(line, "FINALIZE") != 0) {
        fgets(line, 50, stdin);
        /* Setez identificatorul pentru fiecare comanda */
        commandIndex = getCommand(line);
        /* Setez parametrii pentru fiecare comanda care ii necesita */
        getParams(commandIndex);
        executeCommand(commandIndex);
    } 
    /* Eliberez memoria si inchid programul */
    FINALIZE();

    return 0;
}
