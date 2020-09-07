#include "infos.h"
void showMenu(){
    // putp(enter_bold_mode);
    printf("Você está utilizando o CLIENTE!\n");
    printf("Pressione 'h' para obter ajuda!\n");
    // putp(exit_attribute_mode);
    printf("Digite seu comando: \n");
}

void showHelp(){
    printf("COMANDOS:\n");
    printf("1 - cd <nome_dir>\n");
    printf("2 - lcd <nome_dir>\n");
    printf("3 - ls\n");
    printf("4 - lls\n");
    printf("5 - ver <nome_arq>\n");
    printf("6 - linha <numero_linha> <nome_arq>\n");
    printf("7 - linhas <numero_linha_inicial> <numero_linha_final> <nome_arq>\n");
    printf("8 - edit <numero_linha> <nome_arq> '<NOVO_TEXTO>' \n");
}

void printMsg(Message *msg){
    if (msg->marker == 126)
    {
        printf("Marcador: %c \n", msg->marker);
        printf("Tamanho: %d \n", msg->size);
        printf("Sequencialização: %d \n", msg->seq);
        printf("Tipo: %d \n", msg->type);
        printf("Dados: %s \n", msg->data);
        printf("Paridade: %d \n", msg->parity);
    }
}

// void getParity(Message *msg){
//     unsigned char p = msg->seq^msg->type;
//     for (int i = 0; i < msg->size; i++) {
//         p ^⁼ msg->data[i];
//     }
//     msg->parity = p;
// }

void setMessage(Message *msg, unsigned char marker, unsigned char size, unsigned char seq, unsigned char type, unsigned char data){
    msg->marker = marker;
    msg->size = size;
    msg->seq = seq;
    msg->type = type;
    // msg->data = data;
    msg->data[0] = 'a';

}

void lls(){

}