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

Por padrao, o programa executa a simulacao com animacao ASCII no terminal.

Tambem e possivel usar:

```bash
make run
```

Esse comando compila o projeto, se necessario, e executa o binario.

## Parametros

O programa aceita os seguintes parametros:

```text
--planes N             Numero total de avioes
--runways N            Numero de pistas
--queue-size N         Tamanho maximo das filas
--landing-prob N       Probabilidade de pouso em porcentagem
--emergency-prob N     Probabilidade de emergencia entre pousos
--runway-time-ms N     Tempo de uso da pista em ms
--arrival-delay-ms N   Intervalo entre chegadas em ms
--max-landings N       Maximo de pousos consecutivos antes de priorizar decolagem
--no-animation         Desativa animacao e mostra logs lineares
--help                 Mostra ajuda
```

Exemplo com logs, util para testes:

```bash
./aeroporto --planes 10 --runways 2 --no-animation --runway-time-ms 50 --arrival-delay-ms 5
```

Exemplo com mais carga:

```bash
./aeroporto --planes 30 --runways 3 --landing-prob 70 --emergency-prob 15
```

## GitHub Actions

O projeto possui um workflow em:

```text
.github/workflows/build.yml
```

Esse workflow executa automaticamente em `push` e `pull_request`, instalando as ferramentas de compilacao e rodando:

```bash
make
bash scripts/ci_tests.sh
```

Assim, o repositorio valida se os arquivos `.c` enviados continuam compilando corretamente e se a simulacao basica termina sozinha.

## Testes

Os testes automatizados ficam em:

```bash
scripts/ci_tests.sh
```

Eles executam cenarios rapidos com `--no-animation`, `--runway-time-ms 1` e `--arrival-delay-ms 0`, validando que todos os avioes terminam e que as filas ficam vazias no final.

Para rodar localmente:

```bash
make
bash scripts/ci_tests.sh
```

Tambem existe um script com cenarios mais pesados e tempos maiores, pensado para visualizar a animacao no terminal:

```bash
make
bash scripts/heavy_local_tests.sh
```

## Status Atual

O projeto possui uma primeira simulacao funcional.

Ja existem:

- `Makefile`.
- GitHub Actions para validar compilacao.
- Estruturas centrais do aeroporto.
- Tipos de aviao, pista, configuracao e estado.
- Fila circular basica.
- Renderizacao ASCII no terminal.
- Thread da torre de controle.
- Threads de avioes.
- Uso de mutex, semaforo e variaveis de condicao.
- Parametros por linha de comando.
- Modo `--no-animation` para testes e CI.

Proximas etapas:

- Refinar a animacao para ficar mais didatica no video.
- Adicionar mais cenarios de teste.
- Melhorar mensagens e explicacao visual da politica da torre.
- Ajustar valores padrao conforme a demonstracao final.

## Documentacao do Planejamento

Mais detalhes do planejamento estao em:

- `perguntas.md`: respostas aos pontos do enunciado do projeto.
