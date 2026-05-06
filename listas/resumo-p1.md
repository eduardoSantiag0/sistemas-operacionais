A ideia central é: **como o sistema operacional consegue executar vários programas ao mesmo tempo, isolando suas memórias, alternando a CPU entre eles e permitindo que eles peçam serviços ao kernel?**

## 1. O que é um processo?

Um **processo** é uma instância de um programa em execução.

Um programa parado no disco é apenas um arquivo executável. Quando ele é carregado para executar, o sistema operacional cria um **processo**, que passa a ter:

* código;
* dados;
* heap;
* pilha;
* registradores;
* descritores de arquivos;
* permissões;
* estado de execução;
* espaço de endereçamento virtual;
* uma ou mais threads.

Então, processo não é só “o código rodando”. Processo é o **conjunto de recursos e informações que o sistema operacional usa para controlar uma execução**.

Exemplo:

```txt
programa no disco: /bin/ls
processo: uma execução específica do ls
```

Se você abrir o mesmo programa duas vezes, terá dois processos diferentes, cada um com seu próprio espaço de memória virtual.

## 2. Como um processo é representado no sistema operacional?

O sistema operacional representa cada processo por uma estrutura interna, normalmente chamada de **PCB**, ou **Process Control Block**.

O PCB guarda as informações necessárias para o SO controlar aquele processo.

Em termos didáticos, ele contém:

```txt
- PID;
- estado do processo: pronto, executando, bloqueado etc.;
- registradores salvos;
- contador de programa;
- ponteiro da pilha;
- informações de escalonamento;
- credenciais/permissões;
- sinais pendentes;
- máscara de sinais;
- descritores de arquivos abertos;
- informações de memória virtual;
- ponteiro para a tabela de páginas;
- mapa de memória do processo.
```

A parte de memória virtual se conecta diretamente ao seu texto: cada processo tem seu próprio **espaço de endereçamento virtual**, dividido em regiões como código, dados, heap, bibliotecas, pilhas e áreas mapeadas. O SO mantém as estruturas necessárias para traduzir esses endereços virtuais em endereços físicos.

## 3. O que é memória virtual?

Memória virtual é a abstração que faz com que o processo enxergue um espaço de memória próprio, contínuo e lógico, sem precisar saber onde aquelas páginas estão fisicamente na RAM.

O processo acha que está acessando, por exemplo:

```txt
endereço virtual 0x400000
```

Mas esse endereço não é necessariamente o endereço real na memória física. Ele precisa ser traduzido.

O texto explica justamente isso: as referências feitas pelos processos são relativas a um espaço lógico, e não diretamente à memória física; a tradução é feita pelo hardware em tempo de execução. 

A vantagem é enorme:

```txt
- cada processo fica isolado dos outros;
- programas podem ser carregados em qualquer lugar da memória física;
- nem todas as páginas precisam estar na RAM ao mesmo tempo;
- o SO pode usar RAM e disco para simular um espaço maior;
- fica mais fácil proteger áreas de memória.
```

## 4. Como a memória virtual se encaixa no processo?

Cada processo tem seu próprio **espaço de endereçamento virtual**.

Exemplo simplificado:

```txt
Processo A enxerga:
0x00000000 até 0xffffffff

Processo B também enxerga:
0x00000000 até 0xffffffff
```

Mas isso não significa que os dois estão usando a mesma memória física.

O endereço virtual `0x400000` no Processo A pode apontar para um frame físico, enquanto o mesmo endereço virtual `0x400000` no Processo B pode apontar para outro frame físico.

É por isso que dois processos podem usar endereços virtuais iguais sem interferirem um no outro.

```txt
Processo A:
endereço virtual 0x400000 → frame físico 100

Processo B:
endereço virtual 0x400000 → frame físico 820
```

O que permite isso é a **tabela de páginas de cada processo**.

## 5. Segmentação e paginação

No texto, aparece a ideia de **segmentação paginada**.

Isso significa que há duas etapas conceituais:

```txt
segmentação → forma um endereço linear/virtual
paginação   → traduz esse endereço virtual para endereço físico
```

