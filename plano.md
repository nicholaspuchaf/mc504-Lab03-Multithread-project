# Plano de Desenvolvimento

## Tema

O projeto escolhido sera uma simulacao multithread de um aeroporto com pistas compartilhadas. A aplicacao vai representar avioes que desejam pousar ou decolar, uma torre de controle que coordena o uso das pistas e recursos limitados do aeroporto.

A visualizacao sera feita diretamente no terminal usando caracteres ASCII. Nao usaremos bibliotecas graficas.

## Objetivo

Implementar uma aplicacao em C com `pthreads` que explore sincronizacao entre threads usando mutexes, semaforos e variaveis de condicao. O estado global da simulacao sera exibido periodicamente no terminal para mostrar a evolucao dos avioes, filas, pistas e decisoes da torre.

## Conceitos de Sistemas Operacionais

Os principais conceitos abordados serao:

- Threads concorrentes representando entidades independentes do sistema.
- Exclusao mutua para proteger o estado compartilhado.
- Semaforos para controlar recursos limitados, como numero de pistas disponiveis.
- Variaveis de condicao para bloquear e acordar avioes conforme autorizacao da torre.
- Filas de espera com politicas de prioridade.
- Problema de alocacao de recursos compartilhados.
- Evitar condicoes de corrida no acesso ao estado global.
- Evitar deadlock por meio de uma ordem clara de aquisicao de locks.
- Evitar starvation usando regras de prioridade balanceadas.
- Produtor-consumidor, no sentido de avioes produzindo requisicoes e a torre consumindo/liberando essas requisicoes.
- Visualizacao de estado global em aplicacao concorrente.

## Regras da Simulacao

Cada aviao sera uma thread. Ao iniciar, o aviao escolhe ou recebe uma operacao:

- Pouso.
- Decolagem.

Avioes que querem pousar entram em uma fila de pouso. Avioes que querem decolar entram em uma fila de decolagem.

A torre de controle decide quais avioes podem usar as pistas. Como pouso costuma ser mais critico, a politica inicial sera:

- Pousos tem prioridade sobre decolagens.
- Se houver muitos pousos consecutivos, a torre libera uma decolagem quando possivel para evitar starvation.
- Avioes em emergencia tem prioridade maxima.

As pistas sao recursos limitados. Se o aeroporto tiver `N` pistas, no maximo `N` avioes podem estar pousando ou decolando ao mesmo tempo.

## Parametros Variaveis

A implementacao deve permitir variar parametros por argumentos de linha de comando ou constantes configuraveis:

- Numero total de avioes.
- Numero de pistas.
- Tamanho maximo das filas.
- Probabilidade de um aviao querer pousar ou decolar.
- Probabilidade de emergencia em avioes que querem pousar.
- Tempo medio de aproximacao.
- Tempo medio de uso da pista.
- Tempo entre chegada de novos avioes.
- Numero maximo de pousos consecutivos antes de tentar liberar uma decolagem.

## Bibliotecas C

As principais bibliotecas previstas sao:

- `pthread.h`: criacao e sincronizacao de threads.
- `semaphore.h`: semaforos POSIX para representar pistas disponiveis ou outros recursos contaveis.
- `stdio.h`: entrada e saida no terminal.
- `stdlib.h`: alocacao de memoria, conversao de argumentos e `rand`.
- `unistd.h`: `usleep`, `sleep` e temporizacao simples.
- `time.h`: inicializacao de semente aleatoria e medicao simples de tempo.
- `string.h`: manipulacao de strings simples, se necessario.
- `stdbool.h`: uso de booleanos.
- `stdint.h`: tipos inteiros com tamanho definido, se necessario.
- `signal.h`: captura opcional de `Ctrl+C` para encerrar a simulacao de forma organizada.

Nao usaremos `ncurses` nem bibliotecas graficas. A animacao sera feita limpando/redesenhando o terminal com sequencias ANSI simples.

## Arquitetura de Arquivos

Estrutura sugerida:

