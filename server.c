#include "infos.h"
#include "message.h"
#include "functions.c"
#include "rawSocket.c"

int main(void) {
    int socket = ConexaoRawSocket("lo");
    Message message_recv;
    Message message_send;
    char controle[20];
    unsigned char data[15];
    setupterm(NULL, STDOUT_FILENO, NULL);

    origem:
    while(1){
            recv(socket, &message_recv, sizeof(message_recv), 0);
            recv(socket, &message_recv, sizeof(message_recv), 0);
            if(&message_recv && message_recv.marker == '~'){ 
                int t = checkParity(&message_recv);
                // printMsg(&message_recv);
                // if((message_recv.type == message_send.type)){
                //     goto origem;
                // }
                if(checkParity(&message_recv) == 0 && message_recv.type != 8 && message_recv.type != 9){ //NACK - 1001 (Não envia Nack para Ack/Nack)
                    setMessage(&message_send, '~' , 0, 0, 9, 0);
                    send(socket, &message_send, sizeof(message_send), 0);
                    printf("ENVIOU NACK\n");
                    goto origem;
                }
                if (message_recv.type == 0){ //CD
                    char cwd[PATH_MAX];
                    if (chdir(message_recv.data) == 0){
                        printf("Diretório modificado! \n"); 
                        printf("Diretório atual: %s\n", getcwd(cwd, 100)); 
                        setMessage(&message_send, '~' , 0, 0, 8, 0); //ACK
                        send(socket, &message_send, sizeof(message_send), 0);
                    }
                    else{
                        if (errno == 1){ //Detecta NÃO PERMITIDO
                            for (int i = 0; i < 15; i++)
                                data[i] = NULL;
                            data[0] = '1';
                        }
                        if (errno == 2){ //Detecta NÃO EXISTENTE
                            for (int i = 0; i < 15; i++)
                                data[i] = NULL;
                            data[0] = '2';
                        }
                        setMessage(&message_send, '~' , 1, 0, 15, data);
                        send(socket, &message_send, sizeof(message_send), 0);
                        goto origem;
                        
                    }

                    
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
                    if (dp == NULL){
                        printf("Erro: %s \n", strerror(errno));
                    }
                    // for (size_t i = 0; i < sizeof(ls); i++)
                    //     // strcpy(ls[i], '\0');
                    //     ls[i] = NULL;
                    // for (size_t i = 0; i < sizeof(ls_full); i++)
                    //     ls_full[i] = NULL;
                    //     // strcpy(ls_full[i], '\0');
                    
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
                        if (errno != 0){ //Detecta Erro de Permissão
                            for (int i = 0; i < 15; i++)
                                data[i] = NULL;
                            data[0] = 1;
                            setMessage(&message_send, '~' , 0, 0, 15, data);
                            send(socket, &message_send, sizeof(message_send), 0);
                            goto origem;
                        }
                        ls_full = (char *)malloc(tam_strings * sizeof(char));
                        memset(ls_full,NULL,sizeof(ls_full));
                        for (int k = 0; k < i; k++){
                            strcat(ls_full , ls[k]);
                            strcat(ls_full , "\a");
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
                                while(1){
                                        recv(socket, &message_recv, sizeof(message_recv), 0);
                                        recv(socket, &message_recv, sizeof(message_recv), 0);
                                        if(&message_recv && message_recv.marker == '~'){ 
                                            //setControleServidor();
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
                            while(1){
                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                    if(&message_recv && message_recv.marker == '~'){ 
                                        //setControleServidor();
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
                    FILE *file;
                    char linha[1000];
                    char cnt_linha[1000];
                    char *str_all = (char *) malloc(1 * sizeof(char));
                    int str_size = 1;
                    int aux,line=0,count=0;
                    char * pre_linha;
                    char buf[1000];
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
                    char linha_aux[1000];
                    if((file = fopen(message_recv.data,"r")) == NULL){
                        if (errno == 1){ //Detecta NÃO PERMITIDO
                            for (int i = 0; i < 15; i++)
                                data[i] = NULL;
                            data[0] = '1';
                        }
                        if (errno == 2){ //Detecta NÃO EXISTENTE
                            for (int i = 0; i < 15; i++)
                                data[i] = NULL;
                            data[0] = '3';
                        }
                        setMessage(&message_send, '~' , 1, 0, 15, data);
                        send(socket, &message_send, sizeof(message_send), 0);
                        goto origem;
                    }
                    else
                    {
                        while(fgets(linha,sizeof(linha),file) != NULL){
                            for(aux=0; linha[aux]; aux++) 
                                if(linha[aux]=='\n'){
                                    count++;
                                    sprintf(cnt_linha, "%d", count); 
                                    strcat(cnt_linha, " - ");
                                    strcat(cnt_linha, linha);
                                    strcpy(linha, cnt_linha);
                                    // printf("%d - %s",(strlen(linha)/14), linha);
                                    // printf("---------\n");
                                    linha[(strlen(linha)-1)] = '\0';
                                    strcat(linha, "\a"),
                                    tam_strings = strlen(linha);
                                    n_msgs =  round(tam_strings / 14);
                                    tam_ctrl = tam_strings;
                                    tam_strings_aux = tam_strings;
                                    n_msgs_ctr = n_msgs_ctr;
                                    // printf("STRING: %s\n", linha);
                                    // printf("TAMANHO DA STRING: %d\n", tam_strings);
                                    // printf("NUMERO DE MSG: %d\n", n_msgs);
                                    for (z = 0; z <= n_msgs; z++){
                                        for (int i = 0; i < 15; i++)
                                            data[i] = 0;
                                        for (int i = 0; i < 14; i++){
                                            data[i] = linha[tam_strings_aux - tam_ctrl];
                                            tam_ctrl--;
                                        }
                                        data[15]= '\0';
                                        if (tam_strings >= 15){
                                            setMessage(&message_send, '~' , 15, z, 12, data); 
                                        }
                                        else{
                                            setMessage(&message_send, '~' , tam_strings, z, 12, data); 
                                        }
                                        // printMsg(&message_send);
                                        jump4:
                                        if (send(socket, &message_send, sizeof(message_send), 0) == -1){ //ENVIANDO DADOS - 1011
                                        printf("Erro ao enviar mensagem! \n");
                                            printf("Erro: %s \n", strerror(errno));
                                        }
                                        else
                                        {
                                            gettimeofday(&tv1, NULL);
                                            while(1){
                                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                                    if(&message_recv && message_recv.marker == '~'){ 
                                                        //setControleServidor();
                                                        if(message_recv.type == 8){
                                                            break;
                                                        }
                                                        if(message_recv.type == 9){
                                                            printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                                                            goto jump4;
                                                        }
                                                    }
                                                gettimeofday(&tv2, NULL);
                                                tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
                                                if (tDecorrido > 12){
                                                    printf("Time Out \n \n \n");
                                                    goto jump4;
                                                }
                                            }
                                        }
                                        n_msgs_ctr--;
                                        tam_strings -= 14;
                                    }

                                }	
                        }
                    }	
                    fclose(file);
                    setMessage(&message_send, '~' , 0, z, 13, data); 
                        // printMsg(&message_send); 
                        jump5:
                        if (send(socket, &message_send, sizeof(message_send), 0) == -1){ //ENVIAR FIM DE TRANSMISSÃO - 1101
                            printf("Erro ao enviar mensagem! \n");
                            printf("Erro: %s \n", strerror(errno));
                        }
                        else
                        {
                            gettimeofday(&tv1, NULL);
                            while(1){
                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                    if(&message_recv && message_recv.marker == '~'){ 
                                        //setControleServidor();
                                        if(message_recv.type == 8){
                                            printf("Recebeu ACK do LS \n \n \n");
                                            goto origem;
                                        }
                                        if(message_recv.type == 9){
                                            printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                                            goto jump5;
                                        }
                                    }
                                gettimeofday(&tv2, NULL);
                                tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
                                if (tDecorrido > 12){
                                    printf("Time Out \n \n \n");
                                    goto jump5;
                                }
                            }
                        }

                }
                if (message_recv.type == 3){ //LINHA
                    FILE *file;
                    char linha[1000];
                    char cnt_linha[1000];
                    char *str_all = (char *) malloc(1 * sizeof(char));
                    int str_size = 1;
                    int aux,line=0,count=0;
                    char * pre_linha;
                    char buf[1000];
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
                    int n_linha;
                    int line_ok = 0;
                    struct timeval  tv1, tv2;
                    double tDecorrido;
                    unsigned char data[15];
                    char linha_aux[1000];
                    if((file = fopen(message_recv.data,"r")) == NULL){
                        if (errno == 1){ //Detecta NÃO PERMITIDO
                            for (int i = 0; i < 15; i++)
                                data[i] = NULL;
                            data[0] = '1';
                        }
                        if (errno == 2){ //Detecta NÃO EXISTENTE
                            for (int i = 0; i < 15; i++)
                                data[i] = NULL;
                            data[0] = '3';
                        }
                        setMessage(&message_send, '~' , 1, 0, 15, data);
                        send(socket, &message_send, sizeof(message_send), 0);
                        goto origem;
                    }
                    else
                    {
                        setMessage(&message_send, '~' , 0, 0, 8, 0); //Enviando ACK
                        jump8:
                        send(socket, &message_send, sizeof(message_send), 0);
                        gettimeofday(&tv1, NULL);
                        while(1){
                            recv(socket, &message_recv, sizeof(message_recv), 0);
                            recv(socket, &message_recv, sizeof(message_recv), 0);
                            if(&message_recv && message_recv.marker == '~'){ // Esperando linhas - 1010
                                n_linha = atoi(message_recv.data);
                                if(message_recv.type == 10){
                                    while(fgets(linha,sizeof(linha),file) != NULL){
                                        for(aux=0; linha[aux]; aux++) 
                                            if(linha[aux]=='\n'){
                                                count++;
                                                if (count == n_linha){
                                                    line_ok = 1;
                                                    sprintf(cnt_linha, "%d", count); 
                                                    strcat(cnt_linha, " - ");
                                                    strcat(cnt_linha, linha);
                                                    strcpy(linha, cnt_linha);
                                                    // printf("%d - %s",(strlen(linha)/14), linha);
                                                    // printf("---------\n");
                                                    linha[(strlen(linha)-1)] = '\0';
                                                    strcat(linha, "\a"),
                                                    tam_strings = strlen(linha);
                                                    n_msgs =  round(tam_strings / 14);
                                                    tam_ctrl = tam_strings;
                                                    tam_strings_aux = tam_strings;
                                                    n_msgs_ctr = n_msgs_ctr;
                                                    // printf("STRING: %s\n", linha);
                                                    // printf("TAMANHO DA STRING: %d\n", tam_strings);
                                                    // printf("NUMERO DE MSG: %d\n", n_msgs);
                                                    for (z = 0; z <= n_msgs; z++){
                                                        for (int i = 0; i < 15; i++)
                                                            data[i] = 0;
                                                        for (int i = 0; i < 14; i++){
                                                            data[i] = linha[tam_strings_aux - tam_ctrl];
                                                            tam_ctrl--;
                                                        }
                                                        data[15]= '\0';
                                                        if (tam_strings >= 15){
                                                            setMessage(&message_send, '~' , 15, z, 12, data); 
                                                        }
                                                        else{
                                                            setMessage(&message_send, '~' , tam_strings, z, 12, data); 
                                                        }
                                                        printMsg(&message_send);
                                                        jump9:
                                                        if (send(socket, &message_send, sizeof(message_send), 0) == -1){ //ENVIANDO DADOS - 1011
                                                        printf("Erro ao enviar mensagem! \n");
                                                            printf("Erro: %s \n", strerror(errno));
                                                        }
                                                        else
                                                        {
                                                            gettimeofday(&tv1, NULL);
                                                            while(1){
                                                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                                                    if(&message_recv && message_recv.marker == '~'){ 
                                                                        //setControleServidor();
                                                                        if(message_recv.type == 8){
                                                                            break;
                                                                        }
                                                                        if(message_recv.type == 9){
                                                                            printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                                                                            goto jump9;
                                                                        }
                                                                    }
                                                                gettimeofday(&tv2, NULL);
                                                                tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
                                                                if (tDecorrido > 12){
                                                                    printf("Time Out \n \n \n");
                                                                    goto jump9;
                                                                }
                                                            }
                                                        }
                                                        n_msgs_ctr--;
                                                        tam_strings -= 14;
                                                    }
                                                }
                                            }	
                                    }
                                    if (line_ok == 0){
                                        printf("LINHA INEXISTENTE");
                                        setMessage(&message_send, '~' , 0, z, 12, data);
                                        jump10:
                                        if (send(socket, &message_send, sizeof(message_send), 0) == -1){ //ENVIANDO DADOS - 1011
                                            printf("Erro ao enviar mensagem! \n");
                                            printf("Erro: %s \n", strerror(errno));
                                        }
                                        else{
                                            gettimeofday(&tv1, NULL);
                                            while(1){
                                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                                    if(&message_recv && message_recv.marker == '~'){ 
                                                        if(message_recv.type == 8){
                                                            break;
                                                        }
                                                        if(message_recv.type == 9){
                                                            printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                                                            goto jump10;
                                                        }
                                                    }
                                                gettimeofday(&tv2, NULL);
                                                tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
                                                if (tDecorrido > 12){
                                                    printf("Time Out \n \n \n");
                                                    goto jump10;
                                                }
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                            gettimeofday(&tv2, NULL);
                            tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
                            if (tDecorrido > 12){
                                printf("Time Out \n \n \n");
                                goto jump8;
                            }
                        }
                        
                    }	
                    fclose(file);
                    setMessage(&message_send, '~' , 0, z, 13, data); 
                        // printMsg(&message_send); 
                        jump6:
                        if (send(socket, &message_send, sizeof(message_send), 0) == -1){ //ENVIAR FIM DE TRANSMISSÃO - 1101
                            printf("Erro ao enviar mensagem! \n");
                            printf("Erro: %s \n", strerror(errno));
                        }
                        else
                        {
                            gettimeofday(&tv1, NULL);
                            while(1){
                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                    if(&message_recv && message_recv.marker == '~'){ 
                                        //setControleServidor();
                                        if(message_recv.type == 8){
                                            printf("Recebeu ACK do LS \n \n \n");
                                            goto origem;
                                        }
                                        if(message_recv.type == 9){
                                            printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                                            goto jump6;
                                        }
                                    }
                                gettimeofday(&tv2, NULL);
                                tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
                                if (tDecorrido > 12){
                                    printf("Time Out \n \n \n");
                                    goto jump6;
                                }
                            }
                        }

                }
                if (message_recv.type == 4){ //LINHAS
                    FILE *file;
                    char linha[1000];
                    char cnt_linha[1000];
                    char *str_all = (char *) malloc(1 * sizeof(char));
                    int str_size = 1;
                    int aux,line=0,count=0;
                    char * pre_linha;
                    char buf[1000];
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
                    int n_linha_i;
                    int n_linha_f;
                    int line_ok = 0;
                    struct timeval  tv1, tv2;
                    double tDecorrido;
                    unsigned char data[15];
                    char linha_aux[1000];
                    if((file = fopen(message_recv.data,"r")) == NULL){
                        if (errno == 1){ //Detecta NÃO PERMITIDO
                            for (int i = 0; i < 15; i++)
                                data[i] = NULL;
                            data[0] = '1';
                        }
                        if (errno == 2){ //Detecta NÃO EXISTENTE
                            for (int i = 0; i < 15; i++)
                                data[i] = NULL;
                            data[0] = '3';
                        }
                        setMessage(&message_send, '~' , 1, 0, 15, data);
                        send(socket, &message_send, sizeof(message_send), 0);
                        goto origem;
                    }
                    else
                    {
                        setMessage(&message_send, '~' , 0, 0, 8, 0); //Enviando ACK
                        jump13:
                        send(socket, &message_send, sizeof(message_send), 0);
                        gettimeofday(&tv1, NULL);
                        while(1){
                            recv(socket, &message_recv, sizeof(message_recv), 0);
                            recv(socket, &message_recv, sizeof(message_recv), 0);
                            if(&message_recv && message_recv.marker == '~'){ // Esperando linhas - 1010
                                char s[2] = "|";
                                char *token;
                                char command[5][100];
                                token = strtok(message_recv.data, s);
                                int k = 1;
                                while( token != NULL ) {
                                    if (k == 1)
                                        n_linha_i = atoi(token);
                                    if (k == 2)
                                        n_linha_f = atoi(token);
                                    k++;  
                                    strcpy(command[i], token);
                                    token = strtok(NULL, s);
                                }  
                                printf("%d - ", n_linha_i);
                                printf("%d", n_linha_f);
                                if(message_recv.type == 10){
                                    while(fgets(linha,sizeof(linha),file) != NULL){
                                        for(aux=0; linha[aux]; aux++) 
                                            if(linha[aux]=='\n'){
                                                count++;
                                                if (count >= n_linha_i && count <= n_linha_f){
                                                    line_ok = 1;
                                                    sprintf(cnt_linha, "%d", count); 
                                                    strcat(cnt_linha, " - ");
                                                    strcat(cnt_linha, linha);
                                                    strcpy(linha, cnt_linha);
                                                    // printf("%d - %s",(strlen(linha)/14), linha);
                                                    // printf("---------\n");
                                                    linha[(strlen(linha)-1)] = '\0';
                                                    strcat(linha, "\a"),
                                                    tam_strings = strlen(linha);
                                                    n_msgs =  round(tam_strings / 14);
                                                    tam_ctrl = tam_strings;
                                                    tam_strings_aux = tam_strings;
                                                    n_msgs_ctr = n_msgs_ctr;
                                                    // printf("STRING: %s\n", linha);
                                                    // printf("TAMANHO DA STRING: %d\n", tam_strings);
                                                    // printf("NUMERO DE MSG: %d\n", n_msgs);
                                                    for (z = 0; z <= n_msgs; z++){
                                                        for (int i = 0; i < 15; i++)
                                                            data[i] = 0;
                                                        for (int i = 0; i < 14; i++){
                                                            data[i] = linha[tam_strings_aux - tam_ctrl];
                                                            tam_ctrl--;
                                                        }
                                                        data[15]= '\0';
                                                        if (tam_strings >= 15){
                                                            setMessage(&message_send, '~' , 15, z, 12, data); 
                                                        }
                                                        else{
                                                            setMessage(&message_send, '~' , tam_strings, z, 12, data); 
                                                        }
                                                        // printMsg(&message_send);
                                                        jump15:
                                                        if (send(socket, &message_send, sizeof(message_send), 0) == -1){ //ENVIANDO DADOS - 1011
                                                        printf("Erro ao enviar mensagem! \n");
                                                            printf("Erro: %s \n", strerror(errno));
                                                        }
                                                        else
                                                        {
                                                            gettimeofday(&tv1, NULL);
                                                            while(1){
                                                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                                                    if(&message_recv && message_recv.marker == '~'){ 
                                                                        //setControleServidor();
                                                                        if(message_recv.type == 8){
                                                                            break;
                                                                        }
                                                                        if(message_recv.type == 9){
                                                                            printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                                                                            goto jump15;
                                                                        }
                                                                    }
                                                                gettimeofday(&tv2, NULL);
                                                                tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
                                                                if (tDecorrido > 12){
                                                                    printf("Time Out \n \n \n");
                                                                    goto jump15;
                                                                }
                                                            }
                                                        }
                                                        n_msgs_ctr--;
                                                        tam_strings -= 14;
                                                    }
                                                }
                                            }	
                                    }
                                    if (line_ok == 0){
                                        printf("LINHA INEXISTENTE");
                                        setMessage(&message_send, '~' , 0, z, 12, data);
                                        jump12:
                                        if (send(socket, &message_send, sizeof(message_send), 0) == -1){ //ENVIANDO DADOS - 1011
                                            printf("Erro ao enviar mensagem! \n");
                                            printf("Erro: %s \n", strerror(errno));
                                        }
                                        else{
                                            gettimeofday(&tv1, NULL);
                                            while(1){
                                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                                    if(&message_recv && message_recv.marker == '~'){ 
                                                        if(message_recv.type == 8){
                                                            break;
                                                        }
                                                        if(message_recv.type == 9){
                                                            printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                                                            goto jump12;
                                                        }
                                                    }
                                                gettimeofday(&tv2, NULL);
                                                tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
                                                if (tDecorrido > 12){
                                                    printf("Time Out \n \n \n");
                                                    goto jump12;
                                                }
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                            gettimeofday(&tv2, NULL);
                            tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
                            if (tDecorrido > 12){
                                printf("Time Out \n \n \n");
                                goto jump13;
                            }
                        }
                        
                    }	
                    fclose(file);
                    setMessage(&message_send, '~' , 0, z, 13, data); 
                        // printMsg(&message_send); 
                        jump11:
                        if (send(socket, &message_send, sizeof(message_send), 0) == -1){ //ENVIAR FIM DE TRANSMISSÃO - 1101
                            printf("Erro ao enviar mensagem! \n");
                            printf("Erro: %s \n", strerror(errno));
                        }
                        else
                        {
                            gettimeofday(&tv1, NULL);
                            while(1){
                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                    recv(socket, &message_recv, sizeof(message_recv), 0);
                                    if(&message_recv && message_recv.marker == '~'){ 
                                        //setControleServidor();
                                        if(message_recv.type == 8){
                                            printf("Recebeu ACK do LS \n \n \n");
                                            goto origem;
                                        }
                                        if(message_recv.type == 9){
                                            printf("Recebeu NACK do CD \n Reenviando mensagem!\n \n");
                                            goto jump11;
                                        }
                                    }
                                gettimeofday(&tv2, NULL);
                                tDecorrido = ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
                                if (tDecorrido > 12){
                                    printf("Time Out \n \n \n");
                                    goto jump11;
                                }
                            }
                        }

                }
            }
    }
}  