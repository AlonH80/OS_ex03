#ifndef SUBST_H
#define SUBST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
//#include <fcntl.h>

#define MAX_STRING 100


void runSubstLoop(char charToReplace, char charToPut);
void substituteChar(char* string, char charToReplace, char charToPut);
int getLine(char* string);
//void multipleCharReplaces(char* string, char* charsToReplace, char* charsToPut);
//void openLog();
//void closeLog();
//void writeToLog(char* str);


#endif
