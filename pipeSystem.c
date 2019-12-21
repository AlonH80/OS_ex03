
#include "pipeSystem.h"

void redirectStdout(int newStdout){
    if(dup2(newStdout, STDOUT_FILENO) < 0){
        perror("Error redirecting stdout..\n");
    }

}

void redirectStdin(int newStdin){
    if(dup2(newStdin, STDIN_FILENO)){
        perror("Error redirecting stdin..\n");
    }
}

void createPipe(int pip[]){
    if(pipe(pip) < 0){
        fprintf(stderr, "cannot open pipe\n");
        exit(EXIT_FAILURE);
    }
}

void createMidSubstProgram(char** args, int inPipe, int outPipe){
    pid_t pid_status = fork();
    if (pid_status < 0) {
        fprintf(stderr, "error in fork\n");
        exit(EXIT_FAILURE);
    }
    else if (pid_status == 0){ // son
        redirectStdin(inPipe);
        redirectStdout(outPipe);
        execv(SUBST_PROG_NAME, args);
        perror("execv failed, exit program..\n");
        writeToLog("execv failed, exit program..\n");
        exit(1);
    }
}

void createSubstProcesses(char* charsToReplace, char* charsToPut, const char* outFile){
    int len = MIN(strlen(charsToPut), strlen(charsToReplace));
    int i;
    int** pipes = generatePipes(len - 1);
    char** firstArgs = generateArgs(charsToReplace[0], charsToPut[0]);
    createMidSubstProgram(firstArgs, STDIN_FILENO, pipes[0][1]);
    freeGeneratedArgs(firstArgs);

    for(i = 1; i < len - 1; ++i){
        char** args = generateArgs(charsToReplace[i], charsToPut[i]);
        createMidSubstProgram(args, pipes[i - 1][0], pipes[i][1]);
        freeGeneratedArgs(args);
    }

    int fdOutFile = openFile(outFile);
    char** lastArgs = generateArgs(charsToReplace[len - 1], charsToPut[len - 1]);
    createMidSubstProgram(lastArgs, pipes[len - 2][0], fdOutFile);
    freeGeneratedArgs(lastArgs);

    for (i = 0; i < len; ++i){
        wait(NULL);
    }
}

void pipeTestProg(char* charsToReplace, char* charsToPut, const char* outFile) {
    int len = MIN(strlen(charsToPut), strlen(charsToReplace));
    int i;
    int pip[2];
    pipe(pip);
    pid_t pid = fork();
    if(pid == 0){
        close(STDIN_FILENO);
        redirectStdin(pip[0]);
        //redirectStdout(openFile(outFile));
        printf("In son\n");
        printf("Redirecting..\n");
        printf("Argv init..\n");
        char* myArgv[4];
        myArgv[0] = "subst";
        myArgv[1] = "a";
        myArgv[2] = "b";
        myArgv[3] = NULL;
        //strcpy(myArgv[0], "subst");
//        strcpy(myArgv[1], "a");
//        strcpy(myArgv[2], "b");
        printf("Start runnin\n");
        execve("subst", myArgv, NULL);
        printf("Unable to execve");
        exit(1);
    }

    redirectStdout(pip[1]);
    while(!feof(stdin)){
        char* string = (char*)malloc(100);
        fgets(string, 100, stdin);
        dprintf(STDOUT_FILENO, "%s", string);
        //printf("%s\n", string);
        free(string);
    }
}

char** generateArgs(char charToReplace, char charToPut){
    char** args = (char**)malloc(sizeof(char*) * 3);
    args[0] = (char*)malloc(2);
    args[1] = (char*)malloc(2);
    args[2] = NULL;
    sprintf(args[0], "%c", charToReplace);
    args[0][1] = '\0';
    sprintf(args[1], "%c", charToPut);
    args[1][1] = '\0';
    return args;
}

void freeGeneratedArgs(char** args){
    free(args[0]);
    free(args[1]);
    free(args);
}

int** generatePipes(int numOfPipes){
    int** pipes = (int**)malloc(sizeof(int*) * numOfPipes);
    int i;
    for(i = 0;i < numOfPipes; ++i){
        pipes[i] = (int*)malloc(sizeof(int) * 2);
        createPipe(pipes[i]);
    }

    return pipes;
}

int main(int argc, char* argv[]){
    //createSubstProcesses(argv[1], argv[2], argv[3]);
    pipeTestProg(argv[1], argv[2], argv[3]);
    return 0;
}