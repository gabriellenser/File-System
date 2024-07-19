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

    std::cout << "SUAS INFORMAÇÕES FORAM GRAVADAS NO  DISCO" <<  endl << endl;
}

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

        cout << endl << "O SETOR NÚMERO " << informacoes.primeiro_sector+i << " AGORA ESTÁ LIVRE" << endl << endl;
    }
}

void listagem(char image_name[15]){

    char file_SA[12];
    int size;
    long long int primeiro_sector;
    int a = 0;

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
        a++;

    }

    if (a == 0)
        cout << "SEU SISTEMA NÃO POSSUI NENHUM ARQUIVO" << endl << endl ;

}

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
        cout << endl << "O SETOR NÚMERO " << informacoes.primeiro_sector+i << " AGORA ESTÁ OCUPADO" << endl << endl;
    }
}

bool arquivoExiste(char image_name[15]) {
    std::ifstream arquivo(image_name);
    return arquivo.good(); 
}

int main(){

    char image_name[15];
    
    cout << endl << "EM QUE IMAGEM VOCÊ DESEJA REALIZAR OPERAÇÕES?" << endl << endl;
    cout << "Observação: caso seu arquivo não exista, ocorrerá segmentation fault e o programa será encerrado" << endl << endl;
    
    cin >> image_name; 
  
    if(!arquivoExiste(image_name)){
        cout << "O ARQUIVO NAO EXISTE" << endl;
        return 0;
    }

    bool teste = false;
    for(int i = 0; i < sizeof(image_name); i++){    
        if (image_name[i] == '.'){
            teste = true;
            if (image_name[i+1] != 'i' || image_name[i+2] != 'm' || image_name[i+3] != 'g'){
                cout << "ERRO, NÃO É UMA IMAGEM" << endl;
                return 0;
        }}
    }

    if (!teste){
        cout << "ERRO, NÃO É UMA IMAGEM" << endl;
        return 0;
    }

    while(1){

    cout << endl;
    cout << "PARA EXCLUIR UM ARQUIVO DO SISTEMA TECLE (1)" << endl;
    cout << "PARA LISTAR OS ARQUIVOS DO SISTEMA TECLE (2)" << endl;
    cout << "PARA COPIAR UM ARQUIVO DO SISTEMA PARA O DISCO TECLE (3)" << endl;
    cout << "PARA COPIAR UM ARQUIVO DO DISCO PARA O SISTEMA TECLE (4)" << endl << endl;

    char op;
    cin >> op;

        if (op == '1'){

            copia informacoes;
            strcpy(informacoes.image_name, image_name);

            cout << endl << "Qual o nome do arquivo que você deseja exluir?" << endl << endl;
            cin >> informacoes.file_name; 

            informacoes = localiza_arquivo_fs(informacoes);
            if (informacoes.primeiro_sector == -1)
                return 0;
            
            excluir(informacoes);
            bitmap(informacoes);

        }else if(op == '2'){

            listagem(image_name);

        }else if( op == '3'){

            copia informacoes;
            strcpy(informacoes.image_name, image_name);

            cout << endl << "Qual o nome do arquivo que você deseja copiar para o disco?" << endl << endl;
            cin >> informacoes.file_name; 

            informacoes = localiza_arquivo_fs(informacoes);
            if (informacoes.primeiro_sector == -1)
                return 0;
        
            escrita_em_disco(informacoes);

        }else if(op == '4'){
        
            copia informacoes;
            strcpy(informacoes.image_name, image_name);
            
            cout << endl << "Qual o nome do arquivo que você deseja copiar para o sistema de arquivos?" << endl << endl;
            cin >> informacoes.file_name; 

            informacoes.size = file_size(informacoes);
            if (informacoes.size == -1){
                cout << "ERRO! " << endl;
                return 0;
            }

            informacoes.numero_setores = static_cast<int>(std::ceil(static_cast<double>(informacoes.size) / 512));

            informacoes = encontrar_primeiro_setor(informacoes);
            if (informacoes.primeiro_sector < 0){
                cout << "ERRO! NÃO EXISTE ESPAÇO EM MEMORIA" << endl;
                return 0;
            }

            escrita_em_fs(informacoes);

        }else{
            cout << endl << endl << "INVALIDO" << endl << endl;
            break;
        }
    }

    return 0;
}