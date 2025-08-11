#include "arvore.h"
#include <stdlib.h>

struct arvore {
  int frequencia;
  int caractere; // Consegue armazenar mais que 256 caracteres
  Arvore *esquerda;
  Arvore *direita;
};

int arvoreVazia(Arvore *a) { return a == NULL; }

Arvore *criaNoFolha(int caractere, int frequencia) {
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

int comparaFrequencia(void *arv1, void *arv2) {
  Arvore *arvore1 = arv1;
  Arvore *arvore2 = arv2;

  return getFrequencia(arvore1) - getFrequencia(arvore2);
};

int getCaractere(Arvore *a) { return a->caractere; };

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