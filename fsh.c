#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "parser.h"
#include "execute.h"
#include "prompt.h"

void sigint_handler(int sig)
{
    (void)sig; // Paramètre inutilisé
    // Peut-être ajouter un message si interruption souhaitée
}

int main(int argc, char **argv)
{
    signal(SIGINT, sigint_handler); // Ignorer SIGINT
    int last_return_code = 0;

    while (1)
    {
        // Affichage du prompt
        char *input = prompt(last_return_code);
        if (!input)
        {
            break; // Quitter si l'utilisateur saisit Ctrl-D
        }

        // Parser et exécuter la commande
        last_return_code = parse_and_execute(input, argc, argv);
        free(input);
    }

    return 0;
}
