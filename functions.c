/*Implementarea functiilor:
 * INITIALIZE
 * FINALIZE
 * DUMP
 * ALLOC <SIZE>
 * FREE <INDEX>
 * FILL <INDEX> <SIZE> <VALUE>
 * SHOW <INFO>
    *FREE
    *USAGE
    *ALLOCATIONS
*/

#include <stdio.h>
#include <stdlib.h>

extern unsigned char *ARENA;
extern int ARENA_SIZE;

int startIndex;
/* Structul memoreaza atributele (temporare) pentru blocuri;
 * Ok este 1 daca am gasit loc, -1 in caz contrar
 * Case ia valori de la 1 la 3 */

struct zoneManagement {
    int Next, Prev, Size, zonePosition, Ok, Case;
} found;
 
void findFreeZone(int size);

/* Initializeaza o arena de N octeti si ii seteaza pe 0 */

void INITIALIZE(int N) {
    int i;
    ARENA = malloc(N * sizeof(char));
    for(i = 0; i < N; i++){
        ARENA[i] = 0;
    }
}

/* Elibereaza memoria alocata si inchide programul */

void FINALIZE() {
    free(ARENA);
    exit(0);
}

/* Afiseaza harta memoriei */

void DUMP() {
    int i;
    for(i = 0; i < ARENA_SIZE; i++) {
        /* Dupa fiecare grup de 16 octeti (adica atunci cand
         * i % 16 == 0) afisez indicele din arena pe linie noua */
        if(i % 16 == 0) {
            if(i != 0){
                printf("\n");
            }
            printf("%08X\t", i);
        } 
        /* Spatiu suplimentar intre al 8-lea si al 9-lea octet */
        if(i % 8 == 7) {
            printf("%02X  ", ARENA[i]);
        }
        else if (i % 8 == 15) {
            printf("%02X", ARENA[i]);
        } else {
            printf("%02X ", ARENA[i]);
        }
    }
    /* Indicele ultimului octet */
    printf("\n%08X\n", i);
}

/* Aloca size octeti in arena la prima zona libera gasita.
 * Foloseste functia findFreeZone */

void ALLOC(int size) {
    findFreeZone(size);
    startIndex = *(int *)ARENA;
    if(found.Ok == -1) {
        printf("0\n");
        return;
    }
    /* Cazul 1
     * Arena este goala */
    if(found.Case == 1) {
        *(int *) ARENA        = 4;
        *(int *) (ARENA + 4)  = 0;
        *(int *) (ARENA + 8)  = 0;
        *(int *) (ARENA + 12) = found.Size;
        printf("%d\n", 16);
        return;
    }

    /* Cazul 2
     * Daca este spatiu intre pozitia de start si primul bloc ocupat */
    if(found.Case == 2) {
        *(int *) (ARENA)      = 4;
        *(int *) (ARENA + 4)  = found.Next;
        *(int *) (ARENA + 8)  = 0;
        *(int *) (ARENA + 12) = size + 12;

        *(int *) (ARENA + startIndex + 4) = 4;

        printf("%d\n", found.zonePosition + 12);
        return;
    }
    /* Cazul 3
     * Atribuim valorile pentru zona de gestiune a blocului gasit liber 
     * in zonele de mijloc si modificam atributele pentru blocurile vecine */
    if(found.Case == 3) {
        *(int *) (ARENA + found.zonePosition)     = found.Next;
        *(int *) (ARENA + found.zonePosition + 4) = found.Prev;
        *(int *) (ARENA + found.zonePosition + 8) = found.Size;

        *(int *) (ARENA + found.Prev)       = found.zonePosition;
        *(int *) (ARENA + found.Next + 4)   = found.zonePosition;
        printf("%d\n", found.zonePosition + 12);
    }
    /* Cazul 4
     * Actualizez atributele pentru blocul de dinainte de locul nou gasit */
    if(found.Case == 4) {
       *(int *) (ARENA + found.zonePosition)     = 0;
       *(int *) (ARENA + found.zonePosition + 4) = found.Prev;
       *(int *) (ARENA + found.zonePosition + 8) = found.Size;

       *(int *) (ARENA + found.Prev) = found.zonePosition;
       printf("%d\n", found.zonePosition + 12);

    }
   return; 
}

/* Elibereaza blocul de memorie care incepe la pozitia index */

