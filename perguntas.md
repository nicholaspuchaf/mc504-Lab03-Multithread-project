# Perguntas e Pontos do Projeto

## Qual problema sera abordado?

O projeto abordara a sincronizacao de um aeroporto com pistas compartilhadas. Avioes podem solicitar pouso ou decolagem, enquanto uma torre de controle coordena o acesso as pistas disponiveis.

As pistas sao recursos limitados e compartilhados. Por isso, a torre precisa decidir quais avioes podem usar uma pista em cada momento, respeitando prioridades e evitando que alguma classe de aviao fique esperando indefinidamente.

## O problema escolhido permite boa exploracao dos conceitos de sincronizacao?

Sim. O problema envolve varios elementos classicos de sincronizacao:

- Multiplas threads concorrentes, representando avioes independentes.
- Recursos limitados, representados pelas pistas.
- Filas de espera para pouso, decolagem e emergencia.
- Prioridade entre requisicoes.
- Exclusao mutua para proteger o estado global.
- Semaforos para controlar a quantidade de pistas disponiveis.
- Variaveis de condicao para bloquear avioes ate a autorizacao da torre.
- Possibilidade de starvation, especialmente se pousos tiverem prioridade absoluta.

Esses elementos tornam o problema adequado para aplicar conceitos de sistemas operacionais de forma clara.

## A animacao permite compreender o problema e a evolucao da execucao?

Sim. A animacao sera exibida no terminal com caracteres ASCII e mostrara o estado global da simulacao.

Ela devera exibir:

- Pistas livres ou ocupadas.
- Qual aviao esta em cada pista.
- Se o aviao esta pousando ou decolando.
- Fila de pouso.
- Fila de decolagem.
- Fila de emergencias.
- Contadores de pousos, decolagens e emergencias atendidas.
- Estado da politica de prioridade da torre.

Com isso, sera possivel acompanhar a evolucao da execucao sem depender apenas de logs textuais isolados.

## A implementacao permite variacao de parametros?

Sim. A implementacao sera planejada para aceitar parametros configuraveis, como:

- Numero total de avioes.
- Numero de pistas.
- Tamanho maximo das filas.
- Probabilidade de pouso.
- Probabilidade de decolagem.
- Probabilidade de emergencia.
- Tempo medio de uso da pista.
- Intervalo de chegada dos avioes.
- Numero maximo de pousos consecutivos antes de liberar uma decolagem.

Esses parametros permitirao observar diferentes comportamentos da simulacao.

## A abordagem proposta e criativa?

Sim. O uso de uma torre de controle com prioridades e politica anti-starvation torna a simulacao mais rica que uma simples disputa por recurso unico.

A representacao no terminal tambem permite criar uma visualizacao clara do aeroporto, incluindo pistas, filas e contadores. O tema permite demonstrar conceitos de sincronizacao de uma forma intuitiva, pois a disputa por pistas e a prioridade de pousos de emergencia sao situacoes faceis de entender.

## Como o repositorio sera organizado?

A organizacao planejada e:

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

Essa separacao ajuda a manter responsabilidades claras entre simulacao, threads de aviao, filas, renderizacao e sincronizacao.

## O README contera quais informacoes?

O `README.md` devera conter:

- Descricao do problema.
- Explicacao das entidades da simulacao.
- Conceitos de sincronizacao usados.
- Como compilar.
- Como executar.
- Parametros aceitos.
- Exemplos de execucao.
- Explicacao da animacao.
- Decisoes de projeto.
- Limites conhecidos.
- Registro do uso de LLM, se aplicavel.

## Como o codigo sera mantido claro?

O codigo sera dividido por responsabilidade, evitando concentrar toda a simulacao em um unico arquivo.

As funcoes devem ter nomes diretos, por exemplo:

- `airport_init`
- `airport_destroy`
- `airplane_thread`
- `tower_thread`
- `render_airport`
- `queue_push`
- `queue_pop`

Comentarios serao usados principalmente em trechos de sincronizacao, especialmente onde houver `pthread_cond_wait`, escolha de prioridade ou politica anti-starvation.