Na segmentação, o endereço pode ser pensado como algo relativo a uma região, como:

```txt
segmento de código
segmento de dados
segmento de pilha
```

No x86 clássico, por exemplo, o registrador de segmento, como `CS` ou `DS`, indica uma base. O deslocamento, como o `EIP`, é somado a essa base, formando um endereço linear. Depois, esse endereço linear ainda passa pela paginação para chegar ao endereço físico.

De forma simplificada:

```txt
CS + EIP → endereço linear virtual
endereço linear virtual → paginação → endereço físico
```

No caso de acesso a dados:

```txt
DS + offset → endereço linear virtual
endereço linear virtual → paginação → endereço físico
```

## 6. Paginação: página lógica, frame e deslocamento

Na paginação, o espaço virtual do processo é dividido em blocos fixos chamados **páginas**.

A memória física é dividida em blocos do mesmo tamanho, chamados **frames** ou **page frames**.

Exemplo com páginas de 4 KB:

```txt
endereço virtual = número da página lógica + deslocamento
endereço físico  = número do frame físico + mesmo deslocamento
```

O texto menciona que, em arquiteturas x86 com páginas de 4 KB, os **12 bits menos significativos** indicam o deslocamento dentro da página. Isso acontece porque:

```txt
4 KB = 4096 bytes = 2^12
```

Logo, são necessários 12 bits para escolher uma posição dentro da página.

Em endereço de 32 bits:

```txt
20 bits mais significativos → número da página
12 bits menos significativos → deslocamento
```

A tradução troca a página lógica pelo frame físico, mas mantém o deslocamento.

Exemplo:

```txt
Endereço virtual:
[página lógica 25][deslocamento 100]

Tabela de páginas:
página lógica 25 → frame físico 900

Endereço físico:
[frame físico 900][deslocamento 100]
```

O deslocamento não muda porque ele indica a posição interna dentro do bloco. O que muda é o bloco em si: sai a página lógica, entra o frame físico.

## 7. Tabela de páginas

A **tabela de páginas** é a estrutura que diz para onde cada página virtual aponta na memória física.

Ela é criada e mantida pelo sistema operacional, mas usada pela MMU durante a execução.

Exemplo:

```txt
Página virtual 0 → frame físico 120
Página virtual 1 → frame físico 830
Página virtual 2 → ausente da memória
Página virtual 3 → frame físico 451
```

A tabela de páginas também guarda flags, como:

```txt
- página presente ou ausente;
- página acessível pelo usuário ou apenas pelo kernel;
- página somente leitura ou leitura/escrita;
- página acessada;
- página modificada/suja.
```

Essas informações aparecem no texto com nomes como:

```txt
_PAGE_PRESENT
_PAGE_RW
_PAGE_USER
_PAGE_DIRTY
_PAGE_ACCESSED
```

## 8. MMU: quem faz a tradução de endereços?

A **MMU**, ou **Memory Management Unit**, é a unidade de hardware responsável pela tradução de endereços virtuais em endereços físicos.

A MMU fica no processador, ou integrada a ele.

O sistema operacional cria as tabelas de páginas, mas a tradução em si precisa ser rápida demais para depender de software a cada acesso. Por isso, ela é feita por circuito/hardware.

Toda vez que a CPU busca uma instrução ou acessa um dado, há tradução de endereço.

Exemplo:

```txt
CPU quer buscar instrução no endereço virtual 0x400000
↓
MMU consulta tabela de páginas/TLB
↓
descobre o endereço físico correspondente
↓
CPU acessa a RAM física
```

Isso vale tanto para instruções quanto para dados.

```txt
buscar instrução → precisa traduzir endereço
ler variável     → precisa traduzir endereço
escrever dado    → precisa traduzir endereço
```

## 9. TLB: cache de traduções

A **TLB**, ou **Translation Lookaside Buffer**, é um cache de traduções de endereço.

Como consultar a tabela de páginas toda hora seria caro, o processador guarda traduções recentes na TLB.

Exemplo:

```txt
Página virtual 25 → frame físico 900
Página virtual 30 → frame físico 122
Página virtual 31 → frame físico 700
```

