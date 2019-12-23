
#include "logger.h"

int openFile(const char* fileName){
    int fd;
    fd = open(fileName, O_RDWR|O_CREAT|O_APPEND|O_NONBLOCK, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        fprintf(stderr, "ERROR: open \"%s\" failed (%d). Exiting\n", fileName, fd);
        exit(2);
    }

    return fd;
}

void openLog(){
    logFD = openFile(logFile);
}

void closeLog(){
    close(logFD);
}

void writeToLog(char* str){
    dprintf(logFD, str, strlen(str));
}