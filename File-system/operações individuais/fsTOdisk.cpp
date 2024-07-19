#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

typedef struct {
    char file_name[12], image_name[15];
    int numero_setores;
    int size;
    long long int primeiro_sector;
    int entrada_diretorio;
    long long int bitmap;
}copia;

copia localiza(copia informacoes){

    char file_SA[12];

    FILE *fp;
    fp = fopen(informacoes.image_name, "rb");

    fseek(fp, 512, SEEK_SET);
    fread(&informacoes.bitmap ,8,1, fp);

    for (int i = 0; i < 128; i++){

        fseek(fp, 512+(i*32), SEEK_SET);
        fread(&file_SA, 12,1, fp);

        int resultado = strcmp(file_SA, informacoes.file_name);

        if (resultado == 0){

            cout <<endl << "ARQUIVO ENCONTRADO" << endl << endl;
            informacoes.entrada_diretorio = i;

            fseek(fp, 512+(i*32)+12, SEEK_SET);
            fread(&informacoes.primeiro_sector, 8,1, fp);

            fseek(fp, 512+(i*32)+20, SEEK_SET);
            fread(&informacoes.size, 4,1, fp);

            break;
        }

        if (i == 127){
            cout << endl << "NÃO FOI POSSÍVEL ENCONTRAR SEU ARQUIVO" << endl;
            informacoes.primeiro_sector = -1;
        }
    }
            return informacoes;
}

void escrita_em_disco(copia informacoes){

    FILE *fp;
    fp = fopen(informacoes.image_name, "rb");

    if (fp == nullptr) {
        cout << "Erro ao abrir o arquivo de imagem.\n";
        return;
    }

    std::ofstream out(informacoes.file_name);
    if (!out.is_open()) {
        cout << "Erro ao abrir o arquivo de saída.\n";
        fclose(fp);
        return;
    }

    char k;

    for (int i = 0; i < informacoes.size; i++){
    fseek(fp,(512 * informacoes.primeiro_sector)+i, SEEK_SET);
        fread(&k, 1, 1, fp);
        out << k;
    }

    fclose(fp);
    out.close();

    std::cout << "SUAS INFORMAÇÕES FORAM GRAVADAS NO SISTEMA DE ARQUIVOS" << std::endl;
}

int main(){

    copia informacoes;
    cout << endl << "Qual o nome do arquivo que você deseja copiar para o disco?" << endl << endl;
    cin >> informacoes.file_name; 

    cout << endl << "Em que imagem este arquivo está localizado?" << endl << endl;
    cin >> informacoes.image_name; 

    informacoes = localiza(informacoes);
    if (informacoes.primeiro_sector == -1)
        return 0;
    
    escrita_em_disco(informacoes);
    
    return 0;
}