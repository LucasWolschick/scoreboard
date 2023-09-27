# Simulador de Scoreboarding

Este repositório contém a implementação do algoritmo de escalonamento dinâmico de instruções
Scoreboarding baseado em ASM RISC.

O código foi elaborado para a disciplina de Arquitetura e Organização de Computadores II
do curso de Ciência da Computação da Universidade Estadual de Maringá, na turma de 2023.

## Autores
- Guilherme Frare Clemente (ra124349@uem.br)
- Lucas Wolschick (ra123658@uem.br)
- Marcos Vinícius de Oliveira (ra124408@uem.br)

## Compilando

Certifique-se de que possui o compilador `gcc` instalado e o GNU `make`.
No diretório raiz do repositório, execute:

    $ make

Para limpar os arquivos, execute `make clean`.

## Executando

Após compilar o programa, execute:

    $ ./scoreboard -p NOME_DO_PROGRAMA -m TAMANHO_DA_MEMÓRIA [-o ARQUIVO_DE_SAIDA]

Caso a flag `-o` seja especificada, o simulador irá salvar a saída da simulação
no arquivo especificado. Caso contrário, ele será executado de modo interativo
no console. Para executar o próximo ciclo de clock, aperte `Enter`. Quando finalizada
a simulação o programa sairá automaticamente.

## Programas exemplo

O diretório `test/` possui um conjunto de dez programas implementados em assembly que
podem ser executados no simulador.

1. `01_war.sb` - contém um exemplo de uma dependência WAR.
2. `02_waw.sb` - contém um exemplo de uma dependência WAW.
3. `03_raw.sb` - contém um exemplo de uma dependência RAW.
4. `04_todas_deps.sb` - contém exemplos de todas as dependências de dados.
5. `05_fatorial.sb` - calcula o fatorial de um dado número iterativamente.
6. `06_busca_binaria.sb` - realiza uma busca binária sobre um vetor ordenado por um dado
    elemento.
7. `07_aula.sb` - programa-exemplo visto em sala de aula e presente no material da
    disciplina.
8. `08_mult_matriz.sb` - realiza a multiplicação de duas matrizes NxN.
9. `09_primo.sb` - testa se um dado número natural é um número primo.
10. `10_insertionsort.sb` - implementa o algoritmo de ordenação insertion sort e o
    executa sobre um vetor de elementos.

Os programas são prefixados com um comentário que parametriza o número de unidades
funcionais e a quantidade de ciclos de clock que cada instrução leva para executar.

## Notas sobre a simulação

O simulador implementa o algoritmo de escalonamento dinâmico de instruções Scoreboarding,
conforme descrito para a máquina CDC 6600, com alguns detalhes:

 - A simulação do barramento é aproximada mas buscamos torná-la condizente com a
    realidade. O CDC 6600 original agrupava unidades funcionais em grupos que compartilhavam
    um mesmo conjunto de barramentos de leitura de operandos e escrita de resultados, e
    apenas uma unidade funcional por grupo podia usar os barramentos por ciclo.

    Implementamos algo semelhante: dividimos as unidades funcionais em grupos de acordo
    com o parâmetro definido em `consts.h` `N_UF_PER_GROUP`, que especifica o número
    de unidades funcionais por grupo. A cada ciclo, apenas uma unidade funcional pode
    usar o barramento de leitura e escrita.

 - O simulador imprime o estado da memória antes e depois da execução dos programas.