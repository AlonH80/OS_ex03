#include "subst.h"

void runSubstLoop(char charToReplace, char charToPut){
    char* string = (char*)malloc(MAX_STRING);
    while(1){
        string[0] = '\0';
        fgets(string, MAX_STRING, stdin);
        substituteChar(string, charToReplace, charToPut);
        dprintf(STDOUT_FILENO, "%s", string);
        if (string[0] == '\0'){
            break;
        }
    }

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

int main(int argc, char** argv) {
    runSubstLoop(argv[1][0], argv[2][0]);
    return 0;
}