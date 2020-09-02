#include "infos.h"
void showMenu(){
    printf("Digite seu comando: \n");
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

void checkParity(Message *msg){
    unsigned char p = msg->seq^msg->type;
    // int tam = msg.size;
    // char a = msg->size;
    // for (int i = 0; i < 8; i++) {
    //     printf("%d", !!((a << i) & 0x80));
    // }
    // printf("\n");
    printf("X: %d", p);
}