void FREE(int index) {
    int freeIndex = index - 3 * sizeof(int);
    startIndex = *(int *) ARENA;

    /* Cazul 1
     * In arena este doar un bloc si se cere stergerea lui */
    if(*(int *) (ARENA + startIndex) == 0 && (freeIndex == startIndex)) {
        startIndex = *(int *) ARENA = 0;
        return;
    }
    /* Cazul 2
     * Stergerea primului bloc */
    if(*(int *) (ARENA + startIndex) != 0 && (freeIndex == startIndex)) { 
        int next   = *(int *) (ARENA + freeIndex);
        startIndex = *(int *) ARENA = next;
        *(int *)(ARENA + next + 4) = 0;
        return;
    }

    /* Cazul 3
     * Stergerea ultimului bloc din arena (cand sunt cel putin 2 
     * blocuri in arena) */
    if( *(int *)(ARENA + freeIndex) == 0 ) {
        int prev  = *(int *) (ARENA + freeIndex + 4);
        *(int *)(ARENA + prev) = 0;
        return;     
    }
    /* Cazul 4
     * Se cere stergerea unui bloc din zona de mijloc */
        int next  = *(int *) (ARENA + freeIndex);
        int prev  = *(int *) (ARENA + freeIndex + 4);
        *(int *)(ARENA + prev)     = next;
        *(int *)(ARENA + next + 4) = prev;

    return;     
}

/* Seteaza size octeti consecutivi din arena la valoarea value
 * indepand cu pozitia index */

void FILL(int index, int size, int value) {
   int i;
   for(i = index; (i < index + size) && i <= ARENA_SIZE; i++) {
       ARENA[i] = value;
   }
}

/* Afiseaza numarul de octeti liberi si numarul de regiuni (blocuri)
 * libere din arena */

void SHOW_FREE() {
    int nblocks = 0, nbytes = 0, next, current, bsize, tmp;
    startIndex = *(int *) ARENA;
    /* Daca arena este goala */
    if(*(int *) ARENA == 0) {
        nblocks = 1;
        nbytes = ARENA_SIZE - 4;
        printf("%d blocks (%d bytes) free\n", nblocks, nbytes);
        return;
    }
    /* Daca este un singur block ocupat in arena */
    if(*(int *) (ARENA + startIndex) == 0) {
        nbytes += (startIndex - 4) + (ARENA_SIZE - (startIndex + *(int *) (ARENA + startIndex + 8)));
        nblocks += (startIndex - 4) > 0;
        nblocks += (ARENA_SIZE - (startIndex + *(int *) (ARENA + startIndex + 8))) > 0;
        printf("%d blocks (%d bytes) free\n", nblocks, nbytes);
        return;
    }

    /* Daca sunt mai multe blocuri ocupate in arena */
    /* Verificam daca este spatiu liber intre pozitia de start si primul bloc ocupat */
    if(startIndex - 4 > 0) {
        nbytes += startIndex - 4;
        nblocks++;
    }
    current = *(int *) (ARENA); // = startIndex
    tmp     = current;
    next    = *(int *) (ARENA + startIndex);
    bsize   = *(int *) (ARENA + current + 8);
    /* Parcurgem arena secvential pentru a calcula nr de octeti liberi si nr de blocuri
     * libere */
    while (next != 0) {
        if(next - (current + bsize) > 0) {
            nblocks++;
            nbytes += next - (current + bsize);
        }
        tmp = next;
        current = next;
        bsize   = *(int *) (ARENA + next + 8);
        next    = *(int *) (ARENA + tmp);
            
    }
    /* Verificam daca este spatiu intre ultimul bloc ocupat si sfarsitul arenei */
    if(ARENA_SIZE - (tmp + *(int*) (ARENA + tmp + 8)) > 0) {
        nblocks++;
        nbytes += ARENA_SIZE - (tmp + *(int *) (ARENA + tmp + 8));
    }
    printf("%d blocks (%d bytes) free\n", nblocks, nbytes);
    return;
}

/* Afiseaza numarul de octeti folositi din arena, eficienta 
 * utilizarii si fragmentarea */
 
/* dataBytes = cei din zona de date
   reservedBytes = cei din zona de date + cei din zone de gestiune + cei 4 de la inceput */

void SHOW_USAGE() {
    int nblocks, eff, frag, dataBytes, reservedBytes, current, next, tmp, freeZones, bsize;
    float aux;
    startIndex = *(int *) ARENA;
    /* Daca arena este goala */
    if(startIndex == 0) {
        nblocks = dataBytes = eff = frag = 0;
        printf("%d blocks (%d bytes) used\n", nblocks, dataBytes);
        printf("%d%% efficiency\n", eff);
        printf("%d%% fragmentation\n", frag);
        return;
    }
        freeZones = 0;
        dataBytes = *(int *) (ARENA + startIndex + 8) - 12;
        reservedBytes = 4;
        nblocks = 0; 
        current = *(int *) (ARENA); // = startIndex
        next    = *(int *) (ARENA + startIndex);
        bsize   = *(int *) (ARENA + current + 8);
        /* Parcurgem secvential blocurile arenei */
        while (next != 0) {
            if(next - (current + bsize) > 0) {
                freeZones++;
            } 
            tmp        = next;
            current    = next;
            dataBytes += *(int *) (ARENA + next + 8) - 12;
            next       = *(int *) (ARENA + tmp);
            bsize      = *(int *) (ARENA + tmp + 8);
            nblocks++;
        }
        nblocks++;
        if(startIndex - 4 > 0) {
            freeZones++;
        }
        if(ARENA_SIZE - (tmp + *(int*) (ARENA + tmp + 8)) > 0) {
            freeZones++;
        }
        reservedBytes += dataBytes + nblocks * 3 * sizeof(int);
        aux = ((float)dataBytes / reservedBytes) * 100;
        eff = (int) aux;
        aux = ((float)(freeZones - 1) / nblocks) * 100;
        frag = (int) aux;

        printf("%d blocks (%d bytes) used\n", nblocks, dataBytes);
        printf("%d%% efficiency\n", eff);
        printf("%d%% fragmentation\n", frag);
        return;
    /*} */
}
/* Afiseaza zonele libere si zonele ocupate din arena */