Se a CPU precisa traduzir novamente a página virtual 25, ela verifica a TLB. Se estiver lá, a tradução é muito rápida.

```txt
TLB hit  → tradução encontrada rapidamente
TLB miss → precisa consultar a tabela de páginas
```

O texto explica que a TLB é uma espécie de cache mantido automaticamente pelo processador e que acelera referências futuras à memória para o mesmo processo. 

## 10. Troca de contexto: o que o SO precisa salvar?

A **troca de contexto** ocorre quando o sistema operacional tira um processo ou thread da CPU e coloca outro para executar.

Para isso, ele precisa salvar o estado da execução atual e restaurar o estado da próxima.

Informações importantes:

```txt
- registradores;
- contador de programa;
- ponteiro da pilha;
- estado do processo/thread;
- informações de escalonamento;
- ponteiro para a tabela de páginas;
- contexto de memória;
- máscara de sinais;
- sinais pendentes;
- pilha de kernel;
- informações de FPU/SIMD, quando necessário.
```

A conexão com memória virtual é fundamental.

A CPU não “sabe” abstratamente qual processo está rodando. Ela sabe apenas qual tabela de páginas deve usar. Por isso, em uma troca de contexto entre processos, o SO precisa ajustar o registrador que aponta para a tabela de páginas atual.

No x86, esse registrador é classicamente associado ao **CR3**, que aponta para a estrutura principal de paginação do processo.

Então:

```txt
Processo A executando:
CR3 aponta para tabela de páginas do Processo A

troca de contexto

Processo B executando:
CR3 passa a apontar para tabela de páginas do Processo B
```

Essa é a ligação direta entre **processo**, **troca de contexto** e **memória virtual**.

## 11. O que é uma thread?

Uma **thread** é uma unidade de execução dentro de um processo.

Um processo pode ter uma ou várias threads.

O processo é o “contêiner” de recursos. A thread é o fluxo de execução.

Exemplo:

```txt
Processo: navegador

Thread 1 → interface gráfica
Thread 2 → rede
Thread 3 → renderização
Thread 4 → áudio
```

Threads do mesmo processo compartilham o mesmo espaço de endereçamento virtual.

Isso significa que usam a mesma tabela de páginas.

## 12. O que as threads compartilham entre si?

Threads do mesmo processo compartilham:

```txt
- espaço de endereçamento virtual;
- código;
- heap;
- variáveis globais;
- bibliotecas carregadas;
- descritores de arquivos;
- credenciais/permissões;
- PID do processo, em sentido geral;
- mapa de memória.
```

Mas cada thread tem individualmente:

```txt
- pilha própria;
- registradores próprios;
- contador de programa próprio;
- estado de execução próprio;
- identificador próprio de thread;
- máscara de sinais própria, em muitos sistemas.
```

A parte mais importante para conectar com memória virtual é:

> Threads compartilham a mesma memória virtual do processo, mas cada uma tem sua própria pilha dentro desse mesmo espaço de endereçamento.

Exemplo:

```txt
Processo P:
heap compartilhado
globais compartilhadas
código compartilhado

Thread 1:
pilha própria

Thread 2:
pilha própria

Thread 3:
pilha própria
```

Como compartilham memória, se uma thread altera uma variável global, as outras podem ver essa alteração. Por isso, threads exigem mecanismos de sincronização, como mutexes, semáforos e variáveis de condição.

## 13. Diferença entre processo, thread e processo filho

Um **processo** tem seu próprio espaço de endereçamento virtual.

Uma **thread** roda dentro de um processo e compartilha o espaço de endereçamento dele.

Um **processo filho**, criado por `fork()`, é outro processo. Ele não é uma thread.

Depois de `fork()`, temos:

```txt
processo pai
processo filho
```

Eles são processos distintos, com PIDs distintos e espaços de endereçamento distintos.

Mas, por eficiência, sistemas modernos usam **copy-on-write**. Isso significa que, logo após o `fork()`, pai e filho podem apontar para as mesmas páginas físicas, marcadas como somente leitura. Quando um deles tenta modificar uma página, o sistema cria uma cópia.

