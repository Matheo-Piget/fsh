#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"

int parse_and_execute(char *input, int argc, char **argv)
{
    char *command = strtok(input, " ");
    if (command == NULL)
        return 0;

    // Commandes internes
    if (strcmp(command, "exit") == 0)
    {
        exit(0);
    }
    else if (strcmp(command, "pwd") == 0)
    {
        if (argc > 1)
        {
            fprintf(stderr, "pwd: too many arguments\n");
            return 1; // Code d'erreur
        }
        return builtin_pwd();
    }
    else if (strcmp(command, "cd") == 0)
    {
        if (argc > 2)
        {
            fprintf(stderr, "cd: too many arguments\n");
            return 1; // Code d'erreur
        }
        return builtin_cd(strtok(NULL, " "));
    }
    else if (strcmp(command, "ftype") == 0)
    {
        if (argc > 2)
        {
            fprintf(stderr, "ftype: too many arguments\n");
            return 1; // Code d'erreur
        }
        char *filename = strtok(NULL, " ");
        if (!filename)
        {
            fprintf(stderr, "ftype: no file specified\n");
            return 1; // Code d'erreur
        }
        return builtin_ftype(filename);
    }

    else if (strcmp(command, "for") == 0)
    {
        // TODO : Implémenter la commande for
        return 0;
    }

    // Commande externe
    return execute_command(command, argc, argv);
}
