# Plano de Execucao

## 1. Implementar a thread da torre

- Criar `src/tower.c` e `src/tower.h`.
- Definir `TowerThreadArgs`.
- Implementar loop principal da torre.
- Esperar em `pthread_cond_wait(&tower_event, &mutex)` quando nao houver trabalho.
- Escolher proximo aviao pela politica: emergencia, pouso, decolagem.
- Aplicar limite de pousos consecutivos antes de liberar decolagem.
- Sinalizar o aviao escolhido via `pthread_cond_signal`.

## 2. Implementar a thread dos avioes

- Completar `airplane_thread`.
- Fazer o aviao entrar na fila adequada.
- Inicializar e destruir `pthread_cond_t authorized` para cada aviao.
- Bloquear o aviao enquanto `cleared == false`.
- Simular uso da pista.
- Ao terminar, liberar pista e avisar a torre.
- Marcar estado final como `PLANE_DONE`.

## 3. Conectar semaforo, mutex e variaveis de condicao

- Usar `airport->mutex` para todo acesso ao estado compartilhado.
- Usar `airport->tower_event` para acordar a torre.
- Usar `airplane->authorized` para acordar aviao especifico.
- Usar `sem_t available_runways` para limitar uso simultaneo das pistas.
- Revisar todos os `pthread_cond_wait` para usar `while`, nao `if`.

## 4. Gerenciar pistas

- Criar funcoes em `airport.c`:
  - `airport_find_free_runway`
  - `airport_assign_runway`
  - `airport_release_runway`
- Marcar `occupied_by` com o ID do aviao.
- Registrar se a operacao e pouso ou decolagem.
- Garantir que a pista so seja liberada pelo aviao que a ocupa.
- Atualizar contadores de pouso/decolagem.

## 5. Criar os avioes no `main`

- Alocar vetor de `Airplane`.
- Alocar vetor de `pthread_t`.
- Configurar ID, tipo de requisicao e emergencia.
- Criar thread da torre.
- Criar threads dos avioes com intervalo pequeno entre chegadas.
- Fazer `pthread_join` em todos os avioes.
- Encerrar torre com `running = false`.
- Sinalizar `tower_event` para destravar encerramento.
- Fazer `pthread_join` da torre.

## 6. Melhorar o render

- Mostrar IDs nas filas, nao so quantidade.
- Mostrar pistas com aviao e operacao atual.
- Mostrar contadores:
  - pousos concluidos
  - decolagens concluidas
  - emergencias atendidas
  - avioes finalizados
- Mostrar politica:
  - pousos consecutivos atuais
  - limite configurado
- Opcional: criar thread de renderizacao com atualizacao periodica.

## 7. Adicionar parametros por linha de comando

- Implementar parser simples em `main.c`.
- Suportar flags:
  - `--planes N`
  - `--runways N`
  - `--queue-size N`
  - `--landing-prob N`
  - `--emergency-prob N`
  - `--runway-time-ms N`
  - `--arrival-delay-ms N`
  - `--max-landings N`
  - `--no-animation`
- Validar valores invalidos.
- Adicionar `--help`.

## 8. Criar modo para CI/teste

- Implementar `--no-animation`.
- Implementar execucao finita.
- Garantir que o programa termina sozinho.
- Atualizar GitHub Actions para:

```bash
make
./aeroporto --planes 10 --runways 2 --no-animation
```

- Opcional: adicionar timeout no Actions:

```bash
timeout 10s ./aeroporto --planes 10 --runways 2 --no-animation
```

## 9. Melhorar tratamento de erro

- Revisar `airport_init`.
- Evitar destruir mutex/cond/semaforo que nao foram inicializados.
- Usar flags internas ou inicializacao em etapas.
- Checar retorno de:
  - `pthread_create`
  - `pthread_join`
  - `pthread_mutex_init`
  - `pthread_cond_init`
  - `sem_init`
  - `calloc`
- Retornar mensagens claras no `stderr`.

## 10. Atualizar README conforme implementacao real

- Documentar flags reais.
- Adicionar exemplos de execucao.
- Explicar a politica da torre.
- Explicar como interpretar a animacao.
- Explicar como rodar o teste basico.
- Atualizar status do projeto de "esqueleto" para "simulacao funcional".
- Incluir secao sobre uso de LLM, se for usado no relatorio.

## Ordem Recomendada

1. Pontos 1 a 5: nucleo funcional da simulacao.
2. Ponto 8: garantir execucao automatica e testavel.
3. Ponto 6: melhorar visualizacao.
4. Ponto 7: parametros.
5. Ponto 9: robustez.
6. Ponto 10: documentacao final.

## Status de Implementacao

- Pontos 1 a 5: implementados em uma primeira versao funcional.
- Ponto 6: renderizacao melhorada com filas, pistas, estatisticas e politica da torre.
- Ponto 7: parametros por linha de comando implementados.
- Ponto 8: GitHub Actions executa compilacao e uma simulacao finita.
- Ponto 9: tratamento de erro inicial reforcado para criacao de threads, argumentos, alocacoes e destruicao parcial do aeroporto.
- Ponto 10: README atualizado com compilacao, execucao, parametros e CI.
