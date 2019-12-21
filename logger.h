
#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>

static const char* logFile = "run.log";
static int logFD = -1;

int openFile(const char* fileName);
void openLog();
void closeLog();
void writeToLog(char* str);

#endif