```text
.
├── Makefile
├── README.md
├── plano.md
├── perguntas.md
└── src
    ├── main.c
    ├── airport.c
    ├── airport.h
    ├── airplane.c
    ├── airplane.h
    ├── queue.c
    ├── queue.h
    ├── render.c
    ├── render.h
    ├── sync.c
    └── sync.h
```

Responsabilidades:

- `main.c`: parse de argumentos, inicializacao da simulacao, criacao das threads e encerramento.
- `airport.c/.h`: estado global do aeroporto, pistas, contadores, filas e configuracao.
- `airplane.c/.h`: comportamento das threads de aviao.
- `queue.c/.h`: filas de pouso, decolagem e emergencia.
- `render.c/.h`: visualizacao ASCII no terminal.
- `sync.c/.h`: funcoes auxiliares de sincronizacao, se a separacao fizer sentido.

Dependendo do tamanho final, `sync.c` pode ser incorporado em `airport.c` para evitar abstracao desnecessaria.

## Modelo de Dados

Estruturas previstas:

```c
typedef enum {
    PLANE_WAITING_LANDING,
    PLANE_WAITING_TAKEOFF,
    PLANE_LANDING,
    PLANE_TAKING_OFF,
    PLANE_DONE,
    PLANE_LEFT
} PlaneState;

typedef enum {
    REQUEST_LANDING,
    REQUEST_TAKEOFF
} RequestType;

typedef struct {
    int id;
    RequestType request;
    PlaneState state;
    bool emergency;
    int runway_assigned;
    pthread_cond_t authorized;
    bool cleared;
} Airplane;

typedef struct {
    int id;
    int occupied_by;
    RequestType operation;
} Runway;

typedef struct {
    int total_planes;
    int runway_count;
    int max_queue_size;
    int max_consecutive_landings;
    int landing_count_before_takeoff;
    int running;
} Config;

typedef struct {
    Config config;
    Runway *runways;
    Queue emergency_landing_queue;
    Queue landing_queue;
    Queue takeoff_queue;
    pthread_mutex_t mutex;
    pthread_cond_t tower_event;
    sem_t available_runways;
} Airport;
```

## Sincronizacao

O estado global do aeroporto sera protegido por um mutex principal:

```c
pthread_mutex_t airport_mutex;
```

Esse mutex protegera:

- Filas de espera.
- Estados dos avioes.
- Estado das pistas.
- Contadores da simulacao.
- Decisoes da torre.

Semaforo:

```c
sem_t available_runways;
```

Esse semaforo representara a quantidade de pistas livres. Antes de ocupar uma pista, a torre ou o aviao devera garantir que ha uma pista disponivel. Ao liberar a pista, o semaforo sera incrementado.

Variaveis de condicao:

- Uma condicao global `tower_event`, usada para acordar a torre quando um novo aviao entra em uma fila ou uma pista e liberada.
- Uma condicao por aviao, usada para bloquear a thread do aviao ate que a torre autorize pouso ou decolagem.

Fluxo geral:

1. A thread do aviao registra sua requisicao.
2. O aviao entra na fila adequada.
3. O aviao sinaliza a torre.
4. O aviao fica bloqueado em sua variavel de condicao.
5. A torre escolhe o proximo aviao de acordo com a politica.
6. A torre atribui uma pista, marca o aviao como autorizado e sinaliza sua condicao.
7. O aviao usa a pista por um tempo simulado.
8. O aviao libera a pista e sinaliza a torre.

## Politica da Torre

A torre sera uma thread dedicada. Ela repetira o seguinte ciclo enquanto a simulacao estiver rodando:

1. Esperar evento se nao houver avioes aguardando ou pistas livres.
2. Verificar se existe pista disponivel.
3. Escolher o proximo aviao:
   - Emergencia de pouso primeiro.
   - Pouso normal em seguida.
   - Decolagem quando nao houver pousos esperando ou quando o limite de pousos consecutivos for atingido.
4. Atribuir uma pista.
5. Autorizar o aviao escolhido.
6. Atualizar o estado global para a visualizacao.

Essa politica permite demonstrar prioridade, concorrencia e controle contra starvation.

