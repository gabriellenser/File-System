#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;


void listagem(char image_name[15]){

    char file_SA[12];
    int size;
    long long int primeiro_sector;

    FILE *fp;
    fp = fopen(image_name, "rb");

    for (int i = 0; i < 128; i++){

        fseek(fp, 512+(i*32), SEEK_SET);
        fread(&file_SA, 12,1, fp);

        fseek(fp, 512+(i*32)+12, SEEK_SET);
        fread(&primeiro_sector, 8,1, fp);

        fseek(fp, 512+(i*32)+20, SEEK_SET);
        fread(&size, 4,1, fp);

        if(file_SA[0] == 0)
            break;

        if(file_SA[0] == -27)
            continue;

        cout << "NOME: " << file_SA  << endl;
        cout << "TAMANHO: " << size  << endl;
        cout << "PRIMEIRO SETOR: " << primeiro_sector << endl << endl;

    }

}

int main(){

    char file_SA[15];
    cout << "QUAL O NOME DA IMAGEM QUE VOCÊ DESEJA LISTAR OS ARQUIVOS?" << endl << endl;
    cout << "Observação: caso seu arquivo não exista, ocorrerá segmentation fault e o programa será encerrado" << endl << endl;
    cin >> file_SA;

    listagem(file_SA);
    return 0;
    
}