#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> // Ajouté pour les drapeaux d'ouverture de fichier

// ...existing code...

#define MAX_COMMANDS 100
#define MAX_ARGS 100

int parse_and_execute_pipe(int argc, char **argv)
{
    int i, j;
    int num_pipes = 0;
    int valReTOUR = 0 ;

    // Compter le nombre de pipes
    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            num_pipes++;
        }
    }

    int num_commands = num_pipes + 1;
    char *commands[MAX_COMMANDS][MAX_ARGS];

    // Séparer les commandes
    int cmd = 0;
    int start = 0;
    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            int cmd_length = i - start;
            for (j = 0; j < cmd_length; j++)
            {
                commands[cmd][j] = argv[start + j];
            }
            commands[cmd][j] = NULL;
            cmd++;
            start = i + 1;
        }
    }
    // Dernière commande
    int cmd_length = argc - start;
    for (j = 0; j < cmd_length; j++)
    {
        commands[cmd][j] = argv[start + j];
    }
    commands[cmd][j] = NULL;

    int in_fd = STDIN_FILENO;
    pid_t pid;
    int fd[2];

    for (i = 0; i < num_commands; i++)
    {
        if (i < num_commands - 1)
        {
            if (pipe(fd) < 0)
            {
                perror("pipe");
                return -1;
            }
        }

        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return -1;
        }

        if (pid == 0)
        {
            // Enfant
            if (in_fd != STDIN_FILENO)
            {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (i < num_commands - 1)
            {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
            }

            // Gestion des redirections de sortie et d'erreur standard
            int j = 0;
            while (commands[i][j] != NULL)
            {
                if (strcmp(commands[i][j], ">") == 0 || strcmp(commands[i][j], ">>") == 0 || strcmp(commands[i][j], ">|") == 0)
                {
                    int flags = O_WRONLY | O_CREAT;
                    if (strcmp(commands[i][j], ">") == 0 || strcmp(commands[i][j], ">|") == 0)
                    {
                        flags |= O_TRUNC;
                    }
                    else if (strcmp(commands[i][j], ">>") == 0)
                    {
                        flags |= O_APPEND;
                    }
                    int fd_out = open(commands[i][j + 1], flags, 0644);
                    if (fd_out < 0)
                    {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd_out, STDOUT_FILENO);
                    close(fd_out);
                    commands[i][j] = NULL;
                    j += 2;
                }
                else if (strcmp(commands[i][j], "2>") == 0 || strcmp(commands[i][j], "2>>") == 0 || strcmp(commands[i][j], "2>|") == 0)
                {
                    int flags = O_WRONLY | O_CREAT;
                    if (strcmp(commands[i][j], "2>") == 0 || strcmp(commands[i][j], "2>|") == 0)
                    {
                        flags |= O_TRUNC;
                    }
                    else if (strcmp(commands[i][j], "2>>") == 0)
                    {
                        flags |= O_APPEND;
                    }
                    int fd_err = open(commands[i][j + 1], flags, 0644);
                    if (fd_err < 0)
                    {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd_err, STDERR_FILENO);
                    close(fd_err);
                    commands[i][j] = NULL;
                    j += 2;
                }
                else if (strcmp(commands[i][j], "<") == 0)
                {
                    int fd_in = open(commands[i][j + 1], O_RDONLY);
                    if (fd_in < 0)
                    {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd_in, STDIN_FILENO);
                    close(fd_in);
                    commands[i][j] = NULL;
                    j += 2;
                }
                else
                {
                    j++;
                }
            }

            execvp(commands[i][0], commands[i]);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
        {
            // Parent
            if (in_fd != STDIN_FILENO)
            {
                close(in_fd);
            }
            if (i < num_commands - 1)
            {
                close(fd[1]);
                in_fd = fd[0];
            }
            if (i = num_commands - 1)
            {
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status))
                {
                    valReTOUR= WEXITSTATUS(status);
                }
                else if (WIFSIGNALED(status))
                { // Si l'enfant s'est terminé de manière anormale
                    valReTOUR= 2;
                }
                else
                {
                    valReTOUR=1;
                }
            }
        }
    }

    // Attendre tous les processus
    for (i = 0; i < num_commands; i++)
    {
        wait(NULL);
    }

    // Supprimer les appels à free

    return valReTOUR;
}
