#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "../include/parser.h"
#include "../include/execute.h"
#include "../include/prompt.h"
#include "../include/command.h"

void sigint_handler(int sig)
{
    (void)sig; // Paramètre inutilisé
    // Peut-être ajouter un message si interruption souhaitée
}

int argc(char *input)
{
    int count = 0;
    int len = strlen(input);
    for (int i = 0; i < len; i++)
    {
        if (input[i] == ' ')
        {
            count++;
        }
    }
    return count + 1;
}
/*faudra enlevr les guillment des mots si y en a */
char **argv(char *input)
{
    int arg_count = argc(input);
    char **args = malloc((arg_count + 1) * sizeof(char *));
    if (args == NULL)
    {
        perror("malloc");
        return NULL;
    }
    char *input_copy = strdup(input);
    if (input_copy == NULL)
    {
        perror("strdup");
        free(args); // Libérer args si strdup échoue
        return NULL;
    }
    char *arg = strtok(input_copy, " ");
    int i = 0;
    while (arg != NULL)
    {
        args[i] = strdup(arg);
        if (args[i] == NULL)
        {
            goto error;
        }
        arg = strtok(NULL, " ");
        i++;
    }
    args[i] = NULL;
    free(input_copy);
    return args;

error: // free tout en cas d'erreur
    perror("strdup");
    free(input_copy);
    for (int index = 0; index < i; index++)
    {
        free(args[index]);
    }
    free(args);
    return NULL;
}

int main()
{
    signal(SIGINT, sigint_handler);  // Ignorer SIGINT
    signal(SIGTERM, sigint_handler); // Ignorer SIGTERM
    int last_return_code = 0;

    while (1)
    {
        // Affichage du prompt
        char *input = prompt(last_return_code);
        if (!input)
        {
            return 0; // Quitter si l'utilisateur saisit Ctrl-D
        }
        char **s = argv(input);
        if (s[0] && strcmp(s[0], "exit") == 0) // Vérifier si s[0] est valide
        {
            free(input); // Libérer la mémoire allouée pour l'entrée
            if (s[1] == NULL)
            {
                return last_return_code; // Quitte le shell avec le code 0
            }
            else if (s[1] != NULL)
            {
                return atoi(s[1]); // Quitte le shell avec le code passé en argument
            }
            else
            {
                return 1; // Code de retour par défaut si aucun argument n'est passé
            }
        }

        if (strcmp(input, "") == 0)
        {
            continue;
        }

        // Parser et exécuter la commande
        last_return_code = parse_and_execute(argc(input), argv(input));
        free(input);
    }

    return 0;
}
