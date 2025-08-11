#ifndef ARVORE_H_
#define ARVORE_H_

typedef struct arvore Arvore;

/**
 * @brief Cria um nó folha da árvore
 * @param caractere O caractere a ser armazenado no nó folha
 * @param frequencia A frequência de ocorrência do caractere
 * @return Ponteiro para o novo nó folha criado
 */
Arvore *criaNoFolha(int caractere, int frequencia);

/**
 * @brief Cria um nó interno da árvore
 * @param esquerda Ponteiro para a subárvore esquerda
 * @param direita Ponteiro para a subárvore direita
 * @return Ponteiro para o novo nó interno criado
 */
Arvore *criaNoInterno(Arvore *esquerda, Arvore *direita);

/**
 * @brief Obtém a frequência de um nó da árvore
 * @param a Ponteiro para o nó da árvore
 * @return A frequência armazenada no nó
 */
int getFrequencia(Arvore *a);

/**
 * @brief Compara a frequência de dois nós da árvore
 *
 * Esta função compara os valores de frequência de dois nós da árvore,
 * tipicamente usada para operações de ordenação em estruturas de dados baseadas
 * em árvore como árvores de Huffman.
 *
 * @param arv1 Ponteiro para o primeiro nó da árvore a ser comparado
 * @param arv2 Ponteiro para o segundo nó da árvore a ser comparado
 * @return int Retorna:
 *         - valor negativo se frequência de arv1 < frequência de arv2
 *         - 0 se as frequências são iguais
 *         - valor positivo se frequência de arv1 > frequência de arv2
 */
int comparaFrequencia(void *arv1, void *arv2);

/**
 * @brief Obtém o caractere armazenado em um nó folha da árvore
 * @param a Ponteiro para o nó da árvore
 * @return O caractere armazenado no nó (válido apenas para nós folha)
 */
int getCaractere(Arvore *a);

/**
 * @brief Obtém a subárvore esquerda de um nó
 * @param a Ponteiro para o nó da árvore
 * @return Ponteiro para a subárvore esquerda (NULL se for nó folha)
 */
Arvore *getEsquerda(Arvore *a);

/**
 * @brief Obtém a subárvore direita de um nó
 * @param a Ponteiro para o nó da árvore
 * @return Ponteiro para a subárvore direita (NULL se for nó folha)
 */
Arvore *getDireita(Arvore *a);

/**
 * @brief Verifica se um nó da árvore é uma folha
 *
 * Esta função determina se o nó fornecido é uma folha da árvore,
 * ou seja, se não possui filhos (nós descendentes).
 *
 * @param a Ponteiro para o nó da árvore a ser verificado
 * @return int Retorna 1 se o nó for uma folha, 0 caso contrário
 */
int ehNoFolha(Arvore *a);

/**
 * @brief Libera a memória ocupada pela árvore
 * @param a Ponteiro para a raiz da árvore a ser liberada
 * @return Ponteiro para arvore
 */
Arvore *liberaArvore(Arvore *a);

#endif