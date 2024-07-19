#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cmath> 
#include <stdio.h>
#include <math.h>

using namespace std;

typedef struct {
    char file_name[12], image_name[15];
    int numero_setores;
    int size;
    long long int primeiro_sector;
    int entrada_diretorio;
    long long int bitmap;
}copia;

copia localiza_arquivo_fs(copia informacoes){

    char file_SA[12];

    FILE *fp;
    fp = fopen(informacoes.image_name, "rb");

    fseek(fp, 14, SEEK_SET);
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
            
            informacoes.numero_setores = static_cast<int>(std::ceil(static_cast<double>(informacoes.size) / 512));

            break;
        }

        if (i == 127){
            cout << endl << "NÃO FOI POSSÍVEL ENCONTRAR SEU ARQUIVO" << endl;
            informacoes.primeiro_sector = -1;
        }
    }
            return informacoes;
}

void excluir(copia informacoes){

    std::ofstream writable_file(informacoes.image_name, std::ios::binary | std::ios::in | std::ios::out);
    if (!writable_file.is_open()) {
        return;
    }
    
    unsigned char excluido = 229;
    
    std::streampos position = 512 + (informacoes.entrada_diretorio * 32);
    writable_file.seekp(position);
    writable_file.write(reinterpret_cast<const char*>(&excluido), 2);

}

void bitmap(copia informacoes){

    std::ofstream writable_file(informacoes.image_name, std::ios::binary | std::ios::in | std::ios::out);
    if (!writable_file.is_open()) {
        return;
    }
    
    cout << informacoes.numero_setores;
    informacoes.numero_setores =1;

    for (int i = 0; i < informacoes.numero_setores; i++) {

    std::streampos position = (informacoes.bitmap*512)+informacoes.primeiro_sector+i;
    writable_file.seekp(position);
    char zero_byte = 0;
    writable_file.write(&zero_byte, 1);

        cout << endl << "O SETOR NÚMERO " << informacoes.primeiro_sector+i << " AGORA ESTÁ LIVRE" << endl;
    }
}

int main(){

    copia informacoes;
    cout << endl << "Qual o nome do arquivo que você deseja exluir?" << endl << endl;
    cin >> informacoes.file_name; 

    cout << endl << "Em que imagem este arquivo está localizado?" << endl << endl;
    cin >> informacoes.image_name; 

    informacoes = localiza_arquivo_fs(informacoes);
    if (informacoes.primeiro_sector == -1)
        return 0;
    
    excluir(informacoes);
    bitmap(informacoes);
        
    return 0;
}