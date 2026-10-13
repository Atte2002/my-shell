#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MYSH_TOK_BUFSIZE 64
#define MYSH_TOK_DELIM "\t\r\n\a"

int mysh_start(char **args);
void mysh_loop(void);
char *mysh_read_line(void);
char **mysh_split_line(char *line);
int mysh_execute(char **args);

int main(int argc, char **argv)
{
    
    mysh_loop();
    
    return EXIT_SUCCESS;
}

void mysh_loop(void)
{
    char *line;
    char **args;
    int status;

    do
    {
        printf("> ");
        line = mysh_read_line();
        args = mysh_split_line(line);
        status = mysh_execute(args); 

        free(line);
        free(args);

    } while(status);
}

char *mysh_read_line()
{
    char *line = NULL;
    size_t buffer_size = 0;

    if(getline(&line, &buffer_size, stdin) == -1)
    {
        // EOF found
        if(feof(stdin))
        {   
            exit(EXIT_FAILURE);
        }
        else 
        {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }
    
    return line;
}

char **mysh_split_line(char *line)
{
   int bufsize = MYSH_TOK_BUFSIZE;
   int pos = 0;
   char **tokens = malloc(bufsize * sizeof(char*));
   char *token;

   if(tokens == NULL)
   {
       fprintf(stderr, "mysh: allocation error\n");
       exit(EXIT_FAILURE);
   }

   token = strtok(line, MYSH_TOK_DELIM);
   while(token != NULL)
   {
       tokens[pos] = token;
       pos++;

        if(pos >= bufsize)
        {
            bufsize += MYSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));   

            if(tokens == NULL)
            {
                fprintf(stderr, "mysh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, MYSH_TOK_DELIM);
   }
   tokens[pos] = NULL;
   return tokens;
}

int mysh_start(char **args)
{
    pid_t pid;
    pid_t wpid;
    int status;

    pid = fork();
    if(pid == 0)
    {
        if(execvp(args[0], args) == -1)
        {
            perror("mysh");
        }
        exit(EXIT_FAILURE);
    }
    else if(pid < 0)
    {
        perror("mysh");
    }
    else 
    {
        while(1)
        {
            wpid = waitpid(pid, &status, WUNTRACED);
            if(wpid == -1)
            {
                perror("mysh");
                break;
            }

            if(WIFEXITED(status) || WIFSIGNALED(status))
            {
                break;
            }
        }
    }
    return 1;
}

