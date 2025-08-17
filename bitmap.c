/** Define um TAD representando um mapa de bits.
 * @file bitmap.c
 * @author Joao Paulo Andrade Almeida (jpalmeida@inf.ufes.br)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitmap.h"

struct map {
    unsigned int max_size;        ///< tamanho maximo em bits
    unsigned int length;         ///< tamanho atual em bits
    unsigned char* contents;     ///< conteudo do mapa de bits
	unsigned int capacity;      ///< capacidade alocada em bits
};

/**
 * Funcao auxiliar que imprime uma mensagem de falha e aborta o programa caso testresult seja falso.
 * @param testresult Valor booleano representando o resultado do teste que deveria ser verdadeiro.
 * @param message A mensagem para ser impressa se resultado do teste for falso.
 */
void assert(int testresult, char* message) {
	if (!testresult) {
		printf("%s\n", message);
		exit(EXIT_FAILURE);
	}
}


/**
 * Retorna o conteudo do mapa de bits.
 * @param bm O mapa de bits.
 */
unsigned char* bitmapGetContents(bitmap* bm) {
	return bm->contents;
}

/**
 * Retorna o tamanho maximo do mapa de bits.
 * @param bm O mapa de bits.
 * @return O tamanho maximo do mapa de bits.
 */
unsigned int bitmapGetMaxSize(bitmap* bm) {
	return bm->max_size;
}

/**
 * Retorna o tamanho atual do mapa de bits.
 * @param bm O mapa de bits.
 * @return O tamanho atual do mapa de bits.
 */
unsigned int bitmapGetLength(bitmap* bm) {
	return bm->length;
}

/**
 * Constroi um novo mapa de bits, definindo um tamanho maximo.
 * @param max_size O tamanho maximo para o mapa de bits.
 * @return O mapa de bits inicializado.
 */
bitmap* bitmapInit(unsigned int initial_capacity) {
    bitmap* bm = (bitmap*)malloc(sizeof(bitmap));
    unsigned int capacityInBytes = (initial_capacity + 7) / 8;
    bm->contents = calloc(capacityInBytes, sizeof(char));
    assert(bm->contents != NULL, "Erro de alocacao de memoria.");
    bm->capacity = initial_capacity;
    bm->length = 0;
    bm->max_size = initial_capacity; // pode remover se não for mais usar
    return bm;
}

/**
 * Retorna o valor do bit na posicao index.
 * @param bm O mapa de bits.
 * @param index A posicao do bit.
 * @pre index<bitmapGetLength(bm)
 * @return O valor do bit.
 */
unsigned char bitmapGetBit(bitmap* bm, unsigned int index) // index in bits
{
	// verificar se index<bm.length, pois caso contrario, index e' invalido
	assert(index<bm->length, "Acesso a posicao inexistente no mapa de bits.");
	// index/8 e' o indice do byte que contem o bit em questao
	// 7-(index%8) e' o deslocamento do bit em questao no byte
	return (bm->contents[index/8] >> (7-(index%8))) & 0x01;
}

/**
 * Modifica o valor do bit na posicao index.
 * @param bm O mapa de bits.
 * @param index A posicao do bit.
 * @param bit O novo valor do bit.
 * @post bitmapGetBit(bm,index)==bit
 */
static void bitmapSetBit(bitmap* bm, unsigned int index, unsigned char bit) {
    // verificar se index<bm->length, pois caso contrario, index e' invalido
    assert(index<bm->length, "Acesso a posicao inexistente no mapa de bits.");
    // index/8 e' o indice do byte que contem o bit em questao
    // 7-(index%8) e' o deslocamento do bit em questao no byte
    bit=bit & 0x01;
    bit=bit<<(7-(index%8));
    bm->contents[index/8]= bm->contents[index/8] | bit;
}


static void bitmapEnsureCapacity(bitmap* bm, unsigned int min_capacity) {
    if (bm->capacity >= min_capacity) return;
    unsigned int new_capacity = bm->capacity ? bm->capacity * 2 : 1024;
    while (new_capacity < min_capacity) {
        new_capacity *= 2;
    }
    unsigned int new_bytes = (new_capacity + 7) / 8;
    bm->contents = realloc(bm->contents, new_bytes);
    assert(bm->contents != NULL, "Erro de realocacao de memoria.");
    // zera a nova área alocada
    unsigned int old_bytes = (bm->capacity + 7) / 8;
    if (new_bytes > old_bytes) {
        memset(bm->contents + old_bytes, 0, new_bytes - old_bytes);
    }
    bm->capacity = new_capacity;
}

/**
 * Adiciona um bit no final do mapa de bits.
 * @param bm O mapa de bits.
 * @param bit O novo valor do bit.
 * @pre bitmapGetLength(bm) < bitmapGetMaxSize(bm)
 * @post (bitmapGetBit(bm,bitmapGetLength(bm) @ pre)==bit) 
 * and (bitmapGetLength(bm) == bitmapGetLength(bm) @ pre+1)
 */
void bitmapAppendLeastSignificantBit(bitmap* bm, unsigned char bit) {
    bitmapEnsureCapacity(bm, bm->length + 1);
    bm->length++;
    bitmapSetBit(bm, bm->length-1, bit);
}
/**
 * Libera a memória dinâmica alocada para o mapa de bits.
 * @param bm O mapa de bits.
 */
void bitmapLibera (bitmap* bm){
    
    free (bm->contents);
    free (bm);
}

/**
 * Remove o último bit adicionado ao mapa de bits.
 * @param bm O mapa de bits.
 */
void bitmapRemoveLastBit(bitmap* bm) {
    if (bm->length > 0) {
        //encontra a posiçao do ultimo bit antes de decrementar o length
        unsigned int byte_index = (bm->length - 1) / 8;
        unsigned int bit_offset = 7 - ((bm->length - 1) % 8);
        
        //zera o bit na posiçao do antigo ultimo bit
        bm->contents[byte_index] &= ~(1 << bit_offset);
        
        //decrementa
        bm->length--;
    }
}