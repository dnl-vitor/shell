#include "lsh.h"

void lsh_loop(void)                   // laço principal: ler, interpretar e executar comandos
{
  char *line;                         // guarda a linha digitada
  char **args;                        // guarda o vetor de argumentos da linha
  int status;                         // guarda o retorno do comando (0 = sair, 1 = continuar)

  do{
    printf(STRONG YELLOW "@dhell:" RESET "$ ");                     // mostra o prompt
    line = lsh_read_line();           // lê a linha digitada pelo usuário
    args = lsh_split_line(line);      // separa a linha em argumentos
    status = lsh_execute(args);       // executa o comando e guarda o status

    free(line);                       // libera a memória da linha
    free(args);                       // libera a memória do vetor de argumentos
  }while(status);                     // continua enquanto status for diferente de 0
}

char *lsh_read_line(void)             // lê uma linha inteira da entrada padrão
{
  int bufsize = LSH_RL_BUFSIZE;       // tamanho atual do buffer
  int position = 0;                   // posição atual de escrita no buffer
  char *buffer = malloc(sizeof(char) * bufsize);  // aloca o buffer inicial
  int c;                              // guarda o caractere lido (int para comportar o EOF)

  if(!buffer){                        // se a alocação falhou
    fprintf(stderr, "dhell: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while(1){                           // laço infinito, sai pelo return
    c = getchar();                    // lê um caractere da entrada padrão

    if(c == EOF || c == '\n'){        // se chegou ao fim do arquivo ou ao fim da linha
      buffer[position] = '\0';        // termina a string com o caractere nulo
      return buffer;                  // devolve a linha lida
    }else{
      buffer[position] = c;           // guarda o caractere no buffer
    }
    position++;                       // avança para a próxima posição

    if(position >= bufsize){          // se a posição atingiu o tamanho do buffer
      bufsize += LSH_RL_BUFSIZE;      // aumenta o tamanho do buffer
      buffer = realloc(buffer, bufsize);  // realoca o buffer com o novo tamanho
      if(!buffer){                    // se a realocação falhou
        fprintf(stderr, "dhell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

char **lsh_split_line(char *line)     // divide a linha em um vetor de strings terminado em NULL
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;    // tamanho do vetor e posição atual
  char **tokens = malloc(bufsize * sizeof(char*));  // aloca o vetor de ponteiros para tokens
  char *token;                        // ponteiro para o token atual

  if(!tokens){                        // se a alocação falhou
    fprintf(stderr, "dhell: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);  // pega o primeiro token da linha
  while(token != NULL){               // enquanto houver tokens
    tokens[position] = token;         // guarda o token no vetor
    position++;                       // avança para a próxima posição

    if(position >= bufsize){          // se o vetor ficou cheio
      bufsize += LSH_TOK_BUFSIZE;     // aumenta o tamanho do vetor
      tokens = realloc(tokens, bufsize * sizeof(char*));  // realoca o vetor de tokens
      if(!tokens){                    // se a realocação falhou
        fprintf(stderr, "dhell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, LSH_TOK_DELIM);  // pega o próximo token (NULL continua a mesma string)
  }
  tokens[position] = NULL;            // marca o fim do vetor com NULL
  return tokens;                      // devolve o vetor de tokens
}

int lsh_launch(char **args)           // cria um processo filho para rodar um programa externo
{
  pid_t pid, wpid;                    // pid do processo e retorno do waitpid
  int status;                         // estado de saída do processo filho

  pid = fork();                       // duplica o processo atual
  if(pid == 0){                       // se pid é 0, estamos no processo filho
    if(execvp(args[0], args) == -1){  // substitui o filho pelo programa pedido; -1 indica erro
      perror("dhell");                  // mostra o erro do sistema
    }
    exit(EXIT_FAILURE);               // se o exec falhou, o filho termina com falha
  }else if(pid < 0){                  // se pid é negativo, o fork falhou
    perror("dhell");                    // mostra o erro do sistema
  }else{                              // caso contrário, estamos no processo pai
    do{                               // espera o filho terminar
      wpid = waitpid(pid, &status, WUNTRACED);  // aguarda mudança de estado do filho (inclusive parado)
    }while(!WIFEXITED(status) && !WIFSIGNALED(status)); // repete até o filho sair normalmente ou ser morto por sinal
  }
  return 1;                           // retorna 1 para o shell continuar rodando
}

int lsh_execute(char **args)          // executa um comando interno ou um programa externo
{
  int i;                              // índice do laço

  if(args[0] == NULL)                 // se nenhum comando foi digitado
    return 1;                         // não faz nada e continua o shell

  for(i = 0; i < lsh_num_builtins(); i++){      // percorre todos os comandos internos
    if(strcmp(args[0], builtin_str[i]) == 0){   // se o comando digitado é igual a um interno
      return (*builtin_func[i])(args);          // chama a função correspondente e devolve o resultado
    }
  }
  return lsh_launch(args);            // não é interno: executa como programa externo
}
