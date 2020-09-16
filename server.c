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
            recv(socket, &message_recv, sizeof(message_recv), 0);
            recv(socket, &message_recv, sizeof(message_recv), 0);
            if(&message_recv && message_recv.marker == '~'){ 
                int t = checkParity(&message_recv);
                if((message_recv.type == message_send.type)){
                    goto origem;
                }
                if(checkParity(&message_recv) == 0 && message_recv.type != 8 && message_recv.type != 9){ //NACK - 1001 (Não envia Nack para Ack/Nack)
                    setMessage(&message_send, '~' , 0, 0, 9, 0);
                    send(socket, &message_send, sizeof(message_send), 0);
                    printf("ENVIOU NACK\n");
                    goto origem;
                }
                if (message_recv.type == 0){ //CD
                    lcd(message_recv.data);
                    setMessage(&message_send, '~' , 0, 0, 8, 0); //ACK
                    send(socket, &message_send, sizeof(message_send), 0);
                    printf("Paridade:  %d", t);
                }
                if (message_recv.type == 1){ //LS
                    char ls[1000][1000];
                    char *ls_full;
                    int tam_strings = 0; 
                    int tam_strings_aux = 0; 
                    int tam_ctrl = 0;
                    int n_msgs = 0;
                    int n_msgs_ctr = 0;
                    int i = 0;
                    int z = 0;
                    errno = 0; 
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
                        if (0 != errno){
                            for (int i = 0; i < 15; i++)
                                data[i] = NULL;
                            data[0] = 1;
                            setMessage(&message_send, '~' , 0, 0, 15, data);
                            send(socket, &message_send, sizeof(message_send), 0);
                            goto origem;
                        }
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
                            for (int i = 0; i < 15; i++)
                                data[i] = NULL;
                            for (int i = 0; i < 15; i++){
                                data[i] = ls_full[tam_strings_aux - tam_ctrl];
                                tam_ctrl--;
                            }
                            if (tam_strings >= 15){
                                setMessage(&message_send, '~' , 15, z, 11, data); 
                                // printf(&message_send.data);
                            }
                            else{
                                setMessage(&message_send, '~' , tam_strings, z, 11, data); 
                                // printf(&message_send.data);
                            }
                            jump:
                            if (send(socket, &message_send, sizeof(message_send), 0) == -1){ //ENVIANDO DADOS - 1011
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
                                    if (tDecorrido > 12){
                                        printf("Time Out \n \n \n");
                                        goto jump;
                                    }
                                }
                            }
                            n_msgs_ctr--;
                            tam_strings -= 15;
                        }
                        setMessage(&message_send, '~' , 0, z, 13, data); 
                        // printMsg(&message_send); 
                        jump3:
                        if (send(socket, &message_send, sizeof(message_send), 0) == -1){ //ENVIAR FIM DE TRANSMISSÃO - 1101
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
                                if (tDecorrido > 12){
                                    printf("Time Out \n \n \n");
                                    goto jump3;
                                }
                            }
                        }
                    }
                    else
                        perror ("Não foi possível realizar essa ação! \n");
                }
                if (message_recv.type == 2){ //VER
                    char dados[15];
                    printf("%s \n", message_recv.data);
                    FILE * stream;
                    stream = fopen(message_recv.data, "r");
                    while(fread(dados, 1, 3, stream) != EOF) {
                        dados[3] = "\0";
                        printf("%c", dados);
                        break;
                    }
                    
                    fclose(stream);
                    // Printing data to check validity
                }
                
            }
    }
}  



//int comando_ver(No *enviar, char *arquivo, unsigned char *seqEnv) {
//	Mensagem mensagem;
//	char dados[15];
//
//
//	FILE* farquivo = fopen(arquivo, "r");
//	if(farquivo == NULL) {
//	    fprintf(stderr, "Erro ao abrir o arquivo.txt.");
//	    return 1;
//	}
//
//	while((fgets(dados), 15, farquivo) != NULL) {
//		gerar_mensagem(&mensagem, *seqEnv, TIPO_CONT_LS, (unsigned char) strlen(dados), dados);
//		inserir_mensagem(enviar, mensagem);
//		*seqEnv += 1;
//	}
//	gerar_mensagem(&mensagem, *seqEnv, TIPO_FIM, (unsigned char) 0, NULL);
//	inserir_mensagem(enviar, mensagem);
//	*seqEnv += 1;
//
//	return fclose(farquivo);
//}