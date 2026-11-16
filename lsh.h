#ifndef LSH_H                         // evita incluir este header mais de uma vez
#define LSH_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>

/* ---------- macros ---------- */

#define LSH_RL_BUFSIZE 1024           // tamanho inicial (e incremento) do buffer de leitura
#define LSH_TOK_BUFSIZE 64            // tamanho inicial (e incremento) do vetor de tokens
#define LSH_TOK_DELIM " \t\r\n\a"     // caracteres que separam os argumentos

#define BUF_SIZE 1024                 // tamanho do buffer usado no pwd
#define BG_BLUE "\033[44m"            // escape ANSI: fundo azul
#define RESET "\033[0m"               // escape ANSI: volta à cor normal
#define YELLOW "\033[33m"
#define STRONG "\033[1;35m"

/* ---------- shell.c: núcleo do shell ---------- */

void lsh_loop(void);                  // laço principal: ler, interpretar e executar
char *lsh_read_line(void);            // lê uma linha da entrada padrão
char **lsh_split_line(char *line);    // divide a linha em tokens (termina em NULL)
int lsh_launch(char **args);          // executa um programa externo (fork/execvp)
int lsh_execute(char **args);         // decide entre builtin e programa externo

/* ---------- builtins.c: comandos internos ---------- */

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_pwd(char **args);
int lsh_echo(char **args);
int lsh_clear(char **args);
int lsh_mkdir(char **args);
int lsh_rm(char **args);
int lsh_ls(char **args);
int lsh_touch(char **args);
int lsh_num_builtins(void);         

extern char *builtin_str[];           // nomes dos comandos internos
extern int (*builtin_func[])(char **);  // funções dos comandos internos (mesma ordem)

#endif