Exemplo:

```txt
antes da escrita:
pai  → página física X
filho → página física X

filho escreve:
pai  → página física X
filho → cópia da página física X
```

Isso economiza memória.

Já uma thread, criada por algo como `pthread_create()` ou por `clone()` com flags específicas no Linux, compartilha a memória normalmente.

## 14. fork(), clone() e execve()

Essas chamadas se conectam diretamente à ideia de “imagem de processo”.

### `fork()`

Cria um novo processo filho copiando, logicamente, o processo pai.

Depois do `fork()`:

```txt
pai e filho continuam executando a partir do mesmo ponto
mas são processos diferentes
```

O filho recebe uma cópia lógica da imagem do pai.

### `clone()`

No Linux, `clone()` é uma syscall mais flexível. Dependendo das flags, ela pode criar algo parecido com processo ou algo parecido com thread.

Por exemplo:

```txt
clone sem compartilhar memória → comportamento parecido com processo
clone com CLONE_VM            → compartilha espaço de endereçamento
clone com CLONE_THREAD        → integra o mesmo grupo de threads
```

Então, de forma didática:

```txt
fork()  → cria processo filho
clone() → cria execução com compartilhamentos configuráveis
```

### `execve()`

`execve()` não cria um novo processo. Ele **substitui a imagem do processo atual** por outro programa.

Isso responde à pergunta: **como modificar uma imagem de um processo?**

A resposta é:

> A imagem de um processo é substituída por meio de `execve()`.

Depois do `execve()`:

```txt
o PID pode continuar o mesmo
mas o código, dados, heap, stack inicial e mapeamentos mudam
```

Exemplo típico:

```c
fork();
execve("/bin/ls", ...);
```

O `fork()` cria o processo filho. O `execve()` troca a imagem do filho pelo programa `/bin/ls`.

Fluxo comum em shells:

```txt
shell chama fork()
↓
filho chama execve("programa")
↓
pai espera o filho terminar
```

## 15. Como modificar a imagem de um processo?

A imagem do processo é o conjunto formado por:

```txt
- código executável;
- dados;
- heap;
- pilha;
- bibliotecas carregadas;
- mapeamentos de memória.
```

Quando se chama `execve()`, o sistema operacional descarta a antiga imagem de memória do processo e carrega uma nova.

Ou seja:

```txt
antes:
processo executando shell

execve("/bin/ls")

depois:
mesmo processo agora executando ls
```

O PID geralmente permanece, mas o conteúdo do espaço de endereçamento muda.

A memória virtual entra aqui porque o SO precisa criar um novo mapa de memória e novas tabelas de páginas para representar o novo programa.

## 16. Syscalls e ABI

Uma **syscall** é uma chamada de sistema: um pedido que o programa faz ao kernel.

Exemplos:

```txt
read()
write()
open()
close()
fork()
clone()
execve()
mmap()
kill()
```

O programa normalmente não entra no kernel diretamente escrevendo instruções de baixo nível. Ele chama uma função da libc, como:

```c
write(fd, buffer, size);
```

Essa função é uma wrapper. Ela prepara os argumentos e executa a instrução especial que entra no kernel.

A **ABI** entra aqui porque define como a chamada acontece em nível binário.

Há duas convenções relevantes:

```txt
1. ABI de chamada de função:
   programa → libc

2. ABI de syscall:
   libc → kernel
```

A ABI define, por exemplo:

```txt
- quais registradores recebem argumentos;
- qual registrador recebe o número da syscall;
- como o retorno é entregue;
- como erros são representados;
- quais registradores são preservados ou destruídos.
```

Então, em uma syscall, a libc recebe os argumentos segundo a ABI de função comum, reorganiza para a ABI de syscall e executa a entrada no kernel.

## 17. Syscalls e memória virtual

Syscalls também dependem de memória virtual.

Exemplo:

```c
write(1, buffer, 10);
```

O `buffer` é um endereço virtual do processo.

Quando o kernel recebe esse ponteiro, ele precisa acessar a memória do processo chamador. Mas esse endereço não é físico; ele precisa ser validado e traduzido.

