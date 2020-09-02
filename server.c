#include "infos.h"
#include "message.h"
#include "functions.c"
#include "rawSocket.c"

int main(void) {
    int socket = ConexaoRawSocket("lo");
    Message message;


    while(1){
        recv(socket, &message, sizeof(message), 0);
        if(&message){
            // printf("- \n");
            printMsg(&message);
            checkParity(&message);
            // printf(message.marker);
        }

    }
        
}  