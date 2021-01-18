#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int readLine(char* word, char line[])
{
    int i = 0, count = 0;
    fgets(line,100,stdin);

    //remove \n from line
    while(line[i] != '\n')
        i++;

    line[i] = '\0';

    if(strcmp(line,"exit") == 0)
        exit(0);

    count = split_into_words(word,line);

    return count;
}

int split_into_words(char* word[], char line[])
{
    int i = 0;
    char* ptr = strtok(line," ");

    while(ptr != NULL)
    {
        word[i++] = ptr;
        ptr = strtok(NULL," ");
    }
    /*for(int j=0 ; j<i ; j++)
            printf("%s \n",word[j]);*/
    return i;
}

int cd(char *path)
{
    return chdir(path);
}
void signalHandler(int sig)
{
    FILE* f;
    wait(NULL);
    f = fopen("log.txt","a");
    printf("\nChild process was terminated with id = %d \n\n",getpid());
    fprintf(f,"\nChild process was terminated with id = %d \n",getpid());
    fclose(f);
}
int main()
{

    char* word[10];
    char line[100];
    int count, background = 0;

    signal(SIGCHLD,signalHandler);

    while(1)
    {

        count = readLine(word,line);
        pid_t pid = fork();

        if(strcmp(word[count-1],"&") == 0)
        {
            background = 1;
            word[count-1] = NULL;
        }
        if(strcmp(word[0],"cd") == 0)
        {
            if(cd(word[1])<0)
                perror(word[1]);
            continue;   /* skip fork*/
        }

        if(pid >= 0)  // fork success
        {
            if(pid == 0)  //child proccess
            {
                if(execvp(word[0],word)<0)
                    perror(word[0]);
            }
            else    //parent process
            {
                if(background == 1)
                    continue;
                waitpid(pid,NULL,0);  // wait until child process finishes
            }
        }
        else
            printf("Fork Failed\n");
    }

    return 0;
}