void SHOW_ALLOCATIONS() {
    int current, next, bsize, tmp;
    startIndex = *(int *) ARENA;
    printf("OCCUPIED 4 bytes\n");
    /* Daca arena este goala */
    if(startIndex == 0) {
        printf("FREE %d bytes\n", ARENA_SIZE - 4);
        return;
    }
        if (startIndex > 4) {
            printf("FREE %d bytes\n", startIndex - 4);
        }
        if (*(int *) (ARENA + startIndex + 8) > 0) {
            printf("OCCUPIED %d bytes\n", *(int *)(ARENA + startIndex + 8));
        }
        current = startIndex;
        tmp     = current;
        next    = *(int *) (ARENA + current);
        bsize   = *(int *) (ARENA + current + 8);
        /* Parcurgem secvential blocurile arenei */
        while (next != 0) {
            if(next - (current + bsize) > 0) {
                printf("FREE %d bytes\n", next - (current + bsize));
            } 
            if (*(int *) (ARENA + next + 8) > 0) {
                printf("OCCUPIED %d bytes\n", *(int *)(ARENA + next + 8));
            }
            tmp        = next;
            current    = next;
            next       = *(int *) (ARENA + tmp);
            bsize      = *(int *) (ARENA + tmp + 8);
        }
        if(ARENA_SIZE - (tmp + *(int*) (ARENA + tmp + 8)) > 0) {
            printf("FREE %d bytes\n", ARENA_SIZE - (tmp  + *(int *)(ARENA + tmp + 8)));
        }
        return;
    
}

/* Cauta prima zona libera din arena si returneaza pozitia 
 * la care incepe */

void findFreeZone(int size) {
    startIndex = *(int *) ARENA;
    found.Ok = -1;
    /* Daca arena este goala, prima pozitia libera este 4 */ 
    if(startIndex == 0 && (ARENA_SIZE - 4 * sizeof(char)) >= (size + 3 * sizeof(int))) {
        found.zonePosition = 4;
        found.Next         = 0;
        found.Prev         = 0;
        found.Size         = size + 3 * sizeof(int);
        found.Case         = 1;
        found.Ok           = 1;
        return;
    }
    /* Verificam daca exista loc intre pozitia de start si primul bloc ocupat */
    if(startIndex - sizeof(int) >= size + 3 * sizeof(int)) {
        found.Next         = startIndex;
        found.zonePosition = 4;
        found.Size         = size + 3 * sizeof(int);
        found.Prev         = 0;
        found.Ok           = 1;
        found.Case         = 2;
        return;
    }
    /* Cautam loc liber in zonele "de mijloc" */
    if(found.Ok == -1) {
        int next, bsize, current;
        current = *(int *) (ARENA); // = startIndex
        next    = *(int *) (ARENA + current);
        bsize   = *(int *) (ARENA + current + 8);
        /* Parcurgem toate blocurile pana cand gasim primul loc liber dintre 
         * doua blocuri */
        while (next != 0) {
            if(next - (current + bsize) >= size + 12) {
                found.Ok           = 1;
                found.Case         = 3;
                found.zonePosition = current + bsize;
                found.Next         = next;
                found.Prev         = current;
                found.Size         = size + 3 * sizeof(int);
                return;
            }

            int tmp = next;
            current = next;
            bsize   = *(int *) (ARENA + next + 8);
            next    = *(int *) (ARENA + tmp);
        }
    }
    
    /* Verificam daca exista spatiu liber intre ultimul bloc si sfarsitul arenei */

    if(found.Ok == -1) { 
        int next, bsize, current, tmp;
        current = *(int *) (ARENA); // = startIndex
        next    = current;
        /* Parcurgem arena pentru a gasi ultimul bloc */
        do {
            tmp  = next;
            next = *(int *) (ARENA + tmp);
        } while(next != 0);

        bsize = *(int *) (ARENA + tmp + 8); // dimensiunea ultimului bloc
        current = tmp;
        int dimension = size + 3 * sizeof(int); // evit warningul afisat la compilare pentru comparatia de mai jos
        if(ARENA_SIZE - (current + bsize) >= dimension) {
            found.Ok           = 1;
            found.Case         = 4;
            found.zonePosition = current + bsize;
            found.Next         = 0;
            found.Prev         = current;
            found.Size         = size + 3 * sizeof(int);
        }
        return;
    }

    return;
}
