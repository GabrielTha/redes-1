#include "infos.h"
#include "functions.c"

int main(void) {
    char command_line[100];

    while(1){
        showMenu();
        inicio:
        //Read and separe user 
        scanf("%[a-z A-Z]s",command_line);
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
        else if(strcmp(command[0], "lls") == 0){
            printf("lls \n");
        }
        else{
            printf("Comando inválido!");
        }
    }
}  