/**
 * Tyler Crawford - PID: 5099748
 * I affirm that I wrote this program myself without any help from any
 * other people or sources from the internet.
 This program is a shell program that recognizes the following: ">" (redirects standard output from a command to a file), ">>" (Appends standard output from a command to a file if the file exists), "<" ( Redirects the standard input to be from a file, rather than keyboard), and  "|" (Passes standard output of one command to another for further processing .
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_ARGS 20
#define BUFSIZ 1024

void execute(char* comandline);

//Main Function to initiate, read command line and call the Execute() Function
int main (int argc, char* argv [])
{
    char comandline[BUFSIZ];
    execute(comandline);
    return 0;
}

/*Function to seperate the tokens in each command.
 */
int tokenizer(char* comandline, char* args[])
{
    int i = 0;
    while((args[++i] = strtok(NULL, "\n\t ")) != NULL)
    {
        if(i >= MAX_ARGS) //if more than 20 arguments -> exit!
        {
            printf("Too many arguments!\n");
            exit(1);
        }
    }
    return i;
}

/*Function That executes the multi-processes via pipelines and I/O redirections, ">" - Redirects standard output from a comman to a file, ">>" - Appends standard output from a command to a file (if the file exists), "<" - Redirects standard input to be from a file and " |" - passes standard output of one command to another.
 */
void execute(char* comandline)
{
    char* read;
    char* write;
    char* append;
    char* args[MAX_ARGS];
    int comand[MAX_ARGS];
    int pid = 0, sameCall, i = 0, j = 0, k = 0, curr = 0, count = 0, start = 0, output = 0, input = 0, childProcess = 1, pipes = 0;
    
    comand[0] = 0;
    
    int numargs = tokenizer(comandline, args); //number of arguments
    while(args[count])
    {
        if(!strcmp(args[count], "|")) //multiple Pipe
        {
            args[count] = NULL;
            comand[childProcess] = count + 1;
            childProcess++;
        }
        count++;
    }
    
    int fileDes[2*(childProcess - 1)];  // Fild Descriptor
    
    for(count=0; count < (childProcess-1); count++)
    {
        pipe(fileDes + (count*2));
    }
    
    for(count=0; count < childProcess; ++count)
    {
        start = comand[count];
        pid = fork(); //Forks the child process.
        if(pid == 0) //parent process
        {
            while(args[start + i])
            {
                curr = start + i;
                if(args[curr])
                {
                    if(!strcmp(args[curr], ">")) //redirects standard output from a command to a file.
                    {
                        write = args[curr + 1];
                        args[curr] = NULL;
                        output = creat(write,0640); //Creates or overwirtes a file.
                    }
                    else if(!strcmp(args[curr], ">>"))//Append standard output from a command to a file if the file exists; if the file does not exist, create one.
                    {
                        append = args[curr + 1];
                        args[curr] = NULL; //Ensures that token ">>" doesn't crash the command.
                        output = open(append, O_CREAT | O_WRONLY | O_APPEND); //Creates, opens or appends (file offset set to end of file prior to each write a file to attach the output
                    }
                    else if(!strcmp(args[curr], "<")) //Redirect the standard input to be from a file, rather than the keyboard.
                    {
                        read = args[curr + 1];
                        args[curr] = NULL; //Ensures that token "<" doesn't crash the command.
                        input = open(read, O_RDONLY); //Open a file to stdin.
                    }
                    
                }
                i++;
            }
            
            if(output)
            {
                dup2(output, 1);
                close(output);
            }
            else if(count < (childProcess - 1))
            {
                dup2(fileDes[pipes + 1], 1);
            }
            
            if(input)
            {
                dup2(input, 0);
                close(input);
            }
            else if(pipes != 0)
            {
                dup2(fileDes[pipes - 2], 0);
            }
            
            for(k = 0; k < (2*(childProcess - 1)); k++)
            {
                close(fileDes[k]);
            }
            execvp(args[start], args + start);
            perror("exec failed");
            exit(-1);
            
        }
        else if(pid < 0)//Prints error if pid is less than zero.
        {
            perror("Error! Parent Not found! ");
            exit(1);
        }
        pipes = pipes + 2;
    }
    
    for(j = 0; j < (2*(childProcess - 1)); j++)
    {
        close(fileDes[j]);
    }
     
}
