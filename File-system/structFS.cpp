#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint64_t high;
    uint16_t low;
} area_bitmap;

typedef struct {
    unsigned int byter_por_setor;
    unsigned int setores_reservados;
    unsigned int numero_de_entradas_DR;
    unsigned long long int tam_diretorio_raiz;    
    area_bitmap bitmap;
} __attribute__((packed)) bootRecord;//ignora o alinhameto de memoria de pares

typedef struct {
    char nome[9];
    char ext[3];
    unsigned long long int primeiro_setor;
    unsigned int tam_arquivo;
    unsigned long long int numero_setores;
} __attribute__((packed)) diretorio_Raiz;

typedef struct {
    uint8_t bits;  
    size_t tam_bitmap;    
} bitmap;

typedef struct {
    uint8_t data[512]; 
} dataSector;

typedef struct {
    dataSector *setores;
    uint32_t num_setores; 
} areaDados;

typedef struct {
    bootRecord bootRecord;
    diretorio_Raiz Diretorio_raiz;    
    areaDados Area_de_dados;
    bitmap bitmap;
} SistemaArquivos;