#include "compactador.h"
#include "descompactador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Função para verificar se um arquivo existe
int arquivo_existe(const char *nome_arquivo) {
  FILE *arq = fopen(nome_arquivo, "rb");
  if (arq) {
    fclose(arq);
    return 1; // Existe
  }
  return 0; // Não existe
}

// Função para exibir a forma de uso do programa
void exibir_ajuda(const char *nome_programa) {
  printf("Erro: Argumentos invalidos.\n");
  printf("Uso: %s <opcao> <nome_do_arquivo>\n", nome_programa);
  printf("\nOpcoes:\n");
  printf("  -c\tCompacta o arquivo especificado.\n");
  printf("  -d\tDescompacta o arquivo especificado.\n");
}

int main(int argc, char *argv[]) {

  // 1. O programa agora espera 3 argumentos: ./programa <opcao> <arquivo>
  if (argc != 3) {
    exibir_ajuda(argv[0]);
    return EXIT_FAILURE;
  }

  const char *opcao = argv[1];
  const char *nome_arquivo = argv[2];

  // 2. Verifica se o arquivo de entrada fornecido existe
  if (!arquivo_existe(nome_arquivo)) {
    printf("Erro: O arquivo de entrada '%s' nao foi encontrado.\n",
           nome_arquivo);
    return EXIT_FAILURE;
  }

  // 3. Decide a ação com base na opção (-c ou -d)
  if (strcmp(opcao, "-c") == 0) {
    // --- MODO COMPACTAÇÃO ---
    printf("--- Iniciando Compactacao de '%s' ---\n", nome_arquivo);

    Compactador *compactador = criaCompactador(nome_arquivo);
    executaCompactacao(compactador);
    liberaCompactador(compactador);

    printf("--- Compactacao concluida! Arquivo '%s.comp' gerado. ---\n",
           nome_arquivo);

  } else if (strcmp(opcao, "-d") == 0) {
    // --- MODO DESCOMPACTAÇÃO ---
    printf("--- Iniciando Descompactacao de '%s' ---\n", nome_arquivo);

    // Usa o TAD Descompactador, que já sabe criar o nome de saída
    // (ex: "arquivo_descompactado.txt") sem sobrescrever o original.
    Descompactador *descompactador = criaDescompactador(nome_arquivo);
    executaDescompactacao(descompactador);
    liberaDescompactador(descompactador);

    printf("--- Descompactacao concluida! ---\n");
    printf("Verifique o arquivo gerado com o sufixo '_descompactado'.\n");

  } else {
    // Se a opção não for nem -c nem -d
    printf("Opcao invalida: '%s'\n", opcao);
    exibir_ajuda(argv[0]);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}