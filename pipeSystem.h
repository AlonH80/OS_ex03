
#ifndef PIPESYSTEM_H
#define PIPESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>

#include "logger.h"

#define SUBST_PROG_NAME "subst"
#define MIN(x, y) (((x) < (y)) ? (x):(y))
typedef char BOOL;

void redirectStdout(int);
void redirectStdin(int);
void createPipe(int[]);
void createMidSubstProgram(char**, int, int);
void createSubstProcesses(char* charsToReplace, char* charsToPut, const char* outFile);
char** generateArgs(char charToReplace, char charToPut);
void freeGeneratedArgs(char** args);
int** generatePipes(int);
void pipeTestProg(char* charsToReplace, char* charsToPut, const char* outFile);
//int openFile(const char* fileName);
void runProgramLoop(char** args);


#endif