O kernel precisa verificar:

```txt
- esse endereço pertence ao processo?
- essa região está mapeada?
- o processo tem permissão para acessá-la?
- a página está presente na memória?
```

Então a memória virtual protege o sistema: o processo não pode simplesmente passar um endereço físico arbitrário e fazer o kernel acessar qualquer lugar da RAM.

## 18. Interrupções

Uma **interrupção** é um evento que desvia a execução normal da CPU para uma rotina de tratamento.

Pode vir de hardware, como:

```txt
- teclado;
- disco;
- placa de rede;
- timer;
- controlador de I/O.
```

A interrupção é tratada pelo kernel.

Fluxo simplificado:

```txt
hardware gera interrupção
↓
CPU consulta tabela de interrupções
↓
CPU entra em modo kernel
↓
kernel executa rotina de tratamento
↓
execução normal é retomada
```

A interrupção é fundamental para escalonamento, porque o timer gera interrupções periódicas. Quando ocorre a interrupção do timer, o kernel pode decidir que a thread atual já executou o suficiente e escolher outra.

## 19. Sinais

**Sinais** são notificações entregues a processos ou threads para informar que algum evento ocorreu.

Exemplos:

```txt
SIGINT  → Ctrl+C
SIGTERM → pedido de término
SIGKILL → término forçado
SIGSEGV → acesso inválido à memória
SIGCHLD → processo filho terminou
```

Um processo pode:

```txt
- executar a ação padrão;
- ignorar o sinal, quando permitido;
- registrar um handler personalizado;
- bloquear temporariamente o sinal com uma máscara.
```

Cuidado com a diferença:

```txt
ignorar sinal  → descartar o sinal
bloquear sinal → adiar temporariamente sua entrega
```

Alguns sinais não podem ser tratados nem ignorados, como:

```txt
SIGKILL
SIGSTOP
```

## 20. Relação entre sinais e memória virtual

Um exemplo claro é o `SIGSEGV`.

Quando um processo tenta acessar uma região de memória inválida, a MMU detecta o problema durante a tradução do endereço.

Exemplo:

```txt
processo tenta acessar endereço virtual inválido
↓
MMU não encontra mapeamento válido ou detecta violação de permissão
↓
ocorre uma exceção/page fault
↓
kernel analisa o caso
↓
se for acesso inválido, envia SIGSEGV ao processo
```

Então, um erro de memória virtual pode se transformar em um sinal entregue ao processo.

Nem todo page fault é erro. Se a página existe, mas está temporariamente fora da RAM, o SO pode buscá-la no disco. Mas se o endereço é inválido ou viola permissão, o resultado pode ser `SIGSEGV`.

## 21. Políticas de escalonamento

O **escalonador** decide qual processo ou thread vai executar na CPU.

Estados típicos:

```txt
executando
pronto
bloqueado
terminado
```

Políticas comuns:

```txt
FCFS/FIFO:
executa por ordem de chegada.

Round Robin:
cada processo/thread recebe uma fatia de tempo.

Prioridades:
tarefas com maior prioridade são escolhidas antes.

Multilevel Queue:
diferentes filas para diferentes tipos de tarefas.

CFS, no Linux:
busca distribuir a CPU de forma justa entre tarefas.
```

O escalonamento se conecta à memória virtual porque, ao trocar de um processo para outro, o SO precisa trocar também o contexto de memória, isto é, a tabela de páginas ativa.

Se a troca for entre threads do mesmo processo, o espaço de endereçamento é o mesmo. Portanto, a troca é mais leve em termos de memória.

```txt
troca entre processos diferentes:
troca registradores + troca tabela de páginas

troca entre threads do mesmo processo:
troca registradores + pilha/PC, mas mantém tabela de páginas
```

## 22. Syscalls, interrupções e sinais: diferença

Esses três conceitos se parecem porque todos podem desviar o fluxo normal de execução. Mas eles não são a mesma coisa.

### Syscall

É uma entrada voluntária no kernel.

```txt
programa pede algo ao SO
```

Exemplo:

```txt
write()
read()
fork()
execve()
```

