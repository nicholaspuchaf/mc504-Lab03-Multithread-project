# Aeroporto Multithread

Projeto da disciplina MC504 - Sistemas Operacionais.

O objetivo e implementar uma simulacao multithread de um aeroporto com pistas compartilhadas. Avioes solicitam pouso ou decolagem, enquanto uma torre de controle coordena o uso das pistas disponiveis usando mecanismos de sincronizacao.

A visualizacao sera feita no terminal com caracteres ASCII, sem uso de bibliotecas graficas.

## Tema

O sistema representa um aeroporto com:

- Avioes que chegam para pousar.
- Avioes que aguardam decolagem.
- Pistas limitadas e compartilhadas.
- Torre de controle responsavel por autorizar o uso das pistas.
- Prioridade para pousos de emergencia.
- Politica para evitar starvation de decolagens.

As pistas nao sao threads. Elas sao recursos compartilhados. As entidades ativas do sistema serao os avioes, a torre de controle e, possivelmente, uma thread de renderizacao.

## Conceitos Abordados

O projeto foi planejado para exercitar:

- Criacao e coordenacao de threads com `pthreads`.
- Exclusao mutua com `pthread_mutex_t`.
- Variaveis de condicao com `pthread_cond_t`.
- Semaforos POSIX com `sem_t`.
- Filas concorrentes protegidas por mutex.
- Alocacao de recursos limitados.
- Politicas de prioridade.
- Prevencao de starvation.
- Visualizacao do estado global de uma aplicacao concorrente.

## Plano de Implementacao

A simulacao sera implementada em C.

Cada aviao sera representado por uma thread. Ao iniciar, o aviao entra em uma fila de pouso, decolagem ou emergencia. A torre de controle escolhe o proximo aviao a ser autorizado com base nas prioridades da simulacao.

Fluxo esperado:

1. Um aviao cria uma requisicao de pouso ou decolagem.
2. A requisicao entra na fila correspondente.
3. A torre e sinalizada.
4. O aviao espera autorizacao.
5. A torre reserva uma pista livre e autoriza o aviao.
6. O aviao usa a pista por um tempo simulado.
7. O aviao libera a pista.
8. A visualizacao mostra a evolucao do estado no terminal.

## Estrutura do Projeto

```text
.
├── .github
│   └── workflows
│       └── build.yml
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

Responsabilidades principais:

- `main.c`: inicializacao e encerramento da simulacao.
- `airport.c/.h`: estado global do aeroporto, pistas, configuracao e recursos de sincronizacao.
- `airplane.c/.h`: comportamento das threads de aviao.
- `queue.c/.h`: estrutura de fila circular usada pelas filas de espera.
- `render.c/.h`: visualizacao ASCII no terminal.
- `sync.c/.h`: funcoes auxiliares relacionadas a sincronizacao.

## Dependencias

Para compilar o projeto, e necessario ter:

- `gcc`
- `make`
- suporte a `pthread`
- suporte a semaforos POSIX

Em distribuicoes Linux baseadas em Debian/Ubuntu:

```bash
sudo apt-get update
sudo apt-get install -y build-essential
```

## Como Compilar

Na raiz do repositorio, execute:

```bash
make
```

Isso gera o binario:

```text
aeroporto
```

Para remover arquivos gerados:

```bash
make clean
```

## Como Executar

Depois de compilar:

```bash
./aeroporto
```

No estado atual, o programa inicializa a estrutura base do aeroporto e imprime uma primeira visualizacao no terminal. A simulacao completa das threads de aviao e da torre ainda sera implementada nas proximas etapas.

Tambem e possivel usar:

```bash
make run
```

Esse comando compila o projeto, se necessario, e executa o binario.

## GitHub Actions

O projeto possui um workflow em:

```text
.github/workflows/build.yml
```

Esse workflow executa automaticamente em `push` e `pull_request`, instalando as ferramentas de compilacao e rodando:

```bash
make
```

Assim, o repositorio valida se os arquivos `.c` enviados continuam compilando corretamente.

## Status Atual

O projeto esta na fase de esqueleto inicial.

Ja existem:

- `Makefile`.
- GitHub Actions para validar compilacao.
- Estruturas centrais do aeroporto.
- Tipos de aviao, pista, configuracao e estado.
- Fila circular basica.
- Renderizacao ASCII inicial.
- Stubs para comportamento de aviao e sincronizacao.

Proximas etapas:

- Implementar criacao das threads dos avioes.
- Implementar a thread da torre.
- Implementar entrada nas filas.
- Implementar autorizacao via variaveis de condicao.
- Implementar ocupacao e liberacao de pistas.
- Implementar parametros por linha de comando.
- Melhorar a animacao no terminal.

## Documentacao do Planejamento

Mais detalhes do planejamento estao em:

- `plano.md`: plano de desenvolvimento e design geral.
- `perguntas.md`: respostas aos pontos do enunciado do projeto.
