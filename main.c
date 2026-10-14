#include "builtins.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MYSH_TOK_BUFSIZE 64
#define MYSH_TOK_DELIM " \t\r\n\a"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

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
    char cwd[PATH_MAX];

    do
    {
        printf(ANSI_COLOR_BLUE "%s > " ANSI_COLOR_RESET, getcwd(cwd, sizeof(cwd)));
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

int mysh_execute(char **args)
{
    if(args[0] == NULL)
    {
        return 1;
    }

    // Check if the command is builtin
    for(int i = 0; i < mysh_num_builtins(); i++)
    {
        if(strcmp(args[0], builtin_str[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }

    return mysh_start(args);
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



