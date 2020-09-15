#include "infos.h"
#include "message.h"
#include "functions.c"
#include "rawSocket.c"

int main(void) {
    char command_line[100];
    char clearBuf[256]; //JG:
    int socket = ConexaoRawSocket("lo");
    Message message_send;
    Message message_recv;
    
    char c = '0';
    setupterm(NULL, STDOUT_FILENO, NULL);

    while(1){
        setControleCliente();
        showMenu();
        fgets(command_line, 100, stdin);
        strtok(command_line, "\n");
        int i = 0;
        char s[2] = " ";
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
        else if(strcmp(command[0], "h") == 0){
            showHelp();
        }
        else if(strcmp(command[0], "cd") == 0){
            cd(&message_send, &message_recv, command[1], socket);      
            // system("clear"); 
        }
        else if(strcmp(command[0], "ls") == 0){
            ls(&message_send, &message_recv, command[1], socket);      
            // system("clear"); 
        }
        else if(strcmp(command[0], "lls") == 0){
            lls();
        }
        else if(strcmp(command[0], "lcd") == 0){
            lcd(command[1]);
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