#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

void get_current_directory(char *buffer, size_t size)
{
    if (getcwd(buffer, size) == NULL)
    {
        perror("getcwd");
        strcpy(buffer, ""); // Mettre une valeur par défaut pour éviter les problèmes ultérieurs
    }
}

char *prompt(int last_return_code)
{
    char color[10];
    char dir[1024];
    get_current_directory(dir, sizeof(dir));

    if (last_return_code == 0)
    {
        strcpy(color, "\033[32m"); // Vert
    }
    else
    {
        strcpy(color, "\033[91m"); // Rouge
    }

    // Format du prompt
    char *prompt_string = malloc(1024); // Taille de 1024 est correcte si vous êtes sûr que la chaîne ne dépasse pas
    snprintf(prompt_string, 1024, "\001%s\002[%d]\001\033[00m\002 %s$ ", color, last_return_code, dir);
    return readline(prompt_string);
}
