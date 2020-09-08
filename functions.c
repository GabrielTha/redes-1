#include "infos.h"
#include <limits.h>

void showMenu(){
    putp(enter_bold_mode);
    printf("Você está utilizando o CLIENTE!\n");
    printf("Pressione 'h' para obter ajuda!\n");
    putp(exit_attribute_mode);
    printf("Digite seu comando: \n");
}

void showHelp(){
    system("clear"); 
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
    printf("\nMarcador: %c \n", msg->marker);
    printf("Tamanho: %d \n", msg->size);
    printf("Sequencialização: %d \n", msg->seq);
    printf("Tipo: %d \n", msg->type);
    printf("Dados: %s \n", msg->data);
    printf("Paridade: %d \n", msg->parity);
}

void getParity(Message *msg){
    unsigned char p = msg->size^msg->seq^msg->type;
    for (int i = 0; i < msg->size; i++) {
        p ^= msg->data[i];
    }
    msg->parity = p;
}

int checkParity(Message *msg){
    unsigned char p = msg->size^msg->seq^msg->type;
    for (int i = 0; i < msg->size; i++)
        p ^= msg->data[i];
    if (p == msg->parity)
        return 1;
    else
        return 0;
    
}

void setMessage(Message *msg, unsigned char marker, unsigned char size, unsigned char seq, unsigned char type, unsigned char *data){
    msg->marker = marker;
    msg->size = size;
    msg->seq = seq;
    msg->type = type;
    for (int i = 0; i < size; i++)
        msg->data[i] = data[i];
    getParity(msg);
    printMsg(msg);

}

void lls(){
    char cwd[PATH_MAX];
    DIR *dp;
    struct dirent *ep;     
    dp = opendir (getcwd(cwd, 100));
    if (dp != NULL){
        while (ep = readdir (dp))
        if (ep->d_type == 4)
            printf("%s/ \n", ep->d_name);
        else
            printf("%s \n", ep->d_name);
        (void) closedir (dp);
        printf("\n");
    }
    else
        perror ("Não foi possível realizar essa ação! \n");

}

void lcd(char *arg){
    char cwd[PATH_MAX];
    chdir(arg); 
    printf("Diretório atual: %s\n", getcwd(cwd, 100)); 
}

void cd(Message *msg, char *arg, int socket){
    struct timeval  tv1, tv2;
    double tDecorrido;
    int size = strlen(arg);
    if (size > 15){
        printf("Insira um argumento com no máximo 15 caracteres");
        return;
    }
    setMessage(msg, '~' , size, 1, 0, arg); 
    if (send(socket, msg, sizeof(*msg), 0) == -1)            {
        printf("Erro ao enviar mensagem! \n");
        printf("Erro: %s \n", strerror(errno));
    }
    else
    {
        gettimeofday(&tv1, NULL);
        printf("Mensagem enviada com sucesso! \n");
        printf("Aguardando resposta do Servidor! \n \n");
        while(1){
            // recv(socket, &message_recv, sizeof(message_recv), 0);
            // if(&message_recv){
            //     printf("Recebeu retorno");
            //     printMsg(&message_recv);
            //     break;
            // }
            gettimeofday(&tv2, NULL);
            tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
            if (tDecorrido > 12){
                printf("Time Out \n \n \n");
                break;
            }
        }
    }

}