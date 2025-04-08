/*-------------------------------------------------------------------------------------------------/
 *  Main for a C- Compiler
 *  File: main.c
 *---------------------------------*/

#include "globals.h"

#include "utils.h"

char source[512];

int main(void) {
    inputSelect();
    openInput();

    return 0;
}