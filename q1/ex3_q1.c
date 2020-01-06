
#include "ex3_q1.h"

int** pipes;
int len;

int openFile(const char* fileName){
    // open the file fileName. return: fd of the file.
    int fd;
    fd = open(fileName, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        fprintf(stderr, "ERROR: open \"%s\" failed (%d). Exiting\n", fileName, fd);
        exit(2);
    }

    return fd;
}

void redirectStdout(int newStdout){
    // Redirect stdout to newStdout
    if (newStdout != STDOUT_FILENO) {
        close(STDOUT_FILENO);
        if (dup2(newStdout, STDOUT_FILENO) < 0) {
            perror("Error redirecting stdout..\n");
        }
    }
}

void redirectStdin(int newStdin){
    // Redirect stdin to newStdin
    if (newStdin != STDIN_FILENO) {
        close(STDIN_FILENO);
        if (dup2(newStdin, STDIN_FILENO)) {
            perror("Error redirecting stdin..\n");
        }
    }
}

void createPipe(int pip[]){
    if(pipe(pip) < 0){
        fprintf(stderr, "cannot open pipe\n");
        exit(EXIT_FAILURE);
    }
}

int createSubstProgram(char** args, int pipeIndex){
    // Create subst program with arguments args.
    // stdin of the process - pipes[pipeIndex][0],
    // stdout of the process - pipes[pipeIndex + 1][1]
    pid_t pid_status = fork();
    if (pid_status < 0) {
        fprintf(stderr, "error in fork\n");
        exit(EXIT_FAILURE);
    }
    else if (pid_status == 0){ // son
        // close all irrelevant pipes to process
        close(pipes[pipeIndex][1]);
        close(pipes[pipeIndex + 1][0]);
        closeAllPipesFrom(pipeIndex + 2);
        // redirect and execv subst
        redirectStdin(pipes[pipeIndex][0]);
        redirectStdout(pipes[pipeIndex + 1][1]);
        execve(SUBST_PROG_NAME, args, NULL);
        perror("execv failed, exit program..\n");
        exit(1);
    }
    return pid_status;
}

void createSubstProcesses(char* charsToReplace, char* charsToPut, const char* outFile){
    // create len subst processes
    len = MIN(strlen(charsToPut), strlen(charsToReplace));
    int i;
    int fdOutFile = openFile(outFile);

    //int* pids = (int*)malloc(sizeof(int)*len);
    generatePipes();
    pipes[len][1] = fdOutFile;
    for(i = 0; i < len; ++i){
        char** args = generateArgs(charsToReplace[i], charsToPut[i]);
        //pids[i] =
        createSubstProgram(args, i);
        close(pipes[i][0]);
        close(pipes[i][1]);
        free(pipes[i]);
        freeGeneratedArgs(args);
    }

    for (i = 0; i < len; ++i){
        wait(NULL);
    }
}

char** generateArgs(char charToReplace, char charToPut){
    // Generate string array, will used as args in subst process
    char** args = (char**)malloc(sizeof(char*) * 4);
    args[0] = SUBST_PROG_NAME;
    args[1] = (char*)malloc(2);
    args[2] = (char*)malloc(2);
    args[3] = NULL;
    sprintf(args[1], "%c", charToReplace);
    sprintf(args[2], "%c", charToPut);
    return args;
}

void freeGeneratedArgs(char** args){
    // Free args generated from generateArgs
    free(args[1]);
    free(args[2]);
    free(args);
}

void generatePipes(){
    // generate pipes that will used for subst process IO.
    pipes = (int**)malloc(sizeof(int*) * (len + 1));
    int i;
    pipes[0] = (int*)malloc(sizeof(int) * 2);
    pipes[0][0] = STDIN_FILENO;
    pipes[0][1] = -1;
    for(i = 1;i < len; ++i){
        pipes[i] = (int*)malloc(sizeof(int) * 2);
        createPipe(pipes[i]);
    }

    pipes[len] = (int*)malloc(sizeof(int) * 2);
    pipes[len][0] = -1;
    pipes[len][1] = STDOUT_FILENO;
}

void closeAllPipesFrom(int pipeIndex){
    // Close all pipes in pipes array from pipeIndex.
    for (int i = pipeIndex; i < len; ++i){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

int main(int argc, char* argv[]){
    createSubstProcesses(argv[1], argv[2], argv[3]);
    return 0;
}