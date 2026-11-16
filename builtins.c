#include "lsh.h"

char *builtin_str[] = {               // vetor com os nomes dos comandos internos
  "pwd",
  "cd",
  "mkdir",
  "rmdir",
  "touch",
  "echo",
  "ls",
  "clear",
  "help",
  "exit",
};

int (*builtin_func[])(char **) = {    // vetor de ponteiros para as funções (mesma ordem de builtin_str)
  &lsh_pwd,
  &lsh_cd,
  &lsh_mkdir,
  &lsh_rm,
  &lsh_touch,
  &lsh_echo,
  &lsh_ls,
  &lsh_clear,
  &lsh_help,
  &lsh_exit
};

int lsh_num_builtins(void)            // retorna a quantidade de comandos internos
{
  return sizeof(builtin_str) / sizeof(char *);  // tamanho total do vetor dividido pelo tamanho de um elemento
}

int lsh_cd(char **args)               // muda o diretório atual
{
  if(args[1] == NULL){                // se não foi passado o diretório
    fprintf(stderr, "dhell: expected argument to \"cd\"\n");  // avisa que falta o argumento
  }else{
    if(chdir(args[1]) != 0){          // tenta mudar para o diretório; diferente de 0 é erro
      perror("dhell");                  // mostra o erro do sistema
    }
  }
  return 1;                           // retorna 1 para o shell continuar rodando
}

int lsh_help(char **args)             
{
  int i;                              
  printf("Danilo's Shell (dhell)\n");     
  printf("Type program names and arguments, and hit enter.\n");
  printf("All built-in commands currently have no flags;\nthey operate via brute force, meaning in the\nsimplest way possible.\n");
  printf("The following are built in:\n\n");

  for(i = 0; i < lsh_num_builtins(); i++)   // percorre todos os comandos internos
    printf("%s\n", builtin_str[i]);        // imprime o nome de cada um

  printf("\nUse the man command for information on other programs\n");  
  return 1;                           // retorna 1 para o shell continuar rodando
}

int lsh_pwd(char **args)              // mostra o diretório atual com fundo azul
{
  char buf[BUF_SIZE];                 // buffer para o caminho
  if(getcwd(buf, sizeof(buf)) != NULL){
    printf(BG_BLUE "%s" RESET "\n", buf);
  }else{
    perror("dhell");
  }
  return 1;
}

int lsh_echo(char **args)             // imprime os argumentos separados por espaço
{
  for(size_t j = 1; args[j] != NULL; j++){      // começa em 1 para pular o próprio "echo"
    printf("%s%s", args[j], args[j + 1] ? " " : "");  // espaço só entre palavras
  }
  printf("\n");
  return 1;
}

int lsh_ls(char **args)
{
  int show_all = 0;
  const char *path = ".";
  
  for(int i = 1; args[i] != NULL; i++){
    if(strcmp(args[i], "-a") == 0)
      show_all = 1;
    else
      path = args[i];
  }

  DIR *dir = opendir(path);
  if(dir == NULL){
    perror("dhell");
    return 1;
  }

  struct dirent *entry;
  while((entry = readdir(dir)) != NULL){
    if(!show_all && entry->d_name[0] == '.') continue;
    
    char full[1024];
    snprintf(full, sizeof(full), "%s/%s", path, entry->d_name);

    struct stat st;
    if(stat(full, &st) == 0 && S_ISDIR(st.st_mode))
      printf("\033[1;34m%s\033[0m ", entry->d_name);
    else
      printf("%s ", entry->d_name);
  }
  printf("\n");
  closedir(dir);
  return 1;
}

int lsh_clear(char **args)
{
  printf("\033[H\033[J");
  return 1;
}

int lsh_mkdir(char **args)
{
  if(args[1] == NULL){
    fprintf(stderr, "dhell: esperado nome do diretório\n");
    return EXIT_FAILURE;
  }else{
    if(mkdir(args[1], 0755) != 0)
      perror("dhell");
  }
  return 1;
}

int lsh_rm(char **args)
{
  if(args[1] == NULL){
    fprintf(stderr, "dhell: esperado nome do diretório\n");
    return EXIT_FAILURE;
  }
  
  //percorre todos os nomes passados: rm a b c
  for(int i = 1; args[i] != NULL; i++){
    struct stat st;
    
    //lstat não segue links simbólicos (um link pra diretório é apagado como link)
    if(lstat(args[i], &st) != 0){
      fprintf(stderr, "dhell: rm: ");
      perror(args[i]);
      continue;
    }
    
    //diretório usa rmdir(), qualquer outra coisa usa unlink()
    int r = S_ISDIR(st.st_mode) ? rmdir(args[i]) : unlink(args[i]);
    if(r != 0){
      fprintf(stderr, "dhell: rm: ");
      perror(args[i]);
    }
  }
  return 1;
}

int lsh_touch(char **args)
{
  if(args[1] == NULL){
    fprintf(stderr, "dhell: esperado nome do arquivo\n");
    return EXIT_FAILURE;
  }
  for(int i = 1; args[i] != NULL; i++){
    int n = open(args[i], O_WRONLY | O_CREAT, 0644);
    if(n < 0){
      perror("dhell");
      return EXIT_FAILURE;
    }
    close(n);
  }
  return 1;
}

int lsh_exit(char **args)             // encerra o shell
{
  return 0;                           // retorna 0 para o laço principal parar
}
