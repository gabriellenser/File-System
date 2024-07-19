#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

//VARIAVEIS FIXAS//
#define setores_bootR_rootD 9
#define bytes_por_setor 512
#define setores_Reservados 1
#define n_entradas_diretorio_raiz 128
#define tam_bytes_entradaDR 32

//ESTRUTURA DO SISTEMA DE ARQUIVOS//
typedef struct {
    unsigned short int byter_por_setor;
    unsigned short int setores_reservados;
    unsigned short int numero_de_entradas_DR;
    unsigned long long int tam_diretorio_raiz;    
    unsigned long long int setores_bitmap;
    unsigned long long int tam_bitmap;
    unsigned char resto[482];
}  __attribute__((packed)) bootRecord;

typedef struct {
    char nome[9];
    char ext[3];
    unsigned long long int primeiro_setor;
    unsigned int tam_arquivo;
    unsigned long long int numero_setores;
} entrada_diretorio_Raiz;

typedef struct {
    entrada_diretorio_Raiz *diretorio_Raiz;
} diretorio_Raiz;

typedef struct {
    char setor[512]; 
} setor;

typedef struct {
    setor *bytemap; 
} bytemap;

typedef struct {
    setor *setores_dados; 
} areaDados;
/////////////////////////////////////////////////////////////////////////////////////
//FUNÇÕES//
int zeraRestoBR(bootRecord *bootRecord, int resto){
    for(int i=0; i< resto; i++){
        bootRecord->resto[i] = 0;
    }
}

int bytemapPadrao(bytemap *bytemap, int setores_bytemap, int setor_dados, int setores_totais_bytemaps){
    //colocar os primeiros 9 setores ocupados no primeiro setor e depois ocupar o setores do bytemap
    for(int i =0; i < 9; i++){
        bytemap->bytemap[0].setor[i] = 1;
    }

    for(int i =0; i < setores_bytemap; i++){
        bytemap->bytemap[0].setor[9 + setor_dados + i] = 1;
    }
    
    //preencher os acessos fantasmas
    int ghost = setores_totais_bytemaps * 512 - 9 - setor_dados - setores_bytemap;
    //printf("%d\n", ghost);
    for(int i =0; i < ghost; i++){
        bytemap->bytemap[0].setor[9 + setor_dados + setores_bytemap + i] = 1;
    }
    
}
/////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
    //VARIAVEIS//////////////////////////////////////////
    int tamanho_em_setores_dados_bytemap = atoi(argv[1]) - 9;// 9 é a soma do boot record e os setores do root dir
    int totais_setores = atoi(argv[1]);
    double setores_bitmap;
    int setores_area_de_dados;
    int tam_bitmap_bytes;

    //CONTAS PARA INFORMAÇÕES NO BOOT RECORD/////////////////////////////////
    printf("setores sem boot record e diretorio raiz: %d\n",tamanho_em_setores_dados_bytemap);

    //calculo dos setores bitmap e do tamanho em bytes///////////////////////
    setores_bitmap = (float)totais_setores/bytes_por_setor;
    if(setores_bitmap < 1.0){
        setores_bitmap = 1.0;
    }else{
        setores_bitmap = ceilf(setores_bitmap);
    } 
    printf("Setores para o bitmap com o bitmap: %.1f\n", setores_bitmap);
    tam_bitmap_bytes = setores_bitmap * 512;


    //calculo dos setores da area de dados/////////////////////////////////////
    setores_area_de_dados = totais_setores - setores_bootR_rootD - setores_bitmap;
    printf("Setores para a area de dados: %d\n", setores_area_de_dados);
    
    // Aloca na memoria pelo tamaho oferecido do user para as áreas que foram definidas na estrutura SistemaArquivos
    areaDados dataArea;
    bytemap bytemap;
    diretorio_Raiz diretorio_Raiz_area;
    bootRecord bootRecord;
    
    int alocacao_area_de_dados = setores_area_de_dados * sizeof(setor);
    int alocacao_bytemap = setores_bitmap * sizeof(setor);
    int aloocacao_diretorio_raiz = n_entradas_diretorio_raiz * tam_bytes_entradaDR;

    diretorio_Raiz_area.diretorio_Raiz = (entrada_diretorio_Raiz*)malloc(aloocacao_diretorio_raiz);
    dataArea.setores_dados = (setor *)malloc(alocacao_area_de_dados);
    bytemap.bytemap = (setor*)malloc(alocacao_bytemap);

    //ZERA O BYTEMAP////////////////////////////////////////
    memset(bytemap.bytemap, 0, alocacao_bytemap);

    //ZERA DIRETORIO RAIZ//////////////////////////////////////////////////////
    memset(diretorio_Raiz_area.diretorio_Raiz, 0, aloocacao_diretorio_raiz);

    //ZERA O RESTO BOOT RECORD///////////////////////////////////////////////////////
    zeraRestoBR(&bootRecord, sizeof(bootRecord.resto));
    

    //DEFININDO ALGUMAS IFNORMAÇÕES PADRÕES////////////////////////////////////
    bootRecord.byter_por_setor  = bytes_por_setor;
    bootRecord.setores_reservados = setores_Reservados;
    bootRecord.numero_de_entradas_DR = n_entradas_diretorio_raiz;
    bootRecord.tam_diretorio_raiz = n_entradas_diretorio_raiz * tam_bytes_entradaDR; //num_de_Entradas * 32 bytes da estrutura do root dir

    //OUTRAS INFORMAÇÕES QUE VEM DO NUMERO DE SETORES O USER DEFINIU///////////
    bootRecord.setores_bitmap = 9 + setores_area_de_dados;
    bootRecord.tam_bitmap = tam_bitmap_bytes;
    diretorio_Raiz_area.diretorio_Raiz = diretorio_Raiz_area.diretorio_Raiz;
    dataArea.setores_dados = dataArea.setores_dados;
    bytemap.bytemap = bytemap.bytemap;

    //DEFININDO O BITMAP PADRÃO////////////////////////////////////////////////
    bytemapPadrao(&bytemap, setores_bitmap, setores_area_de_dados, setores_bitmap);

   //ESCREVENDO A IMAGEM DE DISCO////////////////////////////////////////////
    FILE *file = fopen("disk_image.img", "wb");

    // escreve a estrutura do sistema de arquivos   

    fwrite(&bootRecord, sizeof(bootRecord), 1, file);

    fwrite(diretorio_Raiz_area.diretorio_Raiz, aloocacao_diretorio_raiz, 1, file);

    fwrite(dataArea.setores_dados, alocacao_area_de_dados, 1, file);

    fwrite(bytemap.bytemap, alocacao_bytemap, 1, file);

    // fecha arquivo
    fclose(file);
    
    // libera memoria alocada antes
    free(diretorio_Raiz_area.diretorio_Raiz);
    free(dataArea.setores_dados);
    free(bytemap.bytemap);

    printf("arquivo escrito!!!! uhuuuuuuuu.\n");
    return 0;
}
