/* Parser pentru comenzi */

#include <stdio.h>
#include <string.h>

extern char line[50];
extern int ARENA_SIZE, ALLOC_SIZE, FREE_INDEX, FILL_INDEX, FILL_SIZE, FILL_VALUE;
extern char SHOW_PARAM[13];

/* Converteste un string format din cifre intr-un numar */

int charToInt(char *s) {
    int i, len = strlen(s) - 1, nr = 0, pow = 1;

    for(i = len; i >= 0; i--) {
        nr = nr + (s[i] - 48) * pow;
        pow *= 10;
    }

    return nr;
}

int getCommand(char s[]);

/* Extrage parametrii din comenzile INITIALIZE, ALLOC, FREE,
 * FILL si SHOW */

void getParams(int index) {
    char *ptr = NULL;
    /* Parametrul pentru INITIALIZE: ARENA_SIZE */
    if(index == 1) {
        ptr = strtok(NULL, " \n");
        ARENA_SIZE = charToInt(ptr);
    }
    /* Parametrul pentru ALLOC: ALLOC_SIZE */
    if(index == 4) {
        ptr = strtok(NULL, " \n");
        ALLOC_SIZE = charToInt(ptr);   
    }
    /* Parametrul pentru FREE: FREE_INDEX */
    if(index == 5) {
        ptr = strtok(NULL, " \n");
        FREE_INDEX = charToInt(ptr);
    }
    /* Parametrii pentru FILL: FILL_INDEX, FILL_SIZE, FILL_VALUE*/
    if(index == 6) {
        ptr = strtok(NULL, " \n");
        FILL_INDEX = charToInt(ptr);
        ptr = strtok(NULL, " \n");
        FILL_SIZE = charToInt(ptr);
        ptr = strtok(NULL, " \n");
        FILL_VALUE = charToInt(ptr);
    }
    /* Parametrul pentru SHOW: SHOW_PARAM */
    if(index == 7) {
        ptr = strtok(NULL, " \n");
        strcpy(SHOW_PARAM, ptr);
    }
    return;
}

/* Returneaza cate un identificator (index) pentru fiecare comanda */

int getCommand(char s[]) {
    char *ptr = NULL;
    ptr = strtok(s, " \n");

    if(strcmp(ptr, "INITIALIZE") == 0){
        return 1;
    }

    if(strcmp(ptr, "DUMP") == 0){
        return 3;
    }

    if(strcmp(ptr, "ALLOC") == 0){
        return 4;
    }

    if(strcmp(ptr, "FREE") == 0){
        return 5;
    }

    if(strcmp(ptr, "FILL") == 0){
        return 6;
    }

    if(strcmp(ptr, "SHOW") == 0){
        return 7;
    }

    return 0;
}


