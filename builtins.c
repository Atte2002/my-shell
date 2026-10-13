#include "builtins.h"
#include <unistd.h>
#include <stdio.h>

char *builtin_str[] = {
    "cd",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &mysh_cd
};

int mysh_cd(char **args)
{
    if(args[1] == NULL)
    {
        fprintf(stderr, "mysh: expected argument to \"cd\"\n");
    }
    else 
    {
        if(chdir(args[1]) != 0)
        {
            perror("mysh");
        }
    }

    return 1;
}

int mysh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}
