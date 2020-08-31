#include "infos.h"
#include "functions.c"
#include "rawSocket.c"

int main(void) {
    char command_line[100];
    int socket = ConexaoRawSocket("lo");
    char message[] = "teste";


    while(1){
        showMenu();
        inicio:
        //Read and separe user 
        scanf("%[^\n]",command_line);
        int i = 0;
        const char s[2] = " ";
        char *token;
        char command[5][100];
        token = strtok(command_line, s);
        while( token != NULL ) {
            strcpy(command[i], token);
            token = strtok(NULL, s);
            i++;
        }  
        if (strcmp(command[0], "sair") == 0){
            printf("Entrou \n");
            break;
        }
        else if(strcmp(command[0], "testar") == 0){
            if (send(socket, &message, sizeof(message), 0) == -1)
            {
                printf("Deu ruim o envio \n");
                printf("Erro: %s \n", strerror(errno));
            }
            else
            {
                printf("Deu boa o envio \n");
                printf("Erro: %s \n", strerror(errno));
            }
            
            
            break;
        }
        else if(strcmp(command[0], "lls") == 0){
            printf("%s\n", getcwd(s, 100)); 
            break;
        }
        else if(strcmp(command[0], "lcd") == 0){
            chdir(command[1]); 
        
            printf("%s\n", getcwd(s, 100)); 
            break;
        }
        else{
            printf("Comando inválido!");
            break;
        }
    }
}  