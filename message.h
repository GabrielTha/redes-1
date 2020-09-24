#include "infos.h"
 
typedef struct {
    //Marcador
    unsigned char marker:8;
    //Tamanho
    unsigned char size:4;
    //Sequencializacao
    unsigned char seq:8;
    //Tipo
    unsigned char type:4;
    //Dados
    unsigned char data[15];
    //Paridade
    unsigned char parity:8;
}Message; 