Normalmente é síncrona: a thread chama e espera o retorno.

### Interrupção

É um evento de hardware ou exceção que força a CPU a chamar uma rotina do kernel.

```txt
hardware ou CPU chama atenção do kernel
```

Exemplo:

```txt
timer
teclado
placa de rede
page fault
```

### Sinal

É uma notificação que o kernel entrega a um processo ou thread.

```txt
SO avisa o processo de algum evento
```

Exemplo:

```txt
SIGINT
SIGTERM
SIGSEGV
SIGCHLD
```

Resumo:

```txt
syscall      → programa chama o kernel
interrupção  → hardware/CPU chama o kernel
sinal        → kernel notifica o processo
```

## 23. Como tudo se encaixa

Imagine que um processo está executando uma instrução.

### Passo 1: a CPU busca a instrução

O contador de programa aponta para um endereço virtual.

```txt
PC/EIP → endereço virtual da próxima instrução
```

### Passo 2: a MMU traduz o endereço

A MMU usa a TLB ou a tabela de páginas do processo atual.

```txt
endereço virtual → MMU/TLB/tabela de páginas → endereço físico
```

### Passo 3: a instrução é carregada e executada

A instrução vai para o registrador interno da CPU e é decodificada.

### Passo 4: a instrução pode acessar dados

Se ela acessa uma variável, outro endereço virtual precisa ser traduzido.

```txt
endereço virtual de dado → endereço físico
```

### Passo 5: pode ocorrer page fault

Se a página não está presente ou a permissão é inválida, a MMU gera uma exceção.

```txt
page fault
↓
kernel trata
↓
pode carregar página do disco
ou enviar SIGSEGV
```

### Passo 6: pode ocorrer syscall

Se o programa chama `read()`, `write()`, `fork()` ou `execve()`, ele entra no kernel por uma syscall.

```txt
programa → libc → ABI de syscall → kernel
```

### Passo 7: pode ocorrer interrupção de timer

O timer interrompe a CPU.

```txt
timer interrupt
↓
kernel entra
↓
escalonador decide se troca a tarefa
```

### Passo 8: pode ocorrer troca de contexto

Se o kernel escolher outro processo:

```txt
salva contexto atual
↓
carrega contexto do próximo
↓
troca registrador da tabela de páginas
↓
retoma execução
```

Se escolher outra thread do mesmo processo:

```txt
salva registradores da thread atual
↓
restaura registradores da próxima thread
↓
mantém o mesmo espaço de endereçamento
```

## 24. Resumo final para estudar

A forma mais fácil de memorizar é esta:

```txt
Processo:
unidade de recursos, com espaço de endereçamento próprio.

Thread:
unidade de execução dentro do processo.

Memória virtual:
faz cada processo enxergar uma memória própria e isolada.

Tabela de páginas:
mapeia páginas virtuais em frames físicos.

MMU:
hardware que traduz endereço virtual em físico.

TLB:
cache de traduções recentes.

Troca de contexto:
SO salva uma execução e restaura outra.

Escalonador:
decide quem executa.

Syscall:
programa pede serviço ao kernel.

ABI:
define como a syscall ocorre em nível binário.

Interrupção:
hardware/CPU chama atenção do kernel.

Sinal:
kernel notifica um processo/thread.

fork():
cria processo filho.

clone():
cria processo/thread conforme flags.

execve():
substitui a imagem do processo.
```

A frase que amarra tudo seria:

> O sistema operacional representa cada processo por estruturas internas que guardam seu estado, seus recursos e suas informações de memória. Cada processo possui um espaço de endereçamento virtual próprio, traduzido pela MMU com base nas tabelas de páginas mantidas pelo SO. Threads de um mesmo processo compartilham esse espaço de endereçamento, mas possuem fluxos de execução próprios. Syscalls permitem que o processo solicite serviços ao kernel; interrupções permitem que hardware e CPU acionem o kernel; sinais permitem que o kernel notifique processos. Na troca de contexto, o SO salva o estado da execução atual, restaura outro contexto e, quando necessário, altera a tabela de páginas ativa para que a MMU passe a traduzir os endereços do novo processo.
