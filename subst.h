#ifndef SUBST_H
#define SUBST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING 100
#define MIN(x, y) (((x) < (y)) ? (x):(y))

void runSubstLoop(char charToPut, char charToReplace);
void substituteChar(char* string, char charToReplace, char charToPut);
void multipleCharReplaces(char* string, char* charsToReplace, char* charsToPut);

#endif
