# Trabalho Prático da Disciplina de Fundamentos e Avaliação de Redes de Computadores - 2026/2
 ** Professor: Dr. Leonardo Bidese de Pinho **

O objetivo geral deste trabalho prático é capacitar a compreensão dos principais conceitos e elementos de redes de computadores modernas através de práticas de programação e avaliação experimental[cite: 1]. O projeto abrange o desenvolvimento de um servidor HTTP com Qualidade de Serviço (QoS) em ambiente Linux, utilizando a Linguagem C e a biblioteca Pthreads, aliado à avaliação de desempenho em diferentes cenários de rede com o uso de ferramentas de análise de pacotes e medição de vazão.

## Servidor HTTP 1.1 com QoS - MVP 1

Primeira versão (MVP 1) do trabalho prático de Fundamentos e Avaliação de Redes de Computadores (2026/2). O objetivo desta entrega é a implementação de um servidor HTTP concorrente em linguagem C utilizando a biblioteca Pthreads, capaz de manter conexões persistentes (Keep-Alive).

## Equipe
* João Oliveira Duarte
* Raul Etcheverry Reis

## Estrutura do Repositório
* `/Servidor`: Contém o código-fonte em C e os arquivos web (HTML/imagens) para teste.
* `/Documentos`: Contém o relatório técnico em formato SBC.
* `/Evidencias`: Contém as capturas de tráfego de rede e pacotes (Wireshark/TCPdump).

## Compilação e Execução
O servidor foi desenvolvido para ambiente Linux. Navegue até a pasta do servidor e utilize o GCC com a flag de ligação aos fios de execução POSIX:

```bash
gcc -Wall -O2 servidor.c -o servidor -lpthread
```

** Para iniciar o servidor (porta padrão 8080): **
```bash
./servidor
```

### Declaração de Autoria
Este projeto foi desenvolvido integralmente pela equipe, sem ajuda não autorizada de alunos não membros do projeto no processo de codificação.
Nota: Ferramentas de Inteligência Artificial foram utilizadas estritamente para auxílio na estruturação sintática da linguagem C e na formatação da documentação técnica.
