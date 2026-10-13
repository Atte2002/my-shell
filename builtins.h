#ifndef BUILTINS_H
#define BUILTINS_H

extern char *builtin_str[];
extern int (*builtin_func[]) (char **);

int mysh_cd(char **args);
int mysh_exit(char **args);
int mysh_num_builtins();


#endif
