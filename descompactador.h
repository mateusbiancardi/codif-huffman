/*
 *
 * Tad Descompactador
 * Responsável por orquestrar todo o processo de descompactação de arquivos
 * Autores: Mateus Biancardi e Rafaela Capovilla
 *
 */

#ifndef DESCOMPACTADOR_H
#define DESCOMPACTADOR_H

#include "arvore.h"
#include "bitmap.h"
#include "lista.h"
#include <stdio.h>

/**
 * @brief Estrutura para representar o descompactador.
 *
 * Esta é uma estrutura opaca para o descompactador.
 * Ela contém os nomes dos arquivos e a árvore de Huffman reconstruída.
 */
typedef struct descompactador Descompactador;

/**
 * @brief Cria e inicializa uma nova instância do descompactador.
 *
 * Aloca a memória para a estrutura do descompactador e armazena os caminhos
 * dos arquivos de entrada e saída.
 *
 * @param caminho_entrada O nome do arquivo a ser descompactado.
 * @return Um ponteiro para a nova instância do Descompactador.
 */
Descompactador* criaDescompactador(const char* caminho_entrada);

/**
 * @brief Executa todo o processo de descompactação.
 *
 * Esta função orquestra todas as etapas: leitura do cabeçalho,
 * reconstrução da árvore, leitura dos bits e escrita do arquivo original.
 *
 * @param d Ponteiro para a estrutura do Descompactador.
 */
void executaDescompactacao(Descompactador* d);

/**
 * @brief Libera toda a memória associada ao descompactador.
 *
 * Libera a árvore de Huffman e a própria estrutura do descompactador.
 *
 * @param d Ponteiro para a estrutura do Descompactador a ser liberado.
 */
void liberaDescompactador(Descompactador* d);

#endif // DESCOMPACTADOR_H