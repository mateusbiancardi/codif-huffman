#include <stdio.h>
#include <stdlib.h>
#include "codificador.h"
#include "bitmap.h"


struct noArvore {
    unsigned char caracter;   //caracter que o nó folha representa
    int frequencia;   //frequencia de um caracter
    NoArvore *esquerda;
    NoArvore *direita;
};

struct codificador {
    NoArvore *raiz;
};

Codificador* criaCodificador() {
    Codificador* codific = (Codificador*)malloc(sizeof(Codificador));
    if (codific == NULL) {
        //erro de alocação de memória
        exit(1);
    }
    codific->raiz = NULL;
    return codific;
}

static NoArvore* criNoFolha(unsigned char caracter, int frequencia) {
    NoArvore* no = (NoArvore*)malloc(sizeof(NoArvore));
    if (no == NULL) {
        exit(1);
    }
    //inicializa o nó
    no->caracter = caracter;
    no->frequencia = frequencia;
    //sao inicializados com NULL pq inicialmente nao tem filhos
    no->esquerda = NULL;
    no->direita = NULL;
    return no;
}

//reduz o numero total de arvores na lista
static NoArvore* criaNoInterno(NoArvore* filhoEsq, NoArvore* filhoDir) {
    NoArvore* noInt = (NoArvore*)malloc(sizeof(NoArvore));
    if (noInt == NULL) {
        exit(1);
    }
    noInt->caracter = 0; //nós internos nao tem um caracter especifico
    //a frequencia interna é a soma das frequencias dos filhos
    noInt->frequencia = filhoEsq->frequencia + filhoDir->frequencia;
    //estrutura hierarquica da arvore
    noInt->esquerda = filhoEsq;
    noInt->direita = filhoDir;
    return noInt;
}

void constroiCodificador(Codificador* codific, int* frequencia) {
    //A implementação do algoritmo de Huffman
}

static void traverse_codificador_tree(struct noArvore* no, bitmap* codigoAtual, bitmap* codigos[256]) {
    if (no == NULL) {   //pra nao acessar um no nulo
        return;
    }

    if (no->esquerda == NULL && no->direita == NULL) {   //no folha
        //armazena o codigo binario no caracter correspondente
        //o tamanho é o compriento do codigoAtual
        codigos[no->caracter] = bitmapInit(bitmapGetLength(codigoAtual));
        //copia os bits do codigoAtual pro novo bitmap na tabela de codigos (codigos)
        for (unsigned int i = 0; i < bitmapGetLength(codigoAtual); i++) {
            bitmapAppendLeastSignificantBit(codigos[no->caracter], bitmapGetBit(codigoAtual, i));
        }
    } 
    else {   //no folha
        //percorre subarvore esquerda com 0
        bitmapAppendLeastSignificantBit(codigoAtual, 0); //adiciona um 0 a sequencia de bits
        traverse_codificador_tree(no->esquerda, codigoAtual, codigos); //percorre a subarvore
        bitmapRemoveLastBit(codigoAtual);  //remove o bit 0 adicionado

        //percorre subarvore direita com 1
        //(mesmos passos só que com 1)
        bitmapAppendLeastSignificantBit(codigoAtual, 1);
        traverse_codificador_tree(no->direita, codigoAtual, codigos);
        bitmapRemoveLastBit(codigoAtual);
    }
}


void geraCodificacao(Codificador* codific, bitmap* codigos[256]) {
    //inicializa os ponteiros do array com NULL
    for (int i = 0; i < 256; i++) {
        codigos[i] = NULL;
    }

    //buffer temporario para armazenar o codigo atual durante a travessia
    bitmap* codigoAtual = bitmapInit(256); // Tamanho suficiente para um código
    traverse_codificador_tree(codific->raiz, codigoAtual, codigos);  //percorre a arovore e gera os codigos
    //libera o bitmap temporario
    bitmapLibera(codigoAtual);
}

//liberar recursivamente a memoria alocada pros nós de uma subarvore
void liberaNosArvore(NoArvore* no) {
    if (no == NULL) {  //nao tem mais nos pra liberar
        return;
    }
    liberaNosArvore(no->esquerda);
    liberaNosArvore(no->direita);
    free(no);
}

void liberaCodificador(Codificador* codific){
    if (codific == NULL) {  //pra nao acessar um ponteiro nulo
        return;
    }
    liberaNosArvore(codific->raiz);
    free(codific);
}