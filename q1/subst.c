#include "subst.h"

void runSubstLoop(char charToReplace, char charToPut){
    // This function get line from stdin, switch charToReplace with charToPut in the line,
    // and prints it. The loop will stop when EOF arrive.
    char* string = (char*)malloc(MAX_STRING);
    int i;
    int strLen = read(STDIN_FILENO, string, MAX_STRING);
    while(strLen > 0){
        substituteChar(string, charToReplace, charToPut);
        write(STDOUT_FILENO, string, strLen);
        for (i = 0; i < strLen; ++i){
            string[i] = 0;
        }

        strLen = read(STDIN_FILENO, string, MAX_STRING);
    }
    close(STDOUT_FILENO);
    close(STDIN_FILENO);

    free(string);
}

void substituteChar(char* string, char charToReplace, char charToPut){
    // This function swap charToReplace with charToPut in every appearance in string
    int i;
    for (i = 0; i < strlen(string); ++i){
        if (string[i] == charToReplace){
            string[i] = charToPut;
        }
    }
}

int main(int argc, char** argv) {
    runSubstLoop(argv[1][0], argv[2][0]);
    return 0;
}