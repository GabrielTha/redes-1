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


    while(1){
        // getControle(controle);
        // if (strcmp(controle, "Cliente") == 0){
            recv(socket, &message_recv, sizeof(message_recv), 0);
            recv(socket, &message_recv, sizeof(message_recv), 0);
            if(&message_recv && message_recv.marker == '~'){ 
                setControleServidor();
                int t = checkParity(&message_recv);
                if((message_recv.type == message_send.type)){
                    //Recebendo mensagem que eu mesmo enviei
                }
                if(checkParity(&message_recv) == 0){
                    //NACK
                }
                if (message_recv.type == 0) //CD
                {
                    printMsg(&message_recv);
                    lcd(message_recv.data);
                    setMessage(&message_send, '~' , 0, 0, 8, 0); //ACK
                    send(socket, &message_send, sizeof(message_send), 0);
                    printf("Paridade:  %d", t);
                }
            }

        // }
    }
        
}  