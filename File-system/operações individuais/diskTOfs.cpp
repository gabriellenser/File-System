#include <cmath> 
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
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

int file_size(copia informacoes) {
    ifstream in(informacoes.file_name);

    if (!in.is_open()) {
        cerr << endl << "ERRO NA ABERTURA DO ARQUIVO" << endl;
        return -1;
    }

    in.seekg(0, ios::end);
    streampos filesize = in.tellg();
    in.close(); 

    cout << "tamanho: " << filesize << " bytes" << endl;
    return filesize;
}


copia encontrar_primeiro_setor(copia informacoes) {
    long long int tamanho_bitmap = 0;
    FILE *fp;
 
    fp = fopen(informacoes.image_name, "rb");

    if (fp == nullptr) {
        std::cerr << "Erro na abertura do arquivo de imagem." << std::endl;
        informacoes.primeiro_sector = -1;
        return informacoes;
    }

    fseek(fp, 14, SEEK_SET);
    fread(&informacoes.bitmap ,8,1, fp);

    fseek(fp, 22, SEEK_SET);
    fread(&tamanho_bitmap ,8,1, fp);

    int aux = 0, cont = 0;
    for (int i = 0; i < tamanho_bitmap; i++) {
        fseek(fp, (informacoes.bitmap * 512) + i, SEEK_SET);
        fread(&aux, 1, 1, fp);
        if(aux == 0)
            cont++;
        else
            cont = 0;
        if(cont == informacoes.numero_setores){
            aux = i;
            break;
        } else if (i == tamanho_bitmap) {
            aux = -1;
        }
    }
    if (aux < 0) {
        cout << "ERRO! NÃO EXISTE ESPAÇO EM MEMORIA" << endl;
        informacoes.primeiro_sector = -1;
    } else {
        informacoes.primeiro_sector = (aux - informacoes.numero_setores + 1);
    }

    fclose(fp);
    return informacoes;
}

void escrita_em_fs(copia informacoes){

    char aux;

    ifstream in(informacoes.image_name);
    in.seekg(512, std::ios::beg);

    for (int i = 0; i < 128; i++){

        in.seekg(512+(i*32), std::ios::beg);
        in >> aux;
        if (aux == 0){
            informacoes.entrada_diretorio = i;
            break;
        }
    }
    
    in.close();

    std::ofstream writable_file(informacoes.image_name, std::ios::binary | std::ios::in | std::ios::out);
    if (!writable_file.is_open()) {
        return;
    }

    std::streampos position = 512 + (informacoes.entrada_diretorio * 32);
    writable_file.seekp(position);
    writable_file.write(informacoes.file_name, 12);

    position = 512 + (informacoes.entrada_diretorio * 32) +12;
    writable_file.seekp(position);
    writable_file.write(reinterpret_cast<const char*>(&informacoes.primeiro_sector), sizeof(informacoes.primeiro_sector));
    
    position = 512 + (informacoes.entrada_diretorio * 32) +20;
    writable_file.seekp(position);
    writable_file.write(reinterpret_cast<const char*>(&informacoes.size), sizeof(informacoes.size));
    
    position = 512 + (informacoes.entrada_diretorio * 32) +24;
    writable_file.seekp(position);
    writable_file.write(reinterpret_cast<const char*>(&informacoes.numero_setores), sizeof(informacoes.numero_setores));

    cout << endl << "AS INFORMAÇÕES SOBRE SEU ARQUIVO JÁ ESTÃO DISPONÍVEIS NO DIRETÓRIO RAÍZ NA ENTRADA NÚMERO " << informacoes.entrada_diretorio << endl;

    ifstream inn(informacoes.file_name);
    char k;

    for (int i = 0; i < informacoes.size; i++) {
        position = 512 * informacoes.primeiro_sector + i;
        inn.get(k);  
        writable_file.seekp(position);
        writable_file.write(reinterpret_cast<const char*>(&k), 1);
    }

    cout << endl << "AS INFORMAÇÕES DE SEU ARQUIVO JÁ ESTÃO DISPONÍVEIS NA ÁREA DE DADOS, A PARTIR DO SETOR " << informacoes.primeiro_sector << " E OCUPA " << informacoes.numero_setores << " SETOR(ES)." << endl << endl;

    
    inn.close();

    k = 1;
    for (int i = 0; i < informacoes.numero_setores; i++) {
        position = 512 * informacoes.bitmap + informacoes.primeiro_sector+i; 
        writable_file.seekp(position);
        writable_file.write(reinterpret_cast<const char*>(&k), 1);
        cout << endl << "O SETOR NÚMERO " << informacoes.primeiro_sector+i << " AGORA ESTÁ OCUPADO" << endl;
    }
}



int main(){

    copia informacoes;

        cout << endl << "Qual o nome do arquivo que você deseja copiar para o sistema de arquivos?" << endl << endl;
        cin >> informacoes.file_name; 

        informacoes.size = file_size(informacoes);
        if (informacoes.size == -1){
            cout << "ERRO! " << endl;
            return 0;
        }

        cout << endl << "Para qual imagem você deseja fazer a cópia?" << endl << endl;
        cin >> informacoes.image_name; 
        
        informacoes.numero_setores = static_cast<int>(std::ceil(static_cast<double>(informacoes.size) / 512));

        informacoes = encontrar_primeiro_setor(informacoes);
        if (informacoes.primeiro_sector < 0){
            cout << "ERRO! NÃO EXISTE ESPAÇO EM MEMORIA" << endl;
            return 0;
        }

       escrita_em_fs(informacoes);
}