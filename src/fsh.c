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

/**
 * @brief Compte le nombre d'arguments dans une chaîne de caractères
 *
 * @param input Chaîne de caractères
 * @return int Nombre d'arguments
 */
int argc(char *input)
{
    int count = 0;
    int i = 0;
    int len = strlen(input);
    while (i < len)
    {
        while (i < len && input[i] == ' ')
        {
            i++;
        }
        if (i < len && input[i] != ' ')
        {
            count++;
            while (i < len && input[i] != ' ')
            {
                i++;
            }
        }
    }
    return count;
}

/**
 * @brief Crée un tableau d'arguments à partir d'une chaîne de caractères
 *
 * @param input Chaîne de caractères
 * @return char** Tableau d'arguments
 */
char **argv(char *input)
{
    int arg_count = argc(input);
    char **args = malloc((arg_count + 1) * sizeof(char *));
    if (args == NULL)
    {
        perror("malloc");
        return NULL;
    }

    int ind = 0;
    for (size_t i = 0; i < arg_count; i++){

        while (input[ind] == ' '){
            ind++;
        }

        int start = ind;

        while (input[ind] != ' ' && input[ind] != '\0'){
            ind++;
        }

        int len = ind - start;
        args[i] = malloc((len + 1) * sizeof(char));
        if (args[i] == NULL)
        {
            perror("malloc");
            free_args(args);
            return NULL;
        }

        strncpy(args[i], input + start, len);
        args[i][len] = '\0';

        

    }

    args[arg_count] = NULL;
    return args;
    
}

/**
 * @brief Libère la mémoire allouée pour les arguments
 *
 * @param args Tableau d'arguments
 */
void free_args(char **args)
{
    if (args)
    {
        for (int i = 0; args[i] != NULL; i++)
        {
            free(args[i]);
        }
        free(args);
    }
}

/**
 * @brief Fonction principale du shell
 *
 * @return int Code de retour du shell
 */
int main()
{
    signal(SIGINT, sigint_handler);  // Ignorer SIGINT
    signal(SIGTERM, sigint_handler); // Ignorer SIGTERM
    int last_return_code = 0;
    char **args;

    while (1)
    {
        // Affichage du prompt
        char *input = prompt(last_return_code);
        if (!input)
        {
            return last_return_code; // Quitter si l'utilisateur saisit Ctrl-D
        }
        args = argv(input);
        if (input != NULL)
        {
            if (args[0] && (strcmp(args[0], "exit") == 0)) // Vérifier si args[0] est valide
            {
                if (args[2] != NULL){

                    printerr("exit: too many arguments\n");
                    free(input); // Libérer la mémoire allouée pour l'entrée
                    free_args(args);
                    last_return_code = 1;
                    continue;

                }
                free(input); // Libérer la mémoire allouée pour l'entrée
                if (args[1] == NULL)
                {
                    free_args(args);
                    return last_return_code; // Quitte le shell avec le code 0
                }
                else if (args[1] != NULL)
                {
                    int exit_code = atoi(args[1]);
                    free_args(args);
                    return exit_code; // Quitte le shell avec le code passé en argument
                }
            }
        }

        if (input != NULL && strcmp(input, "") == 0)
        {
            free(input);
            continue;
        }
        // Parser et exécuter la commande
        last_return_code = parse_and_execute(argc(input), args);
        free(input);
        free_args(args);
    }

    return 0;
}
