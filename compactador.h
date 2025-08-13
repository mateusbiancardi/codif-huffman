/*
 *
 * Tad Compactador
 * Responsável por orquestrar todo o processo de compactação de arquivos
 * Autores: Mateus Biancardi e Rafaela Capovilla
 *
 */

#ifndef COMPACTADOR_H
#define COMPACTADOR_H

typedef struct compactador Compactador;

/**
 * @brief Cria e inicializa uma instância do compactador.
 * * Aloca a memória para a estrutura do compactador e armazena os caminhos
 * dos arquivos de entrada e saída.
 * @param caminho_entrada O nome do arquivo a ser compactado.
 * @return Um ponteiro para a nova instância do Compactador.
 */
Compactador *criaCompactador(const char *caminho_entrada);

/**
 * @brief Executa todo o processo de compactação.
 * * Esta função orquestra todas as etapas: contagem de frequência,
 * construção da árvore, geração da tabela de códigos e escrita do arquivo
 * final.
 * @param c Ponteiro para o Compactador.
 */
void executaCompactacao(Compactador *c);

/**
 * @brief Libera toda a memória associada ao compactador.
 * * Libera a árvore de Huffman, a tabela de códigos e a própria estrutura do
 * compactador.
 * @param c Ponteiro para o Compactador a ser liberado.
 */
void liberaCompactador(Compactador *c);

#endif // COMPACTADOR_H