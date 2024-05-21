## Instruções de execução do script

Foi criado um script para realizar as tarefas de execução e compilação dos programas auxiliares, juntamente do programa principal.

Para executar o script, basta rodar o comando:

```shell
./gera_resultado matriz1 matriz2 output <numero_threads>
```

no final da execução script, conterá um arquivo result.csv para verificar os valores computados pelo programa.

Caso deseja verificar o resultado do programa, execute com a flag ``diff``  da seguinte maneira:

```shell
./gera_resultado matriz1 matriz2 output <numero_threads> diff
```

Para verificar os resultados com os dados sendo printados no terminal, pode utilizar a flag ```INFO```:

```shell
./gera_resultado matriz1 matriz2 output <numero_threads> INFO
```

O programa ``gera_resultado`` só existe para facilitar a execução do script que realiza toda a tarefa de compilar os programas, com o intuito de automatizar o processo de experimento dos 3 casos de testes solicitados. Caso exista a necessidade de fazer um único teste, basta executar:

```shell
./script matriz1 matriz2 output <numero_threads>
```

podendo combinar os outros parâmetros de ```INFO```e ```diff```citados anteriormente.