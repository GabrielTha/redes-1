#include "infos.h"
#include "message.h"
#include "functions.c"
#include "rawSocket.c"

int main(void) {
    int socket = ConexaoRawSocket("lo");
    Message message_recv;
    Message message_send;
    setupterm(NULL, STDOUT_FILENO, NULL);


    while(1){
        recv(socket, &message_recv, sizeof(message_recv), 0);
        if(&message_recv && message_recv.marker == '~'){
            // printf("- \n");
            printMsg(&message_recv);
            int t = checkParity(&message_recv);
            if(checkParity(&message_recv) == 0){
                //NACK
            }
            
            printf("Paridade:  %d", t);
            // setMessage(&message_send, '!', 1, 1, 2, 1);  
            // send(socket, &message_send, sizeof(message_send), 0);
            // checkParity(&message);
            // printf(message.marker);
        }

    }
        
}  