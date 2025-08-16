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

static int leProximoBit(FILE *arq) {
  static int byte_atual = 0;
  static int contador_bits = 0;

  // Se já lemos todos os 8 bits do byte atual, leia um novo byte do arquivo
  if (contador_bits == 0) {
    byte_atual = fgetc(arq);
    if (byte_atual == EOF) {
      return EOF;
    }
    contador_bits = 8;
  }

  // Extrai o bit mais significativo do byte atual
  int bit = (byte_atual >> 7) & 1;

  // Prepara o byte para a próxima leitura, deslocando os bits para a esquerda
  byte_atual = byte_atual << 1;

  contador_bits--;

  return bit;
}

static Arvore *leCabecalho(FILE *arq) {
  int tipo_bit = leProximoBit(arq);

  if (tipo_bit == EOF) {
    return NULL;
  }

  if (tipo_bit == 1) {
    int bit_tipo_folha = leProximoBit(arq); // lê o bit extra
    if (bit_tipo_folha == 1) {
      return criaNoFolha(256, 0);
    } else { // É uma folha normal
      int caractere = 0;
      // Lê os próximos 8 bits para reconstruir o caractere
      for (int i = 0; i < 8; i++) {
        int bit = leProximoBit(arq);
        if (bit == EOF)
          return NULL;
        caractere = (caractere << 1) | bit;
      }
      return criaNoFolha(caractere, 0);
    }
  } else { // Nó interno (lógica inalterada)
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

  // lê o arquivo um bit de cada vez
  while ((bit = leProximoBit(arq_entrada)) != EOF) {

    // Navega na árvore com base no bit lido
    if (bit == 0) {
      noAtual = getEsquerda(noAtual);
    } else {
      noAtual = getDireita(noAtual);
    }

    if (ehNoFolha(noAtual)) {
      if (getCaractere(noAtual) == 256) { // EOF
        break;
      }
      fputc(getCaractere(noAtual), arq_saida); // Escreve o caractere
      noAtual = d->arvore;
    }
  }
}

// Função auxiliar para imprimir a estrutura da árvore com indentação
void imprimeArvore2(Arvore *a, int nivel) {
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
    imprimeArvore2(getEsquerda(a), nivel + 1);
    imprimeArvore2(getDireita(a), nivel + 1);
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

  printf("\n--- Árvore Reconstruída (do Descompactador): ---\n");
  imprimeArvore2(d->arvore, 0);

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