#include "compactador.h"
#include "arvore.h"
#include "bitmap.h"
#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct compactador {
  char *arqEntrada;
  char *arqSaida;
  int frequencias[256];
  Arvore *arvore;
  char *tabelaCodigos[257];
};

static void contaFrequencia(Compactador *c) {

  // le o arquivo em modo read binary
  FILE *arq = fopen(c->arqEntrada, "rb");

  // calcula a frequencia de todos os caracteres
  int caractere;
  while ((caractere = fgetc(arq)) != EOF) {
    c->frequencias[caractere]++;
  }

  fclose(arq);
}

static void constroiArvoreHuffman(Compactador *c) {
  // Implementação da Etapa 2 e 3: Construir lista e montar a árvore
  // ... usa c->frequencias, cria a lista, monta a árvore e salva em
  // c->arvore_huffman ...

  Lista *listaPrioridade = criaLista();

  // Cria nós folhas pra cada caractere e insere em uma lista
  for (int i = 0; i < 256; i++) {
    if (c->frequencias[i] > 0) {
      Arvore *no = criaNoFolha(i, c->frequencias[i]);
      insereItemOrdenado(listaPrioridade, no, comparaFrequencia);
    }
  }

  // criação do nó "end of file" para o descompactador saber quando parar
  Arvore *eof = criaNoFolha(256, 1);
  insereItemOrdenado(listaPrioridade, eof, comparaFrequencia);

//ALTERAÇÃO QUE EU FIZ: 
//qtdLista--; ... = do jeito que tava, em algum moemnto o qtdLista ia ser 1 ou até menos
//fora isso, sua logica continua a mesma, só adaptei essa questão

  // cria nós internos até sobrar o nó raiz
  while (getQuantidadeItemsLista(listaPrioridade) > 1) {
    Arvore *noEsquerdo = removePrimeiroItem(listaPrioridade);
    Arvore *noDireito = removePrimeiroItem(listaPrioridade);

    // cria nó interno e reinsere na lista
    Arvore *interno = criaNoInterno(noEsquerdo, noDireito);
    insereItemOrdenado(listaPrioridade, interno, comparaFrequencia);
  }

  // armazena o nó raiz no compactador
  c->arvore = removePrimeiroItem(listaPrioridade);

  liberaLista(listaPrioridade, NULL);
}

void encontraCaminho(Arvore *noAtual, char caminhoTemporario[],
                     int profundidade, char *tabelaFinal[]) {
  // se for nó folha
  if (ehNoFolha(noAtual)) {
    int caractere = getCaractere(noAtual);
    // adiciona um terminador de string na posição atual
    caminhoTemporario[profundidade] = '\0';
    // copia o caminho para o array de endereços
    tabelaFinal[caractere] = strdup(caminhoTemporario);
    return;
  } else {
    caminhoTemporario[profundidade] = '0';
    encontraCaminho(getEsquerda(noAtual), caminhoTemporario, profundidade + 1,
                    tabelaFinal);
    caminhoTemporario[profundidade] = '1';
    encontraCaminho(getDireita(noAtual), caminhoTemporario, profundidade + 1,
                    tabelaFinal);
  }
};

static void geraTabelaCodigos(Compactador *c) {
  //buffer temporario pra armazenar o codigo binario
  char caminhoTemporario[257];
    for (int i = 0; i < 257; i++){
      c->tabelaCodigos[i] = NULL;
    }
    
  //percorre a árvore e encontra os caminhos para preencher a tabela de códigos
  encontraCaminho(c->arvore, caminhoTemporario, 0, c->tabelaCodigos);
}


static void escreveCabecalho(Arvore *a, FILE *arq) {
  if (ehNoFolha(a)){
    //escreve 1 pra indicar que é um nó folha
    fputc('1', arq);
    //escreve o caracter
    fputc(getCaractere(a), arq);

    //obs: é usado o fputc ao invés do fprintf pq o fputc escreve um byte
  } 
  else{
    //escreve 0 para indicar que é um nó interno
    fputc('0', arq);
    //chama recursivamente pros filhos
    escreveCabecalho(getEsquerda(a), arq);
    escreveCabecalho(getDireita(a), arq);
  }
}

