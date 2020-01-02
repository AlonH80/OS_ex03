#include "subst.h"

void runSubstLoop(char charToReplace, char charToPut){
    char* string = (char*)malloc(MAX_STRING);
    int i;
    int strLen = read(STDIN_FILENO, string, MAX_STRING);
    while(strLen > 0){
        substituteChar(string, charToReplace, charToPut);
        dprintf(STDOUT_FILENO, "%s", string);
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
    int i;
    for (i = 0; i < strlen(string); ++i){
        if (string[i] == charToReplace){
            string[i] = charToPut;
        }
    }
}

int getLine(char* string){
    char currChar;
    int len = 0;
    do {
        currChar = getc(stdin);
        string[len++] = currChar;
    } while(currChar != '\n' && currChar != EOF);
    return currChar == EOF;
}

int main(int argc, char** argv) {
    runSubstLoop(argv[1][0], argv[2][0]);
    return 0;
}