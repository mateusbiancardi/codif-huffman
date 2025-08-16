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
  char *arqEntrada; // arquivo de entrada compactado
  char *arqSaida;   // arquivo de saída descompactado
  Arvore *arvore;
};

Descompactador *criaDescompactador(const char *caminho_entrada) {
  Descompactador *d = calloc(1, sizeof(Descompactador));
  if (d == NULL) {
    perror("Erro ao alocar memória para o descompactador");
    exit(EXIT_FAILURE);
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
      perror("Erro ao alocar memória para o nome do arquivo de saída");
      free(d->arqEntrada);
      free(d);
      exit(EXIT_FAILURE);
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

// faz a leitura en pre ordem
static Arvore *leCabecalho(FILE *arq) {
  int tipo = fgetc(arq);

  if (tipo == EOF) {
    return NULL; // fim do arquivo inesperado
  }

  if (tipo == '1') { // no folha
    int caractere = fgetc(arq);
    if (caractere == EOF)
      return NULL;
    return criaNoFolha(caractere, 0); // cria um novo nó com caracter lido
  } else if (tipo == '0') {           // no interno
    Arvore *esquerda = leCabecalho(arq);
    Arvore *direita = leCabecalho(arq);
    return criaNoInterno(esquerda,
                         direita); // conecta os nós filhos e retorna o nó pai
  }

  return NULL; // nó invalido
}

// descompacta os dados do arquivo original paro arquivo de saída
static void descompactaDados(Descompactador *d, FILE *arq_orig,
                             FILE *arq_saida) {
  if (!d || !d->arvore)
    return;

  Arvore *noAtual = d->arvore;
  int byte_lido;

  while ((byte_lido = fgetc(arq_orig)) != EOF) { // le o arquivo byte por byte
    for (int i = 7; i >= 0; i--) { // itera sobre cada um dos 8 bits do byte
                                   // lido
      int bit = (byte_lido >> i) & 1; // extrai o bit mais significativo
                                      // primeiro

      if (bit == 0) {
        noAtual = getEsquerda(noAtual); // navega na esquerda
      } else {
        noAtual = getDireita(noAtual); // navega na direita
      }

      if (ehNoFolha(noAtual)) {
        if (getCaractere(noAtual) == 256) { // se for pseudo-caractere EOF
          return;                           // enecerra
        }
        fputc(getCaractere(noAtual),
              arq_saida);    // escreve o caracter no arquivo de saída
        noAtual = d->arvore; // reinicia a navegação a partir da raiz
      }
    }
  }
}

void executaDescompactacao(Descompactador *d) {
  if (!d)
    return;

  FILE *arq_entrada =
      fopen(d->arqEntrada, "rb"); // abre o arquivo pra leitra binaria
  if (arq_entrada == NULL) {
    perror("Erro ao abrir arquivo de entrada");
    exit(EXIT_FAILURE);
  }

  // le o cabeçalho e reconstoi a arvore
  d->arvore = leCabecalho(arq_entrada);

  FILE *arq_saida =
      fopen(d->arqSaida, "wb"); // abre um nobo arquivo pra escrever binario
  if (arq_saida == NULL) {
    perror("Erro ao abrir arquivo de saída");
    fclose(arq_entrada);
    exit(EXIT_FAILURE);
  }

  // descompacta os dados
  descompactaDados(d, arq_entrada, arq_saida);

  // fecha os arquivos
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