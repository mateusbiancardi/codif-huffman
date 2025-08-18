/*
 *
 * Tad Compactador
 * Responsável por orquestrar todo o processo de compactação de arquivos
 * Autores: Mateus Biancardi e Rafaela Capovilla
 *
 */

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

  if (arq == NULL) {
    exit(1);
  }

  // calcula a frequencia de todos os caracteres
  int caractere;
  while ((caractere = fgetc(arq)) != EOF) {
    c->frequencias[caractere]++;
  }

  fclose(arq);
}

static void constroiArvoreHuffman(Compactador *c) {

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
    // remove os dois primeiros itens da lista
    Arvore *noEsquerdo = removePrimeiroItem(listaPrioridade);
    Arvore *noDireito = removePrimeiroItem(listaPrioridade);

    // cria nó interno e reinsere na lista
    Arvore *interno = criaNoInterno(noEsquerdo, noDireito);
    insereItemOrdenado(listaPrioridade, interno, comparaFrequencia);

    // retira 2 itens e adiciona 1
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

static void geraTabelaCodigos(Compactador *c) {
  // buffer temporario pra armazenar o codigo binario
  char caminhoTemporario[257];
  for (int i = 0; i < 257; i++) {
    c->tabelaCodigos[i] = NULL;
  }

  // percorre a árvore e encontra os caminhos para preencher a tabela de códigos
  encontraCaminho(c->arvore, caminhoTemporario, 0, c->tabelaCodigos);
}

static void escreveCabecalho(Arvore *a, bitmap *bm) {
  if (ehNoFolha(a)) {
    // escreve 1 para indicar nó folha
    bitmapAppendLeastSignificantBit(bm, 1);

    int caractere = getCaractere(a);
    // se é o EOF
    if (caractere == 256) {
      // escreve 1 para indicar o EOF
      bitmapAppendLeastSignificantBit(bm, 1);
    } else {
      // escreve 0 para indicar caractere normal
      bitmapAppendLeastSignificantBit(bm, 0);
      // escreve os 8 bits do caractere
      for (int i = 7; i >= 0; i--) {
        bitmapAppendLeastSignificantBit(bm, (caractere >> i) & 1);
      }
    }
  } else {
    // escreve 0 indicando nó externo
    bitmapAppendLeastSignificantBit(bm, 0);
    escreveCabecalho(getEsquerda(a), bm);
    escreveCabecalho(getDireita(a), bm);
  }
}

static void escreveArquivoCompactado(Compactador *c) {
  FILE *arqSaida = fopen(c->arqSaida, "wb"); // abre binario
  if (arqSaida == NULL) {
    exit(1);
  }

  // abre o arquivo pra saber o tamanho dele
  FILE *arqParaMedir = fopen(c->arqEntrada, "rb");
  if (arqParaMedir == NULL) {
    exit(1);
  }

  // busca o final do arquivo e conta quantos bytes tem
  fseek(arqParaMedir, 0, SEEK_END);
  long tamanhoArquivoBytes = ftell(arqParaMedir);

  fclose(arqParaMedir);

  // descobre o tamanho máximo do bitmap
  unsigned int tamanhoMaxBits = (tamanhoArquivoBytes * 8) + (512 * 8);
  bitmap *bm = bitmapInit(tamanhoMaxBits);

  escreveCabecalho(c->arvore, bm);

  FILE *arqOriginal = fopen(c->arqEntrada, "rb");
  if (arqOriginal == NULL) {
    exit(1);
  }

  // le cada caractere do arquivo original e escreve seu código binário no
  // bitmap
  int caractere;
  while ((caractere = fgetc(arqOriginal)) != EOF) {
    char *codigo = c->tabelaCodigos[caractere];
    for (int i = 0; codigo[i] != '\0'; i++) {
      bitmapAppendLeastSignificantBit(bm, codigo[i] - '0');
    }
  }
  fclose(arqOriginal);

  // escreve o eof no final
  char *eofCodigo = c->tabelaCodigos[256];
  for (int i = 0; eofCodigo[i] != '\0'; i++) {
    bitmapAppendLeastSignificantBit(bm, eofCodigo[i] - '0');
  }

  // calcula quantos bytes completos precisam ser escritos
  unsigned int totalBytes = (bitmapGetLength(bm) + 7) / 8;
  fwrite(bitmapGetContents(bm), sizeof(unsigned char), totalBytes, arqSaida);

  bitmapLibera(bm);
  fclose(arqSaida);
}

Compactador *criaCompactador(const char *caminho_entrada) {
  Compactador *c = calloc(1, sizeof(Compactador));

  // copia do caminho de entrada
  c->arqEntrada = strdup(caminho_entrada);

  // adiciona o .comp para o arquivo compactado
  c->arqSaida = (char *)malloc(strlen(caminho_entrada) + 5); // .comp + \0
  strcpy(c->arqSaida, caminho_entrada);
  strcat(c->arqSaida, ".comp");

  for (int i = 0; i < 256; i++) {
    c->tabelaCodigos[i] = NULL;
  }

  return c;
};

void executaCompactacao(Compactador *c) {
  contaFrequencia(c);

  constroiArvoreHuffman(c);

  geraTabelaCodigos(c);

  escreveArquivoCompactado(c);
}

void liberaCompactador(Compactador *c) {
  if (c == NULL) {
    return;
  }

  free(c->arqEntrada);
  free(c->arqSaida);
  liberaArvore(c->arvore);

  // pra cada ponteiro nao nulo, libera a string alocada
  for (int i = 0; i < 257; i++) {
    if (c->tabelaCodigos[i] != NULL) {
      free(c->tabelaCodigos[i]);
    }
  }

  free(c);
}