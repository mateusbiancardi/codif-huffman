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
    perror("Erro ao abrir arquivo de entrada em contaFrequencia");
    exit(EXIT_FAILURE);
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
    bitmapAppendLeastSignificantBit(bm, 1); // Continua sendo 1 para folha

    int caractere = getCaractere(a);
    // --- ALTERAÇÃO AQUI ---
    if (caractere == 256) {                   // Se for a folha EOF
      bitmapAppendLeastSignificantBit(bm, 1); // Escreve o bit '1' especial
    } else {
      bitmapAppendLeastSignificantBit(bm, 0); // Bit '0' para folhas normais
      // E então escreve os 8 bits do caractere normal
      for (int i = 7; i >= 0; i--) {
        bitmapAppendLeastSignificantBit(bm, (caractere >> i) & 1);
      }
    }
  } else {
    bitmapAppendLeastSignificantBit(bm, 0); // Continua 0 para nó interno
    escreveCabecalho(getEsquerda(a), bm);
    escreveCabecalho(getDireita(a), bm);
  }
}

static void escreveArquivoCompactado(Compactador *c) {
  FILE *arq_saida = fopen(c->arqSaida, "wb"); // abre binario
  if (arq_saida == NULL) {
    perror("Erro ao abrir arquivo de saida");
    exit(EXIT_FAILURE);
  }

  // abre o arquivo pra saber o tamanho dele
  FILE *arq_para_medir = fopen(c->arqEntrada, "rb");
  if (arq_para_medir == NULL) {
    perror("Erro ao abrir arquivo de entrada para medir tamanho");
    exit(EXIT_FAILURE);
  }

  // move o cursor para o final do arquivo
  fseek(arq_para_medir, 0, SEEK_END);

  // pega a posição atual, que é o tamanho em bytes
  long tamanho_arquivo_bytes = ftell(arq_para_medir);

  fclose(arq_para_medir);

  unsigned int tamanho_max_bits = (tamanho_arquivo_bytes * 8) + (512 * 8);

  bitmap *bm = bitmapInit(tamanho_max_bits);

  // escreve a árvore que é o cabeçalho do arquivo compactado
  escreveCabecalho(c->arvore, bm);

  FILE *arq_orig = fopen(c->arqEntrada, "rb");
  if (arq_orig == NULL) {
    perror("Erro ao abrir arquivo de entrada");
    exit(EXIT_FAILURE);
  }

  int caractere;
  while ((caractere = fgetc(arq_orig)) != EOF) {
    char *codigo = c->tabelaCodigos[caractere];
    for (int i = 0; codigo[i] != '\0'; i++) {
      bitmapAppendLeastSignificantBit(bm, codigo[i] - '0');
    }
  }
  fclose(arq_orig);

  // escreve o eof no final
  char *eof_codigo = c->tabelaCodigos[256];
  for (int i = 0; eof_codigo[i] != '\0'; i++) {
    bitmapAppendLeastSignificantBit(bm, eof_codigo[i] - '0');
  }

  // Calcula quantos bytes completos precisam ser escritos
  unsigned int total_bytes = (bitmapGetLength(bm) + 7) / 8;
  fwrite(bitmapGetContents(bm), sizeof(unsigned char), total_bytes, arq_saida);

  bitmapLibera(bm);
  fclose(arq_saida);
}

Compactador *criaCompactador(const char *caminho_entrada) {
  Compactador *c = calloc(1, sizeof(Compactador));

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

// Função auxiliar para imprimir a estrutura da árvore com indentação
void imprimeArvore(Arvore *a, int nivel) {
  if (a == NULL) {
    return;
  }

  // Imprime espaços para a indentação, mostrando a profundidade
  for (int i = 0; i < nivel; i++) {
    printf("  ");
  }

  if (ehNoFolha(a)) {
    int caractere = getCaractere(a);
    // Se for um caractere imprimível, mostra. Senão, mostra o código.
    if (caractere >= 32 && caractere <= 126) {
      printf("Folha: '%c'\n", (char)caractere);
    } else if (caractere == 256) {
      printf("Folha: [EOF]\n");
    } else {
      printf("Folha: [%d]\n", caractere);
    }
  } else {
    printf("No Interno\n");
    // Chama recursivamente para os filhos
    imprimeArvore(getEsquerda(a), nivel + 1);
    imprimeArvore(getDireita(a), nivel + 1);
  }
}

void executaCompactacao(Compactador *c) {
  // conta a frequência de cada caracter
  contaFrequencia(c);

  // constrói a árvore
  constroiArvoreHuffman(c);

  printf("\n--- Árvore Original (do Compactador): ---\n");
  imprimeArvore(c->arvore, 0);

  // gera a tabela de códigos a partir da árvore
  geraTabelaCodigos(c);

  // escreve o arquivo compactado
  escreveArquivoCompactado(c);
}

void liberaCompactador(Compactador *c) {
  if (c == NULL) { // caso o ponteiro nao seja valido
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