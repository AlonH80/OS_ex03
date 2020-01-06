
#ifndef PIPESYSTEM_H
#define PIPESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SUBST_PROG_NAME "subst"
#define MIN(x, y) (((x) < (y)) ? (x):(y))
typedef char BOOL;

int openFile(const char* fileName);
void redirectStdout(int);
void redirectStdin(int);
void createPipe(int[]);
int createSubstProgram(char**, int);
void createSubstProcesses(char* charsToReplace, char* charsToPut, const char* outFile);
char** generateArgs(char charToReplace, char charToPut);
void freeGeneratedArgs(char** args);
void generatePipes();
void closeAllPipesFrom(int pipeIndex);

#endif
