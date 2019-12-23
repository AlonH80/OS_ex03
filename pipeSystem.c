
#include "pipeSystem.h"

int openFile(const char* fileName){
    int fd;
    fd = open(fileName, O_RDWR|O_CREAT|O_APPEND, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        fprintf(stderr, "ERROR: open \"%s\" failed (%d). Exiting\n", fileName, fd);
        exit(2);
    }

    return fd;
}

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

int createMidSubstProgram(char** args, int inPipe, int outPipe){
    pid_t pid_status = fork();
    if (pid_status < 0) {
        fprintf(stderr, "error in fork\n");
        exit(EXIT_FAILURE);
    }
    else if (pid_status == 0){ // son
        redirectStdin(inPipe);
        redirectStdout(outPipe);
        execve(SUBST_PROG_NAME, args, NULL);
        perror("execv failed, exit program..\n");
        exit(1);
    }
    return pid_status;
}

void createSubstProcesses(char* charsToReplace, char* charsToPut, const char* outFile){
    int len = MIN(strlen(charsToPut), strlen(charsToReplace));
    int i;
    int fdOutFile = openFile(outFile);
    redirectStdout(fdOutFile);
    if (len > 1){
        int** pipes = generatePipes(len);
        for(i = 0; i < len; ++i){
            char** args = generateArgs(charsToReplace[i], charsToPut[i]);
            args[0] = SUBST_PROG_NAME;
            createMidSubstProgram(args, pipes[i][0], pipes[i + 1][1]);
            free(args);
        }
        wait(NULL); // Wait for first program to end
        for (i = 0; i < len - 1; ++i){
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
    }
    else{
        char** args = generateArgs(charsToReplace[0], charsToPut[0]);
        createMidSubstProgram(args, STDIN_FILENO, STDOUT_FILENO);
        free(args);
        wait(NULL); // Wait for first program to end
    }

//    char** firstArgs = generateArgs(charsToReplace[0], charsToPut[0]);
//    firstArgs[0] = SUBST_PROG_NAME;
//    createMidSubstProgram(firstArgs, STDIN_FILENO, pipes[0][1]);
//    free(firstArgs);
//
//    for(i = 1; i < len - 1; ++i){
//        char** args = generateArgs(charsToReplace[i], charsToPut[i]);
//        args[0] = SUBST_PROG_NAME;
//        createMidSubstProgram(args, pipes[i - 1][0], pipes[i][1]);
//        free(args);
//    }
//
//    int fdOutFile = openFile(outFile);
//    char** lastArgs = generateArgs(charsToReplace[len - 1], charsToPut[len - 1]);
//    lastArgs[0] = SUBST_PROG_NAME;
//    createMidSubstProgram(lastArgs, pipes[len - 2][0], fdOutFile);
//    free(lastArgs);

}

char** generateArgs(char charToReplace, char charToPut){
    char** args = (char**)malloc(sizeof(char*) * 4);
    args[1] = (char*)malloc(2);
    args[2] = (char*)malloc(2);
    args[3] = NULL;
    sprintf(args[1], "%c", charToReplace);
    sprintf(args[2], "%c", charToPut);
    return args;
}

void freeGeneratedArgs(char** args){
//    free(args[0]);
//    free(args[1]);
    free(args);
}

int** generatePipes(int numOfPipes){
    int** pipes = (int**)malloc(sizeof(int*) * (numOfPipes + 1));
    int i;
    pipes[0] = (int*)malloc(sizeof(int) * 2);
    pipes[0][0] = STDIN_FILENO;
    pipes[0][1] = NULL;
    for(i = 1;i < numOfPipes; ++i){
        pipes[i] = (int*)malloc(sizeof(int) * 2);
        createPipe(pipes[i]);
    }

    pipes[numOfPipes] = (int*)malloc(sizeof(int) * 2);
    pipes[numOfPipes][0] = NULL;
    pipes[numOfPipes][1] = STDOUT_FILENO;

    return pipes;
}

int main(int argc, char* argv[]){
    createSubstProcesses(argv[1], argv[2], argv[3]);
    //pipeTestProg(argv[1], argv[2], argv[3]);
    return 0;
}