#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include "../include/builtin.h" // Si les fonctions builtin_* sont déclarées ici
#include "../include/command.h" // Si printerr est déclarée ici
#include "../include/parser.h"
#include "../include/loop.h"
#include <sched.h>

char *find_cmd(char *argv[], size_t size_of_tab, size_t *start_index)
{
    size_t size_of_cmd = 0;
    bool find = false;

    for (size_t i = *start_index; i < size_of_tab; i++)
    {
        if (strcmp(argv[i], "{") == 0)
        {
            find = true;
            continue;
        }
        if (strcmp(argv[i], "}") == 0 && find)
        {
            break;
        }
        if (find)
        {
            size_of_cmd += strlen(argv[i]) + 1;
        }
    }

    if (size_of_cmd == 0)
    {
        return NULL;
    }

    char *cmd = malloc(size_of_cmd * sizeof(char));
    if (cmd == NULL)
    {
        printerr("Erreur d'allocation mémoire pour cmd\n");
        exit(EXIT_FAILURE);
    }

    size_t parc = 0;
    find = false;
    for (size_t i = *start_index; i < size_of_tab; i++)
    {
        if (strcmp(argv[i], "{") == 0)
        {
            find = true;
            continue;
        }
        if (strcmp(argv[i], "}") == 0 && find)
        {
            *start_index = i + 1;
            break;
        }
        if (find)
        {
            if (parc > 0)
                cmd[parc++] = ' ';
            strcpy(&cmd[parc], argv[i]);
            parc += strlen(argv[i]);
        }
    }
    cmd[parc] = '\0';
    return cmd;
}

int exec_test(char *cmd)
{
    char *args[128];
    size_t i = 0;

    char *token = strtok(cmd, " ");
    while (token != NULL && i < 127)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    return parse_and_execute(i, args);
}

int if_function(int argc, char **argv)
{
    if (argc < 4)
    {
        printt("Usage : if TEST cmd1 [else cmd2]\n");
        return 1;
    }

    // Exécuter la condition (TEST)
    int test = exec_test(argv[1]);

    size_t start_index = 2;
    char *cmd1 = find_cmd(argv, argc, &start_index);
    if (cmd1 == NULL)
    {
        printt("Erreur : Commande non trouvée pour le if.\n");
        return 1;
    }

    if (test == 0)
    {
        // Exécuter la première commande
        int ret = exec_test(cmd1);
        free(cmd1);
        return ret;
    }
    else
    {
        // Vérifier si une commande 'else' existe
        if (start_index < ((size_t)argc) && strcmp(argv[start_index], "else") == 0)
        {
            start_index++;
            char *cmd2 = find_cmd(argv, argc, &start_index);
            if (cmd2 == NULL)
            {
                printt("Erreur : Commande non trouvée pour le else.\n");
                free(cmd1);
                return 1;
            }

            // Exécuter la commande else
            int ret = exec_test(cmd2);
            free(cmd1);
            free(cmd2);
            return ret;
        }
    }

    free(cmd1);
    return 0;
}