## Animacao no Terminal

A animacao sera feita por uma thread de renderizacao ou pelo fluxo principal, atualizando a tela em intervalos fixos.

Representacao inicial sugerida:

```text
AEROPORTO - TORRE DE CONTROLE

Pistas:
  Pista 0: [A03 POUSANDO]
  Pista 1: [LIVRE]

Fila emergencia:
  A07 A12

Fila pouso:
  A01 A04 A09

Fila decolagem:
  A02 A05 A06

Avioes concluidos:
  Pousos: 8 | Decolagens: 5 | Emergencias atendidas: 2

Politica:
  Pousos consecutivos: 2/3
```

Tecnica de desenho:

- Usar `printf("\033[2J\033[H")` para limpar a tela e voltar o cursor ao inicio.
- Proteger leitura do estado com o mutex do aeroporto.
- Copiar ou imprimir rapidamente o estado enquanto o mutex esta travado.
- Controlar a taxa de atualizacao com `usleep`.

## Design Geral de Implementacao

### Inicializacao

O `main` fara:

1. Ler argumentos de linha de comando.
2. Criar a configuracao da simulacao.
3. Inicializar aeroporto, filas, mutexes, condicoes e semaforos.
4. Criar a thread da torre.
5. Criar a thread de renderizacao.
6. Criar as threads dos avioes com pequenos intervalos entre elas.
7. Aguardar o termino das threads dos avioes.
8. Encerrar torre e renderizacao.
9. Destruir recursos de sincronizacao.

### Thread do Aviao

Cada aviao:

1. Define se quer pousar ou decolar.
2. Se for pouso, define se esta em emergencia.
3. Entra na fila correspondente.
4. Aguarda autorizacao da torre.
5. Ao ser autorizado, simula uso da pista.
6. Libera a pista.
7. Marca seu estado como concluido.

### Thread da Torre

A torre:

1. Aguarda eventos.
2. Verifica pistas disponiveis.
3. Seleciona avioes conforme prioridade.
4. Atribui pista.
5. Acorda a thread do aviao autorizado.

### Thread de Renderizacao

O renderizador:

1. Periodicamente bloqueia o mutex.
2. Le o estado atual.
3. Imprime o desenho ASCII.
4. Libera o mutex.
5. Dorme por alguns milissegundos.

## Cuidados de Implementacao

- Nunca chamar `sleep` ou `usleep` enquanto segura o mutex principal.
- Manter uma ordem simples de locks para evitar deadlock.
- Usar `while` ao esperar variaveis de condicao, nunca `if`.
- Sinalizar a torre sempre que uma fila mudar ou uma pista for liberada.
- Garantir encerramento limpo quando todos os avioes terminarem.
- Evitar que a renderizacao leia estado parcialmente atualizado.
- Testar cenarios com uma pista e muitas threads, pois esse caso revela mais problemas de sincronizacao.

## Testes Planejados

Cenarios manuais:

- Poucos avioes e uma pista.
- Muitos avioes e uma pista.
- Muitos avioes e varias pistas.
- Alta probabilidade de pouso.
- Alta probabilidade de decolagem.
- Alta probabilidade de emergencia.
- Limite baixo de pousos consecutivos para validar anti-starvation.

Validacoes esperadas:

- Nunca ha mais avioes usando pistas do que o numero de pistas configurado.
- Avioes de emergencia sao atendidos antes dos demais.
- Decolagens nao ficam bloqueadas para sempre quando ha fluxo continuo de pousos.
- Todos os avioes terminam ou sao tratados por uma regra explicita.
- A animacao mostra corretamente filas, pistas e contadores.

## Entregaveis

- Codigo-fonte em C.
- `Makefile`.
- `README.md` com explicacao, compilacao, execucao e parametros.
- `plano.md` com planejamento e design.
- `perguntas.md` com respostas aos pontos do enunciado.
- Video de apresentacao mostrando o problema, as decisoes de implementacao e o programa em execucao.
- Registro do uso de LLM, caso aplicavel.
