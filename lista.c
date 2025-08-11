/*
 *
 * Lista Duplamente Encadeada para itens genéricos
 * Autores: Mateus Biancardi e Rafaela Capovilla
 *
 */

#include "lista.h"
#include <stdlib.h>
#include <string.h>

struct celula {
  void *item;
  Celula *prox;
  Celula *ant;
};

struct lista {
  Celula *primeiro;
  Celula *ultimo;
};

Lista *criaLista() {
  Lista *l = calloc(1, sizeof(Lista));

  l->primeiro = NULL;
  l->ultimo = NULL;

  return l;
};

void insereItemLista(Lista *l, void *item) {
  Celula *c = calloc(1, sizeof(Celula));

  c->item = item;
  c->prox = NULL;
  c->ant = l->ultimo;

  if (l->primeiro == NULL) {
    l->primeiro = c;
  } else {
    l->ultimo->prox = c;
  }

  l->ultimo = c;
}

void insereItemOrdenado(Lista *l, void *item,
                        int (*compara)(void *item1, void *item2)) {
  Celula *c = calloc(1, sizeof(Celula));

  c->item = item;

  if (l->primeiro == NULL) {
    l->primeiro = c;
    l->ultimo = c;
    return;
  }

  Celula *atual = l->primeiro;

  while (atual != NULL && compara(c->item, atual->item) > 0) {
    atual = atual->prox;
  }

  // Inserção no início
  if (atual == l->primeiro) {
    c->prox = atual;
    atual->ant = c;
    l->primeiro = c;
  }

  // Insercão no final (atual é NULL)
  else if (atual == NULL) {
    c->ant = l->ultimo;
    l->ultimo->prox = c;
    l->ultimo = c;
  }

  // Inserção no meio
  else {
    c->prox = atual;
    c->ant = atual->ant;
    atual->ant->prox = c;
    atual->ant = c;
  }
};

void *getItemLista(Lista *l, void *id, int (*c)(void *id, void *item)) {
  Celula *p = l->primeiro;

  // Verifica se o id bate com o id de dentro do item (do livro, por exemplo)
  // Se c encontrar o item, vai retornar 1; se não encontrar, retorna 0;
  // por isso a negação de c
  while (p != NULL && !c(id, p->item)) {
    p = p->prox;
  }

  if (p == NULL) {
    return NULL;
  }

  return p->item;
}

int verificaItemListaExiste(Lista *l, void *id,
                            int (*c)(void *id, void *item)) {
  Celula *p = l->primeiro;

  // Verifica se o id bate com o id de dentro do item (do livro, por exemplo)
  // Se c encontrar o item, vai retornar 1; se não encontrar, retorna 0;
  // por isso a negação de c
  while (p != NULL && !c(id, p->item)) {
    p = p->prox;
  }

  if (p == NULL) {
    return 0;
  }

  return 1;
};

void *removeItemLista(Lista *l, void *id, int (*c)(void *id, void *item)) {
  Celula *p = l->primeiro;

  // Verifica se o id bate com o id de dentro do item (do livro, por exemplo)
  // Se c encontrar o item, vai retornar 1; se não encontrar, retorna 0;
  // por isso a negação de c
  while (p != NULL && !c(id, p->item)) {
    p = p->prox;
  }

  if (p == NULL) {
    return NULL;
  }

  if (p == l->primeiro) {
    l->primeiro = p->prox;
  } else {
    p->ant->prox = p->prox;
  }

  if (p == l->ultimo) {
    l->ultimo = p->ant;
  } else {
    p->prox->ant = p->ant;
  }

  void *itemRemovido = p->item;
  free(p);

  return itemRemovido;
}

void *removePrimeiroItem(Lista *l) {
  Celula *p = l->primeiro;

  if (p == NULL) {
    return NULL;
  }

  if (l->primeiro == l->ultimo) {
    l->primeiro = NULL;
    l->ultimo = NULL;
  } else {
    l->primeiro = p->prox;
    l->primeiro->ant = NULL;
  }

  void *itemRemovido = p->item;
  free(p);

  return itemRemovido;
};

void liberaLista(Lista *l, void (*liberaItem)(void *item)) {
  Celula *p = l->primeiro;
  Celula *temp;

  while (p != NULL) {
    temp = p->prox;

    if (liberaItem != NULL) {
      liberaItem(p->item);
    }

    free(p);
    p = temp;
  }

  free(l);
}

Celula *getPrimeiraCelula(Lista *l) { return l->primeiro; }
Celula *getUltimaCelula(Lista *l) { return l->ultimo; }

Celula *getProximaCelula(Celula *c) { return c->prox; }
Celula *getAnteriorCelula(Celula *c) { return c->ant; }

void *getItemFromCelula(Celula *c) { return c->item; }

int getQuantidadeItemsLista(Lista *l) {
  int qntd = 0;

  Celula *c = getPrimeiraCelula(l);

  while (c != NULL) {
    qntd++;
    c = c->prox;
  }

  return qntd;
};