static void escreveBitsNoArquivo(FILE *arq, char *codigos[257], FILE *arq_orig) {
  bitmap *bm = bitmapInit(256); //buffer temporario

  int caractere;
  while ((caractere = fgetc(arq_orig)) != EOF){  //le o arquivo original carcater por caracter até o final
    char *codigo = codigos[caractere];
    for (int i = 0; i < strlen(codigo); i++){
      //cada caracter 0 ou 1 da string é convertido em seu valor numerico e adicionado ao bitmap
      bitmapAppendLeastSignificantBit(bm, codigo[i] - '0');

      if (bitmapGetLength(bm) == 8){
        //quando froma um byte completo, escreve no arquivo
        fputc(bitmapGetContents(bm)[0], arq);
        bitmapLibera(bm);
        bm = bitmapInit(256);
      }
    }
  }
    
  // Adiciona o pseudo-caractere de final de arquivo (256)
  char *eof_codigo = codigos[256];
  for (int i = 0; i < strlen(eof_codigo); i++) {
    bitmapAppendLeastSignificantBit(bm, eof_codigo[i] - '0');
    if (bitmapGetLength(bm) == 8) {
      fputc(bitmapGetContents(bm)[0], arq);
      bitmapLibera(bm);
      bm = bitmapInit(256);
    }
  }
    
  //se depois disso tudo, ainda tiver bits no bitmap (menos de 8), eles formam um byte incompleto e escreve eles no arquivo (são os bits extras)
  if (bitmapGetLength(bm) > 0){
    putc(bitmapGetContents(bm)[0], arq);
  }
    
  bitmapLibera(bm);
}


static void escreveArquivoCompactado(Compactador *c) {
  FILE *arq_saida = fopen(c->arqSaida, "wb");  //abre binario
  if (arq_saida == NULL) {
    perror("Erro ao abrir arquivo de saida");
    exit(EXIT_FAILURE);
  }

  //escreve a árvore que é o cabeçalho do arquivo compactado
  escreveCabecalho(c->arvore, arq_saida);

  //escreve os dados codificados
  FILE *arq_orig = fopen(c->arqEntrada, "rb");  //le binario
  if (arq_orig == NULL){
    perror("Erro ao abrir arquivo de entrada para escrita");
    exit(EXIT_FAILURE);
  }
  escreveBitsNoArquivo(arq_saida, c->tabelaCodigos, arq_orig);
  fclose(arq_orig);
    
  fclose(arq_saida);
}

Compactador *criaCompactador(const char *caminho_entrada) {
  Compactador *c = calloc(1, sizeof(Compactador));

  c->arqEntrada = strdup(caminho_entrada);

  // adiciona o .bica para o arquivo compactado
  c->arqSaida = (char *)malloc(strlen(caminho_entrada) + 5); // .bica + \0
  strcpy(c->arqSaida, caminho_entrada);
  strcat(c->arqSaida, ".bica");

  for (int i = 0; i < 256; i++) {
    c->tabelaCodigos[i] = NULL;
  }

  return c;
};

void executaCompactacao(Compactador *c) {
  //conta a frequência de cada caracter
  contaFrequencia(c);

  //constrói a árvore
  constroiArvoreHuffman(c);

  //gera a tabela de códigos a partir da árvore
  geraTabelaCodigos(c);

  //escreve o arquivo compactado
  escreveArquivoCompactado(c);
}

void liberaCompactador(Compactador *c){
  if (c == NULL){  //caso o ponteiro nao seja valido
    return;
  }

  free(c->arqEntrada);
  free(c->arqSaida);
  liberaArvore(c->arvore);

  //pra cada ponteiro nao nulo, libera a string alocada
  for (int i = 0; i < 257; i++) {
    if (c->tabelaCodigos[i] != NULL) {
      free(c->tabelaCodigos[i]);
    }
  }

  //libera o compactador
  free(c);
}