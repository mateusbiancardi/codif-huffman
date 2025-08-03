#include "arvore.h"
#include <stdlib.h>

struct arvore {
  int frequencia;
  unsigned char caractere;
  Arvore *esquerda;
  Arvore *direita;
};

int arvoreVazia(Arvore *a) { return a == NULL; }

Arvore *criaNoFolha(unsigned char caractere, int frequencia) {
  Arvore *a = (Arvore *)calloc(1, sizeof(Arvore));

  a->frequencia = frequencia;
  a->caractere = caractere;

  return a;
};

Arvore *criaNoInterno(Arvore *esquerda, Arvore *direita) {
  Arvore *interno = (Arvore *)calloc(1, sizeof(Arvore));

  interno->frequencia = esquerda->frequencia + direita->frequencia;
  interno->esquerda = esquerda;
  interno->direita = direita;

  return interno;
};

int getFrequencia(Arvore *a) { return a->frequencia; };

unsigned char getCaractere(Arvore *a) { return a->caractere; };

Arvore *getEsquerda(Arvore *a) { return a->esquerda; };

Arvore *getDireita(Arvore *a) { return a->direita; };

int ehNoFolha(Arvore *a) {
  if (arvoreVazia(a)) {
    return 0;
  }
  return a->esquerda == NULL && a->direita == NULL;
};

Arvore *liberaArvore(Arvore *a) {
  if (!arvoreVazia(a)) {
    liberaArvore(a->esquerda);
    liberaArvore(a->direita);
    free(a);
  }
  return NULL;
};