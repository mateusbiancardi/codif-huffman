/*
 *
 * Tad Descompactador
 * Responsável por orquestrar todo o processo de descompactação de arquivos
 * Autores: Mateus Biancardi e Rafaela Capovilla
 *
 */

#include "descompactador.h"
#include "arvore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct descompactador {
  char *arqEntrada;
  char *arqSaida;
  Arvore *arvore;
};

Descompactador *criaDescompactador(const char *caminho_entrada) {
  Descompactador *d = calloc(1, sizeof(Descompactador));
  if (d == NULL) {
    exit(1);
  }

  // duplica a string do caminho de entrada pro descompactador ter sua própria
  // cópia
  d->arqEntrada = strdup(caminho_entrada);

  // verfica se tem a extensão .comp, se tiver, aloca memoria pro novo nome (sem
  // o .comp)
  size_t len = strlen(caminho_entrada);
  if (len > 5 && strcmp(caminho_entrada + len - 5, ".comp") == 0) {
    d->arqSaida =
        (char *)malloc(len - 4); // len - 5 para remover ".comp", +1 para '\0'
    if (d->arqSaida == NULL) {
      free(d->arqEntrada);
      free(d);
      exit(1);
    }
    strncpy(d->arqSaida, caminho_entrada, len - 5);
    d->arqSaida[len - 5] = '\0';
  } else {
    // se n tiver a extensão .comp, apenas copia o nome
    d->arqSaida = strdup(caminho_entrada);
  }

  d->arvore = NULL;

  return d;
}

static int leProximoBit(FILE *arq) {
  static int byte_atual = 0;
  static int contador_bits = 0;

  // se já lemos todos os 8 bits do byte atual, leia um novo byte do arquivo
  if (contador_bits == 0) {
    byte_atual = fgetc(arq);
    if (byte_atual == EOF) {
      return EOF;
    }
    contador_bits = 8;
  }

  // extrai o bit mais significativo do byte atual
  int bit = (byte_atual >> 7) & 1;

  // prepara o byte para a próxima leitura, deslocando os bits para a esquerda
  byte_atual = byte_atual << 1;

  contador_bits--;

  return bit;
}

static Arvore *leCabecalho(FILE *arq) {
  int tipo_bit = leProximoBit(arq);

  if (tipo_bit == EOF) {
    return NULL;
  }

  // nó folha
  if (tipo_bit == 1) {
    int bit_tipo_folha = leProximoBit(arq); // lê o bit extra
    if (bit_tipo_folha == 1) {
      return criaNoFolha(256, 0);
    } else {
      int caractere = 0;
      // lê os próximos 8 bits para reconstruir o caractere
      for (int i = 0; i < 8; i++) {
        int bit = leProximoBit(arq);
        if (bit == EOF)
          return NULL;
        caractere = (caractere << 1) | bit;
      }
      return criaNoFolha(caractere, 0);
    }
  } else {
    Arvore *esquerda = leCabecalho(arq);
    Arvore *direita = leCabecalho(arq);
    return criaNoInterno(esquerda, direita);
  }
}

// descompacta os dados do arquivo original paro arquivo de saída
static void descompactaDados(Descompactador *d, FILE *arq_entrada,
                             FILE *arq_saida) {
  if (!d || !d->arvore)
    return;

  Arvore *noAtual = d->arvore;
  int bit;

  // le um bit de cada vez
  while ((bit = leProximoBit(arq_entrada)) != EOF) {

    // Navega na árvore: 0 = esquerda, 1 = direita
    if (bit == 0) {
      noAtual = getEsquerda(noAtual);
    } else {
      noAtual = getDireita(noAtual);
    }

    // se chegou numa folha, encontrou um caractere
    if (ehNoFolha(noAtual)) {
      // se for EOF, para a descompactação
      if (getCaractere(noAtual) == 256) { // EOF
        break;
      }
      // escreve o caractere no arquivo de saída
      fputc(getCaractere(noAtual), arq_saida);
      // volta para a raiz da árvore para continuar
      noAtual = d->arvore;
    }
  }
}

void executaDescompactacao(Descompactador *d) {
  if (!d)
    return;

  // abre o arquivo pra leitura binaria
  FILE *arq_entrada = fopen(d->arqEntrada, "rb");
  if (arq_entrada == NULL) {
    exit(1);
  }

  // le o cabeçalho e reconstroi a arvore
  d->arvore = leCabecalho(arq_entrada);

  FILE *arq_saida =
      fopen(d->arqSaida, "wb"); // abre um novo arquivo pra escrever binario
  if (arq_saida == NULL) {
    fclose(arq_entrada);
    exit(1);
  }

  descompactaDados(d, arq_entrada, arq_saida);

  fclose(arq_saida);
  fclose(arq_entrada);
}

void liberaDescompactador(Descompactador *d) {
  if (d != NULL) {
    free(d->arqEntrada);
    free(d->arqSaida);
    liberaArvore(d->arvore);
    free(d);
  }
}