## Quais boas praticas serao respeitadas?

As principais boas praticas previstas sao:

- Verificar erros de criacao de threads, mutexes, condicoes e semaforos.
- Destruir recursos de sincronizacao ao final.
- Evitar dormir enquanto segura mutex.
- Usar `while` em esperas com variaveis de condicao.
- Proteger todo acesso ao estado compartilhado.
- Separar configuracao, estado global, filas e renderizacao.
- Manter o `Makefile` simples e reprodutivel.
- Testar diferentes configuracoes.

## Como o video deve apresentar o projeto?

O video deve apresentar o projeto como uma simulacao concorrente de um aeroporto com pistas compartilhadas. A apresentacao pode ser organizada nos seguintes pontos.

### Descricao breve do problema do aeroporto

O problema consiste em controlar o uso de pistas de um aeroporto por avioes que querem pousar ou decolar. Como o numero de pistas e limitado, nem todos os avioes podem usar uma pista ao mesmo tempo.

O sistema precisa decidir quais avioes recebem autorizacao, respeitando prioridades. Pousos sao mais urgentes que decolagens, e pousos de emergencia sao ainda mais importantes. Ao mesmo tempo, a politica da torre nao pode deixar as decolagens esperando para sempre.

Essa situacao representa um problema classico de sincronizacao: varias threads competem por recursos compartilhados limitados e precisam ser coordenadas para evitar condicoes de corrida, deadlocks e starvation.

### Explicacao das entidades: avioes, torre, pistas e filas

As principais entidades do projeto sao:

- Avioes: cada aviao e representado por uma thread. Um aviao pode solicitar pouso ou decolagem. Se for um pouso, ele pode ou nao estar em emergencia.
- Torre de controle: e uma thread dedicada que coordena a simulacao. Ela observa as filas, escolhe o proximo aviao e autoriza o uso de uma pista.
- Pistas: nao sao threads. Elas sao recursos compartilhados e limitados. Cada pista pode estar livre ou ocupada por um aviao.
- Filas: representam os avioes que estao esperando autorizacao. Existem filas separadas para emergencia, pouso normal e decolagem.

Essa separacao foi escolhida porque avioes e torre sao entidades ativas, enquanto pistas sao recursos. Isso deixa o modelo mais proximo dos conceitos de sistemas operacionais: threads competem por recursos compartilhados controlados por mecanismos de sincronizacao.

### Explicacao da politica de prioridade

A torre usa uma politica de prioridade simples e visualmente compreensivel:

1. Avioes em emergencia tem prioridade maxima.
2. Depois, pousos normais tem prioridade sobre decolagens.
3. Decolagens sao liberadas quando nao ha pousos aguardando ou quando o limite de pousos consecutivos e atingido.

O parametro `--max-landings` define quantos pousos consecutivos podem ocorrer antes de a torre tentar liberar uma decolagem. Essa regra evita starvation, ou seja, evita que avioes aguardando decolagem fiquem bloqueados indefinidamente quando ha muitos pousos chegando.

No video, essa politica pode ser mostrada observando a area "Politica da torre" na animacao, especialmente o contador de pousos consecutivos.

### Explicacao dos mecanismos de sincronizacao usados

O projeto usa tres mecanismos principais de sincronizacao:

- Mutex (`pthread_mutex_t`): protege o estado global do aeroporto, incluindo filas, pistas, contadores e estado dos avioes.
- Semaforo (`sem_t`): representa a quantidade de pistas disponiveis. O valor inicial do semaforo e o numero de pistas.
- Variaveis de condicao (`pthread_cond_t`): fazem as threads esperarem por eventos especificos. A torre espera quando nao ha avioes aguardando; cada aviao espera ate receber autorizacao da torre.

O fluxo de sincronizacao e:

1. O aviao entra na fila apropriada segurando o mutex.
2. O aviao sinaliza a torre com uma variavel de condicao.
3. O aviao dorme esperando sua propria variavel de condicao.
4. A torre acorda, escolhe um aviao e reserva uma pista.
5. A torre sinaliza a variavel de condicao daquele aviao.
6. O aviao usa a pista por um tempo simulado.
7. Ao terminar, o aviao libera a pista e sinaliza a torre novamente.

