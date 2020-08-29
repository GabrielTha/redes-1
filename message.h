#include <infos.h>
 
//message class
typedef struct {
    //parameters
    bitset<BEGIN_S> begin;
    bitset<SIZE_S> size;
    bitset<SEQUENCE_S> sequence;
    bitset<TYPE_S> type;
    bitset<PARITY_S> parity;
    vector<BYTE> data;
 
    //methods
    void check_parity();
}TClasse; 