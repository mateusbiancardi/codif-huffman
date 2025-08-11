/*
 *
 * Lista Duplamente Encadeada para itens genéricos
 * Autores: Mateus Biancardi e Rafaela Capovilla
 *
 */

#ifndef LISTA_H
#define LISTA_H

typedef struct celula Celula;
typedef struct lista Lista;

/**
 * @brief Cria e inicializa uma nova lista duplamente encadeada vazia.
 *
 * @return Um ponteiro para a estrutura Lista recém-alocada.
 */
Lista *criaLista();

/**
 * @brief Insere um novo item no final da lista.
 *
 * @param l Ponteiro para a lista na qual o item será inserido.
 * @param item Ponteiro genérico (void*) para o dado a ser armazenado. A lista
 * armazena apenas o ponteiro, não uma cópia do dado.
 */
void insereItemLista(Lista *l, void *item);

/**
 * @brief Insere um item de forma ordenada na lista.
 * A função de comparação determina a ordem.
 * * @param l Ponteiro para a lista.
 * @param item Ponteiro para o item a ser inserido.
 * @param compara Ponteiro para uma função de comparação que recebe dois itens.
 * Deve retornar < 0 se item1 < item2, 0 se item1 == item2, > 0 se item1 >
 * item2.
 */
void insereItemOrdenado(Lista *l, void *item,
                        int (*compara)(void *item1, void *item2));

/**
 * @brief Busca um item na lista com base em uma chave e uma função de
 * comparação.
 * * A função percorre a lista e utiliza a função de comparação para encontrar
 * uma correspondência. A lista não é modificada durante a operação.
 *
 * @param l Ponteiro para a lista onde a busca será realizada.
 * @param id Ponteiro para uma chave de busca a ser passada para a função de
 * comparação.
 * @param c Ponteiro para a função de comparação. A função deve retornar 1
 * (verdadeiro) em caso de correspondência e 0 (falso) caso contrário.
 * @return Um ponteiro (void*) para o item encontrado, ou NULL se o item não
 * estiver na lista.
 */
void *getItemLista(Lista *l, void *id, int (*c)(void *id, void *item));

/**
 * @brief Verifica se um item com o identificador especificado existe na lista.
 *
 * Esta função percorre a lista e utiliza a função de comparação fornecida
 * para determinar se um item com o identificador dado existe.
 *
 * @param l Ponteiro para a lista a ser pesquisada.
 * @param id Ponteiro para o identificador a ser buscado.
 * @param c Função de comparação que recebe o identificador e um item da lista
 * como argumentos. Deve retornar 1 se o item corresponder ao identificador, 0
 * caso contrário.
 * @return int Retorna 1 se o item existe na lista, 0 caso contrário.
 */
int verificaItemListaExiste(Lista *l, void *id, int (*c)(void *id, void *item));

/**
 * @brief Remove um item da lista com base em uma chave de busca.
 *
 * A busca é realizada por uma função de comparação fornecida pelo usuário.
 * A célula da lista é liberada, mas o item contido nela não é.
 *
 * @param l Ponteiro para a lista da qual o item será removido.
 * @param id Ponteiro para uma chave de busca a ser passada para a função de
 * comparação.
 * @param c Ponteiro para a função de comparação. A função deve receber a
 * chave (id) e um item da lista, retornando 1 em caso de correspondência.
 * @return O ponteiro (void*) para o item que foi removido, ou NULL se não
 * for encontrado.
 */
void *removeItemLista(Lista *l, void *id, int (*c)(void *id, void *item));

/**
 * @brief Remove e retorna o primeiro item da lista.
 *
 * Esta função remove o primeiro elemento da lista encadeada e retorna
 * um ponteiro para os dados que estavam armazenados neste elemento.
 * A memória da célula é liberada, mas a memória dos dados deve ser
 * gerenciada pelo chamador da função.
 *
 * @param l Ponteiro para a estrutura da lista
 * @return void* Ponteiro para os dados do primeiro item removido,
 *               ou NULL se a lista estiver vazia ou for inválida
 *
 */
void *removePrimeiroItem(Lista *l);

/**
 * @brief Libera toda a memória associada à lista, incluindo suas células.
 *
 * Opcionalmente, pode liberar a memória de cada item armazenado se uma função
 * para isso for fornecida.
 *
 * @param l Ponteiro para a lista a ser liberada.
 * @param liberaItem Ponteiro para uma função que libera a memória de um item.
 * Se for NULL, a memória dos itens não será liberada por esta função.
 */
void liberaLista(Lista *l, void (*liberaItem)(void *item));

/**
 * @brief Retorna um ponteiro para a primeira célula da lista.
 *
 * @param l Ponteiro para a lista.
 * @return Um ponteiro para a primeira célula, ou NULL se a lista estiver vazia.
 */
Celula *getPrimeiraCelula(Lista *l);

/**
 * @brief Retorna um ponteiro para a última célula da lista.
 *
 * @param l Ponteiro para a lista.
 * @return Um ponteiro para a última célula, ou NULL se a lista estiver vazia.
 */
Celula *getUltimaCelula(Lista *l);

/**
 * @brief Retorna um ponteiro para a célula seguinte a uma célula fornecida.
 *
 * @param c Ponteiro para a célula atual.
 * @return Um ponteiro para a próxima célula, ou NULL se não houver próxima.
 */
Celula *getProximaCelula(Celula *c);

/**
 * @brief Retorna um ponteiro para a célula anterior a uma célula fornecida.
 *
 * @param c Ponteiro para a célula atual.
 * @return Um ponteiro para a célula anterior, ou NULL se não houver anterior.
 */
Celula *getAnteriorCelula(Celula *c);

/**
 * @brief Retorna o ponteiro para o item genérico armazenado em uma célula.
 *
 * @param c Ponteiro para a célula da qual se deseja obter o item.
 * @return O ponteiro (void*) para o item armazenado.
 */
void *getItemFromCelula(Celula *c);

/**
 * @brief Retorna a quantidade total de itens presentes na lista.
 *
 * Esta função percorre a lista e conta o número de elementos armazenados,
 * retornando o valor total como um inteiro.
 *
 * @param l Ponteiro para a estrutura Lista da qual se deseja obter a quantidade
 * de itens
 * @return int Número total de itens na lista, ou 0 se a lista estiver vazia ou
 * for NULL
 */
int getQuantidadeItemsLista(Lista *l);

#endif // LISTA_H