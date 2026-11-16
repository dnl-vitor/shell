# dhell

**dhell** (*Danilo's Shell*) é um shell simples para Linux escrito em C. O projeto é baseado no clássico tutorial [*Write a Shell in C*](https://brennan.io/2015/01/16/write-a-shell-in-c/) (lsh), de Stephen Brennan, com comandos internos próprios, saída colorida e o código dividido em módulos.

Foi feito com fins de estudo: entender na prática `fork`, `execvp`, `waitpid`, chamadas de sistema POSIX e gerenciamento de memória em C.

## Funcionalidades

- Laço de leitura, interpretação e execução de comandos (REPL)
- Execução de programas externos via `fork` + `execvp` + `waitpid`
- Comandos internos (*builtins*) implementados do zero
- Prompt colorido com escapes ANSI
- Leitura de linha e *tokenização* com buffers dinâmicos (`malloc`/`realloc`)

## Comandos internos

| Comando | Descrição |
|---------|-----------|
| `pwd` | Mostra o diretório atual (com fundo azul) |
| `cd <dir>` | Muda o diretório atual |
| `mkdir <dir>` | Cria um diretório |
| `rmdir <nome>...` | Remove arquivos ou diretórios vazios (aceita vários nomes) |
| `touch <arquivo>...` | Cria arquivos vazios (usa `open()` e `close()`) |
| `echo <texto>` | Imprime os argumentos |
| `ls [-a] [dir]` | Lista arquivos; `-a` mostra ocultos; diretórios aparecem em azul |
| `clear` | Limpa a tela |
| `help` | Lista os comandos internos |
| `exit` | Encerra o shell |

Qualquer outro comando é executado como programa externo, buscado no `PATH`.

## Estrutura do projeto

```
.
├── CMakeLists.txt   # configuração de build
├── run.sh           # compila e executa
├── lsh.h            # macros, protótipos e includes
├── main.c           # ponto de entrada
├── shell.c          # núcleo: loop, leitura, split, launch, execute
└── builtins.c       # comandos internos
```

## Requisitos

- Linux (usa `fork`, `waitpid` e outras chamadas POSIX)
- Compilador C com suporte a C99 (GCC ou Clang)
- CMake 3.14 ou superior

## Como compilar e executar

Com o script pronto:

```bash
chmod +x run.sh
./run.sh
```

Ou manualmente:

```bash
cmake -S . -B build
cmake --build build
./build/bin/dhell
```

## Exemplo de uso

```
@dhell:$ mkdir teste
@dhell:$ cd teste
@dhell:$ touch a.txt b.txt
@dhell:$ ls
a.txt b.txt
@dhell:$ echo olá, mundo
olá, mundo
@dhell:$ exit
```

## Como funciona

1. `lsh_loop` mostra o prompt e repete até o comando retornar `0`.
2. `lsh_read_line` lê a linha caractere a caractere, aumentando o buffer quando necessário.
3. `lsh_split_line` divide a linha em tokens com `strtok`.
4. `lsh_execute` procura o comando na tabela de builtins (`builtin_str` / `builtin_func`); se não encontrar, `lsh_launch` cria um processo filho com `fork` e o executa com `execvp`.

Para adicionar um novo comando interno: declare a função em `lsh.h`, implemente em `builtins.c` e registre o nome em `builtin_str[]` e a função em `builtin_func[]`, **na mesma ordem**.

## Limitações conhecidas

- Sem pipes (`|`), redirecionamento (`>`, `<`) ou execução em segundo plano (`&`)
- Sem aspas, variáveis de ambiente ou expansão de `~` e curingas (`*`)
- Sem histórico de comandos nem autocompletar
- Builtins sem flags, exceto `ls -a`

## Ideias futuras

- [ ] Pipes e redirecionamento
- [ ] Histórico de comandos
- [ ] Suporte a aspas nos argumentos
- [ ] Flag `-r` no `rmdir`/`rm`
- [ ] Interface gráfica de terminal com raylib

## Créditos

Baseado no tutorial *Write a Shell in C*, de Stephen Brennan.

## Autor

Feito por **Danilo** ([@dnl-vitor](https://github.com/dnl-vitor)).
