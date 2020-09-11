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
    // printf("Dados: %s \n", msg->data);
    printf("Dados:");
     for (int i = 0; i < 15; i++){
        printf("%c", msg->data[i]);
    }
    printf("\nParidade: %d \n ----------------- \n", msg->parity);
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
    for (int i = 0; i < 15; i++)
        msg->data[i] = NULL;
    for (int i = 0; i < size; i++){
        printf("%c", data[i]);
        msg->data[i] = data[i];
    }
    printf("\n");
    getParity(msg);
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
    printf("Diretório modificado! \n"); 
    printf("Diretório atual: %s\n", getcwd(cwd, 100)); 
}

void cd(Message *msg, Message *msg_recv, char *arg, int socket){
    struct timeval  tv1, tv2;
    double tDecorrido;
    char controle[20];
    int size = strlen(arg);
    if (size > 15){
        printf("Insira um argumento com no máximo 15 caracteres");
        return;
    }
    setMessage(msg, '~' , size, 0, 0, arg); 
    jump:
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
            getControle(controle);
                // printf("%s \n",controle);
            if(strcmp(controle, "Servidor") == 0){
                recv(socket, msg_recv, sizeof(*msg_recv), 0);
                recv(socket, msg_recv, sizeof(*msg_recv), 0);
                if(msg_recv && msg_recv->marker == '~'){ 
                    printMsg(msg_recv);
                    setControleCliente();
                    if(msg_recv->type == 8){
                        printf("Recebeu ACK do CD \n \n \n");
                        break;
                    }
                    if(msg_recv->type == 9){
                        printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                        goto jump;
                    }
                    if(msg_recv == msg){
                        printf("Mensagem ignorada! ACK do CD \n \n \n");
                        break;
                    }
                }
            }
            gettimeofday(&tv2, NULL);
            tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
            if (tDecorrido > 2){
                printf("Time Out \n \n \n");
                goto jump;
            }
        }
    }

}

void ls(Message *msg, Message *msg_recv, char *arg, int socket){
    struct timeval  tv1, tv2;
    double tDecorrido;
    char controle[20];
    int size = strlen(arg);
    if (size > 15){
        printf("Insira um argumento com no máximo 15 caracteres");
        return;
    }
    setMessage(msg, '~' , size, 0, 1, arg); 
    jump:
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
            getControle(controle);
            // if(strcmp(controle, "Servidor") == 0){
                recv(socket, msg_recv, sizeof(*msg_recv), 0);
                recv(socket, msg_recv, sizeof(*msg_recv), 0);
                if(msg_recv && msg_recv->marker == '~'){ 
                    printMsg(msg_recv);
                    // setControleCliente();
                    if (msg_recv->type == 13){ //FIM DE TRANSMISSÃO
                        setMessage(msg, '~' , 0, 0, 8, 0); //Enviando ACK
                        send(socket, msg, sizeof(*msg), 0);
                        printf("ENVIOU ACK\n");
                        setControleCliente();
                        sleep(2);
                        return;
                    }
                    
                    if(msg_recv->type == 11){
                        setMessage(msg, '~' , 0, 0, 8, 0); //Enviando ACK
                        send(socket, msg, sizeof(*msg), 0);
                        printf("ENVIOU ACK\n");
                    }
                    if(msg_recv->type == 9){
                        printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                        goto jump;
                    }
                }
            // }
            gettimeofday(&tv2, NULL);
            tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
            if (tDecorrido > 2){
                printf("Time Out \n \n \n");
                goto jump;
            }
        }
    }

}

void setControleCliente(){
    //Seta quem esta na vez de escrever
    FILE *file;
    file = fopen("/home/gabriel/Documentos/redes-1/controle.txt","w");
    if (file != NULL){
        fputs("Cliente", file);
    };
    fclose(file);
}

void setControleServidor(){
    //Seta quem esta na vez de escrever
    FILE *file;
    file = fopen("/home/gabriel/Documentos/redes-1/controle.txt","w");
    if (file != NULL){
        fputs("Servidor", file);
    };

    fclose(file);
}

void getControle(char *controle){
    FILE *file;
    file = fopen("/home/gabriel/Documentos/redes-1/controle.txt","r");
    fscanf(file, "%s", controle);
    fclose(file);
}