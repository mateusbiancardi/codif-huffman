#include "compactador.h"
#include "arvore.h"
#include "bitmap.h"
#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct compactador {
  char *arqEntrada;
  char *arqSaida;
  int frequencias[256];
  Arvore *arvore;
  char *tabelaCodigos[257];
};

static void contaFrequencia(Compactador *c) {

  // le o arquivo em modo read binary
  FILE *arq = fopen(c->arqEntrada, "rb");

  // calcula a frequencia de todos os caracteres
  int caractere;
  while ((caractere = fgetc(arq)) != EOF) {
    c->frequencias[caractere]++;
  }

  fclose(arq);
}

static void constroiArvoreHuffman(Compactador *c) {
  // Implementação da Etapa 2 e 3: Construir lista e montar a árvore
  // ... usa c->frequencias, cria a lista, monta a árvore e salva em
  // c->arvore_huffman ...

  Lista *listaPrioridade = criaLista();

  // Cria nós folhas pra cada caractere e insere em uma lista
  for (int i = 0; i < 256; i++) {
    if (c->frequencias[i] > 0) {
      Arvore *no = criaNoFolha(i, c->frequencias[i]);
      insereItemOrdenado(listaPrioridade, no, comparaFrequencia);
    }
  }

  // criação do nó "end of file" para o descompactador saber quando parar
  Arvore *eof = criaNoFolha(256, 1);
  insereItemOrdenado(listaPrioridade, eof, comparaFrequencia);

  int qntdLista = getQuantidadeItemsLista(listaPrioridade);
  // cria nós internos até sobrar o nó raiz
  while (qntdLista > 1) {
    Arvore *noEsquerdo = removePrimeiroItem(listaPrioridade);
    Arvore *noDireito = removePrimeiroItem(listaPrioridade);

    // cria nó interno e reinsere na lista
    Arvore *interno = criaNoInterno(noEsquerdo, noDireito);
    insereItemOrdenado(listaPrioridade, interno, comparaFrequencia);

    // Retira 2 itens e adiciona 1
    qntdLista--;
  }

  // armazena o nó raiz no compactador
  c->arvore = removePrimeiroItem(listaPrioridade);

  liberaLista(listaPrioridade, NULL);
}

void encontraCaminho(Arvore *noAtual, char caminhoTemporario[],
                     int profundidade, char *tabelaFinal[]) {
  // se for nó folha
  if (ehNoFolha(noAtual)) {
    int caractere = getCaractere(noAtual);
    // adiciona um terminador de string na posição atual
    caminhoTemporario[profundidade] = '\0';
    // copia o caminho para o array de endereços
    tabelaFinal[caractere] = strdup(caminhoTemporario);
    return;
  } else {
    caminhoTemporario[profundidade] = '0';
    encontraCaminho(getEsquerda(noAtual), caminhoTemporario, profundidade + 1,
                    tabelaFinal);
    caminhoTemporario[profundidade] = '1';
    encontraCaminho(getDireita(noAtual), caminhoTemporario, profundidade + 1,
                    tabelaFinal);
  }
};

static void geraTabelaCodigos(Compactador *c) {}

static void escreveArquivoCompactado(Compactador *c) {}

Compactador *criaCompactador(const char *caminho_entrada) {
  Compactador *c = calloc(1, sizeof(Compactador));

  c->arqEntrada = strdup(caminho_entrada);

  // adiciona o .bica para o arquivo compactado
  c->arqSaida = (char *)malloc(strlen(caminho_entrada) + 5); // .bica + \0
  strcpy(c->arqSaida, caminho_entrada);
  strcat(c->arqSaida, ".bica");

  for (int i = 0; i < 256; i++) {
    c->tabelaCodigos[i] = NULL;
  }

  return c;
};

void executaCompactacao(Compactador *c);

void liberaCompactador(Compactador *c);