#ifndef CODIFICADOR_H_
#define CODIFICADOR_H_

#include "bitmap.h"

// TAD para um nó da árvore de huffman
typedef struct NoArvore noArvore;

// TAD para o codificador, que encapsula a árvore
typedef struct Codificador codificador;

/**
 * Cria e inicializa um novo codificador.
 * @return Um ponteiro para a estrutura do codificador.
 */
Codificador* criaCodificador();

/**
 * Constrói a árvore de Huffman do codificador a partir de um array de frequências.
 * @param codific O ponteiro para o codificador.
 * @param frequencia Array de inteiros com a frequência de cada caractere ASCII (0-255).
 */
void constroiCodificador(codificador* codific, int* frequencia);

/**
 * Gera a tabela de codificação a partir da árvore de Huffman do codificador.
 * @param codific O ponteiro para o codificador.
 * @param codigos Array de bitmaps para armazenar os códigos gerados.
 */
void geraCodificacao(codificador* codific, bitmap* codigos[256]);

/**
 * Libera a memória alocada para o codificador e sua árvore de Huffman.
 * @param codific O ponteiro para o codificador.
 */
void liberaCodificador(codificador* codific);

#endif /* CODIFICADOR_H_ */