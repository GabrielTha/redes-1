#include "infos.h"
#include "message.h"
#include "functions.c"
#include "rawSocket.c"

int main(void) {
    char command_line[100];
    char clearBuf[256]; //JG:
    int socket = ConexaoRawSocket("lo");
    Message message;
    char c = '0';
    setupterm(NULL, STDOUT_FILENO, NULL);
    system("clear"); 

    while(1){
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
            system("clear"); 
            showHelp();
        }
        else if(strcmp(command[0], "testar") == 0){
            setMessage(&message, '~', 1, 1, 2, '~');            
            
            if (send(socket, &message, sizeof(message), 0) == -1)            {
                printf("Deu ruim o envio \n");
                printf("Erro: %s \n", strerror(errno));
            }
            else
            {
                printf("Mensagem enviada com sucesso! \n \n");
            }
            
            
            
            // system("clear"); 
            // break;
        }
        else if(strcmp(command[0], "lls") == 0){
            // printf("%s\n", getcwd(s, 100)); 
            DIR *dp;
            struct dirent *ep;     
            dp = opendir (getcwd(s, 100));

            if (dp != NULL)
            {
                while (ep = readdir (dp))
                puts (ep->d_name);

                (void) closedir (dp);
            }
            else
                perror ("Não foi possível abrir o");

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