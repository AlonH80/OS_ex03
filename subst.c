#include "subst.h"

void runSubstLoop(char charToPut, char charToReplace){
    while(!feof(stdin)){
        char* string = (char*)malloc(MAX_STRING);
        fgets(string, MAX_STRING, stdin);
        string[strlen(string) - 1] = '\0';
        substituteChar(string, charToPut, charToReplace);
        printf("%s\n", string);
        free(string);
    }

    printf("Bye bye!\n");
}

void substituteChar(char* string, char charToReplace, char charToPut){
    int i;
    for (i = 0; i < strlen(string); ++i){
        if (string[i] == charToReplace){
            string[i] = charToPut;
        }
    }
}

void multipleCharReplaces(char* string, char* charsToReplace, char* charsToPut){
    int len = MIN(strlen(charsToPut), strlen(charsToReplace));
    int i;
    for( i = 0; i < len; ++i){
        // fork + execv to replace chars
        substituteChar(string, charsToReplace[i], charsToPut[i]);
    }
}

int main(int argc, char** argv) {
    runSubstLoop(argv[1][0], argv[2][0]);
    return 0;
}