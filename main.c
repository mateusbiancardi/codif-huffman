#include "compactador.h"
#include "descompactador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Uso: %s <arquivo_a_processar>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *arquivo_original = argv[1];

  FILE *arq_teste = fopen(arquivo_original, "rb");
  if (arq_teste == NULL) {
    perror("Erro: Nao foi possivel encontrar o arquivo de entrada");
    return EXIT_FAILURE;
  }
  fclose(arq_teste);

  printf("--- Processando arquivo: '%s' ---\n\n", arquivo_original);

  // --- ETAPA DE COMPACTAÇÃO ---
  printf("1. Compactando...\n");
  Compactador *compactador = criaCompactador(arquivo_original);
  executaCompactacao(compactador);
  liberaCompactador(compactador);

  char *arquivo_compactado = (char *)malloc(strlen(arquivo_original) + 6);
  sprintf(arquivo_compactado, "%s.comp", arquivo_original);
  printf("   Arquivo compactado gerado: '%s'\n", arquivo_compactado);

  // --- ETAPA DE DESCOMPACTAÇÃO ---
  printf("\n2. Descompactando para um novo arquivo...\n");

  // --- LÓGICA PARA NÃO SOBRESCREVER ---

  // Nomes de arquivos que vamos usar
  char arquivo_protegido[FILENAME_MAX];
  char arquivo_final_descompactado[FILENAME_MAX];

  // a. Crie o nome do arquivo protegido (ex: "teste.txt.protegido")
  sprintf(arquivo_protegido, "%s.protegido", arquivo_original);

  // b. Crie o nome final desejado (ex: "teste_descompactado.txt")
  char *ponto_extensao = strrchr(arquivo_original, '.');
  if (ponto_extensao != NULL) {
    int base_len = ponto_extensao - arquivo_original;
    strncpy(arquivo_final_descompactado, arquivo_original, base_len);
    arquivo_final_descompactado[base_len] = '\0';
    strcat(arquivo_final_descompactado, "_descompactado");
    strcat(arquivo_final_descompactado, ponto_extensao);
  } else {
    sprintf(arquivo_final_descompactado, "%s_descompactado", arquivo_original);
  }

  // c. Renomeie o original para protegê-lo
  if (rename(arquivo_original, arquivo_protegido) != 0) {
    perror("   Erro ao tentar proteger o arquivo original");
    free(arquivo_compactado);
    return EXIT_FAILURE;
  }

  // d. Execute a descompactação. O TAD vai criar um novo arquivo com o nome
  // original.
  Descompactador *descompactador = criaDescompactador(arquivo_compactado);
  executaDescompactacao(descompactador);
  liberaDescompactador(descompactador);

  // e. Renomeie o arquivo recém-criado para o nome final desejado
  if (rename(arquivo_original, arquivo_final_descompactado) != 0) {
    perror("   Erro ao renomear o arquivo descompactado");
  }

  // f. Restaure o nome original do arquivo que foi protegido
  if (rename(arquivo_protegido, arquivo_original) != 0) {
    perror("   Erro ao restaurar o nome do arquivo original");
  }

  printf("   Arquivo descompactado salvo como: '%s'\n",
         arquivo_final_descompactado);

  printf("\n--- Processo concluído! ---\n");
  printf("Pode comparar o arquivo original '%s' com o descompactado '%s'.\n",
         arquivo_original, arquivo_final_descompactado);

  free(arquivo_compactado);
  return EXIT_SUCCESS;
}