Esse desenho evita que duas threads alterem o estado global ao mesmo tempo e garante que o numero de avioes usando pistas nunca ultrapasse o numero de pistas configurado.

### Demonstracao da animacao no terminal

O video deve mostrar a animacao ASCII sendo executada no terminal. Um comando sugerido e:

```bash
make
./aeroporto --planes 24 --runways 2 --landing-prob 65 --emergency-prob 15 --runway-time-ms 900 --arrival-delay-ms 240 --max-landings 3
```

Durante a demonstracao, vale destacar os seguintes elementos da tela:

- Parametros escolhidos para a execucao.
- Pistas livres ou ocupadas.
- Avioes em cada pista.
- Filas de emergencia, pouso e decolagem.
- Estatisticas de pousos, decolagens e emergencias.
- Contador de pousos consecutivos da politica anti-starvation.

Tambem pode ser usado o script local:

```bash
bash scripts/heavy_local_tests.sh
```

Esse script executa cenarios mais longos e com tempos maiores, facilitando a visualizacao da animacao.

### Execucao com parametros diferentes

O video deve apresentar pelo menos uma segunda execucao com parametros diferentes para mostrar que a simulacao e configuravel.

Um exemplo interessante e uma execucao com apenas uma pista, pois aumenta a disputa pelo recurso compartilhado:

```bash
./aeroporto --planes 30 --runways 1 --landing-prob 70 --emergency-prob 20 --runway-time-ms 1000 --arrival-delay-ms 180 --max-landings 3
```

Outro exemplo possivel e forcar muitos pousos de emergencia:

```bash
./aeroporto --planes 25 --runways 2 --landing-prob 90 --emergency-prob 55 --runway-time-ms 900 --arrival-delay-ms 200 --max-landings 4
```

Essas execucoes ajudam a mostrar como a politica da torre reage a cenarios diferentes.

### Comentario sobre desafios de implementacao e testes

O video deve comentar alguns desafios importantes:

- Garantir que nenhuma pista seja ocupada por mais de um aviao ao mesmo tempo.
- Evitar deadlock entre torre, avioes e pistas.
- Evitar starvation de decolagens quando ha muitos pousos.
- Proteger corretamente o estado compartilhado com mutex.
- Usar variaveis de condicao com `while`, pois a thread pode acordar sem que a condicao esperada esteja satisfeita.
- Encerrar a simulacao de forma limpa depois que todos os avioes terminam.
- Criar um modo `--no-animation` para testes automatizados no GitHub Actions.

Tambem e importante mencionar que o projeto possui testes automatizados em:

```bash
bash scripts/ci_tests.sh
```

Esses testes validam compilacao, execucao finita, cenario com uma pista e cenario com emergencias. No GitHub Actions, eles ajudam a detectar problemas como travamentos, filas que nao esvaziam ou avioes que nao concluem a simulacao.

## Quais sao as principais decisoes do grupo?

As principais decisoes planejadas sao:

- Implementar a simulacao em C usando `pthreads`.
- Usar semaforos POSIX para representar pistas disponiveis.
- Usar mutex para proteger o estado global do aeroporto.
- Usar variaveis de condicao para bloquear avioes ate autorizacao da torre.
- Usar uma thread dedicada para a torre de controle.
- Usar visualizacao ASCII no terminal, sem bibliotecas graficas.
- Dar prioridade a pousos, especialmente emergencias.
- Implementar uma regra anti-starvation para decolagens.

## Como sera registrada a utilizacao de LLM?

Caso ferramentas de LLM sejam usadas, o README ou um arquivo separado devera registrar:

- Quais perguntas foram feitas.
- Quais sugestoes foram aproveitadas.
- Quais sugestoes foram alteradas ou rejeitadas.
- Qual foi a contribuicao humana na decisao final.
- Analise critica sobre a qualidade das respostas.

Neste momento, a LLM foi usada para ajudar a interpretar o enunciado e estruturar o planejamento inicial do projeto.
