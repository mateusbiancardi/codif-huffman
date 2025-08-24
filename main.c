#include "compactador.h"
#include "descompactador.h"
#include <stdio.h>
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

int main(int argc, char *argv[]) {

  // espera 3 argumentos -> ./programa <opcao> <arquivo>
  if (argc != 3) {
    return 1;
  }

  const char *opcao = argv[1];
  const char *nome_arquivo = argv[2];

  // verifica se o arquivo de entrada fornecido existe
  if (!arquivo_existe(nome_arquivo)) {
    return 1;
  }

  // decide a ação com base na opção (-c ou -d)
  if (strcmp(opcao, "-c") == 0) {
    Compactador *compactador = criaCompactador(nome_arquivo);
    executaCompactacao(compactador);
    liberaCompactador(compactador);

  } else if (strcmp(opcao, "-d") == 0) {
    // verifica se o arquivo tem a extensão .comp
    int len = strlen(nome_arquivo);
    if (len < 5 || strcmp(nome_arquivo + len - 5, ".comp") != 0) {
      return 1;
    }

    Descompactador *descompactador = criaDescompactador(nome_arquivo);
    executaDescompactacao(descompactador);
    liberaDescompactador(descompactador);

  } else {
    return 1;
  }

  return 0;
}