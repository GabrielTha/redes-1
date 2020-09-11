#include "infos.h"
#include "message.h"
#include "functions.c"
#include "rawSocket.c"

int main(void) {
    int socket = ConexaoRawSocket("lo");
    Message message_recv;
    Message message_send;
    char controle[20];
    setupterm(NULL, STDOUT_FILENO, NULL);

    origem:
    while(1){
        getControle(controle);
        if (strcmp(controle, "Cliente") == 0){
            recv(socket, &message_recv, sizeof(message_recv), 0);
            recv(socket, &message_recv, sizeof(message_recv), 0);
            if(&message_recv && message_recv.marker == '~'){ 
                setControleServidor();
                int t = checkParity(&message_recv);
                if((message_recv.type == message_send.type)){
                    //Recebendo mensagem que eu mesmo enviei
                }
                if(checkParity(&message_recv) == 0){
                    setMessage(&message_send, '~' , 0, 0, 9, 0); //Enviando ACK
                    send(socket, &message_send, sizeof(message_send), 0);
                    printf("ENVIOU ACK\n");
                }
                if (message_recv.type == 0) //CD
                {
                    printMsg(&message_recv);
                    lcd(message_recv.data);
                    setMessage(&message_send, '~' , 0, 0, 8, 0); //ACK
                    send(socket, &message_send, sizeof(message_send), 0);
                    printf("Paridade:  %d", t);
                }
                if (message_recv.type == 1) //LS
                {
                    char ls[100][100];
                    char *ls_full;
                    int tam_strings = 0; 
                    int tam_strings_aux = 0; 
                    int tam_ctrl = 0;
                    int n_msgs = 0;
                    int n_msgs_ctr = 0;
                    int i = 0;
                    int z = 0;
                    struct timeval  tv1, tv2;
                    double tDecorrido;
                    unsigned char data[15];
                    char cwd[PATH_MAX];
                    DIR *dp;
                    struct dirent *ep;     
                    dp = opendir (getcwd(cwd, 100));
                    if (dp != NULL){
                        while (ep = readdir (dp))
                            if (ep->d_type == 4){
                                tam_strings += (strlen(ep->d_name)+2);
                                strcat(ep->d_name , "/");
                                strcpy(ls[i], ep->d_name);
                                i++;
                            }
                            else{
                                tam_strings += (strlen(ep->d_name)+1);
                                strcpy(ls[i], ep->d_name);
                                i++;
                            }
                        (void) closedir (dp);
                        ls_full = (char *)malloc(tam_strings * sizeof(char));
                        for (int k = 0; k < i; k++)
                        {
                            strcat(ls_full , ls[k]);
                            strcat(ls_full , "|");
                        }
                        n_msgs = tam_strings / 15;
                        tam_ctrl = tam_strings;
                        tam_strings_aux = tam_strings;
                        n_msgs_ctr = n_msgs_ctr;
                        printf("STRING: %s\n", ls_full);
                        printf("TAMANHO DA STRING: %d\n", tam_strings);
                        printf("NUMERO DE MSG: %d\n", n_msgs);
                        for (z = 0; z <= n_msgs; z++){
                            if (tam_strings >= 15){
                                for (int i = 0; i < 15; i++)
                                    data[i] = NULL;
                                for (int i = 0; i < 15; i++){
                                    data[i] = ls_full[tam_strings_aux - tam_ctrl];
                                    tam_ctrl--;
                                }
                                setMessage(&message_send, '~' , 15, z, 11, data); 
                                printf("Z:::: %d", z);
                                printMsg(&message_send); //ENVIAR MENSAGEM
                                jump:
                                if (send(socket, &message_send, sizeof(message_send), 0) == -1)            {
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
                                            recv(socket, &message_recv, sizeof(message_recv), 0);
                                            recv(socket, &message_recv, sizeof(message_recv), 0);
                                            if(&message_recv && message_recv.marker == '~'){ 
                                                // printMsg(&message_recv);
                                                setControleServidor();
                                                if(message_recv.type == 8){
                                                    break;
                                                }
                                                if(message_recv.type == 9){
                                                    printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                                                    goto jump;
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
                                n_msgs_ctr--;
                                tam_strings -= 15;
                            }
                            else
                            {
                                for (int i = 0; i < 15; i++)
                                    data[i] = NULL;
                                for (int i = 0; i < tam_strings; i++){
                                    data[i] = ls_full[tam_strings_aux - tam_ctrl];
                                    tam_ctrl--;
                                }
                                setMessage(&message_send, '~' , 15, z, 11, data); 
                                printMsg(&message_send); //ENVIAR MENSAGEM
                                jump2:
                                if (send(socket, &message_send, sizeof(message_send), 0) == -1)            {
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
                                            recv(socket, &message_recv, sizeof(message_recv), 0);
                                            recv(socket, &message_recv, sizeof(message_recv), 0);
                                            if(&message_recv && message_recv.marker == '~'){ 
                                                printMsg(&message_recv);
                                                setControleServidor();
                                                if(message_recv.type == 8){
                                                    printf("Recebeu ACK do CD \n \n \n");
                                                    break;
                                                }
                                                if(message_recv.type == 9){
                                                    printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                                                    goto jump;
                                                }
                                            }
                                        gettimeofday(&tv2, NULL);
                                        tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
                                        if (tDecorrido > 2){
                                            printf("Time Out \n \n \n");
                                            goto jump2;
                                        }
                                    }
                                }
                                n_msgs_ctr--;
                                tam_strings -= 15;
                            }
                        }
                        setMessage(&message_send, '~' , 15, z, 13, data); 
                        printMsg(&message_send); //ENVIAR MENSAGEM
                        jump3:
                        if (send(socket, &message_send, sizeof(message_send), 0) == -1)            {
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
                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                    if(&message_recv && message_recv.marker == '~'){ 
                                        printMsg(&message_recv);
                                        setControleServidor();
                                        if(message_recv.type == 8){
                                            printf("Recebeu ACK do LS \n \n \n");
                                            goto origem;
                                        }
                                        if(message_recv.type == 9){
                                            printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                                            goto jump;
                                        }
                                    }
                                gettimeofday(&tv2, NULL);
                                tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
                                if (tDecorrido > 2){
                                    printf("Time Out \n \n \n");
                                    goto jump3;
                                }
                            }
                        }
                    }
                    else
                        perror ("Não foi possível realizar essa ação! \n");
                }
            }

        }
    }
}  