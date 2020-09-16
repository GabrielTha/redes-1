#include <stdlib.h>

typedef struct linkedList
{
    char value[15];
    struct linkedList *next;
} EList;

/*
 * Função para iniciar uma nova lista
 * 
 * return NULL: lista vazia
*/
EList *newList()
{
    return NULL;
}

/*
 * Função para testar se a posição da lista está vazia
 * 
 * param element_list: referencia para um ponteiro de um elemento da lista
 * 
 * return true: se está vazia
 * return false: se não está vazia
*/
Bool listIsEmpty(EList **element_list)
{
    if (*element_list == NULL)
        return true;
    
    return false;
}

/*
 * Função para adicionar um elemento no inicio da lista
 * 
 * param element_list: referencia para um ponteiro de um elemento da lista
 * param value: valor a ser adicionado na lista
 * 
 * return true: se conseguiu adicionar
 * return false: se não conseguiu adicionar
*/
Bool addFirstList(EList **element_list, int value)
{
    EList *new = (EList *) malloc(sizeof(EList));

    if (listIsEmpty(&new))
        return false;
    
    new -> value = value;

    if (listIsEmpty(element_list))
    {
        new -> next = NULL;
        *element_list = new;

        return true;
    }

    new -> next = *element_list;

    *element_list = new;

    return true;
}

/*
 * Função para adicionar um elemento no fim da lista
 * 
 * param element_list: referencia para um ponteiro de um elemento da lista
 * param value: valor a ser adicionado na lista
 * 
 * return true: se conseguiu adicionar
 * return false: se não conseguiu adicionar
*/
Bool addLastList(EList **element_list, int value)
{
    if (listIsEmpty(element_list))
        return addFirstList(element_list, value);
    
    EList *new = (EList *) malloc(sizeof(EList));

    if (listIsEmpty(&new))
        return false;
    
    new -> value = value;
    new -> next = NULL;

    EList *aux = *element_list;
    for (aux; aux -> next != NULL; aux = aux -> next);
    
    aux -> next = new;

    return true;
}

/*
 * Função para adicionar um elemento na posição desejada da lista
 * 
 * param element_list: referencia para um ponteiro de um elemento da lista
 * param value: valor a ser adicionado na lista
 * param posX: posição a ser adicionado o elemento na lista
 * 
 * return true: se conseguiu adicionar
 * return false: se não conseguiu adicionar
*/
Bool addIndexList(EList **element_list, int value, unsigned int posX)
{
    if (listIsEmpty(element_list) || posX == 0)
        return addFirstList(element_list, value);
    
    EList *new = (EList *) malloc(sizeof(EList));

    if (listIsEmpty(&new))
        return false;

    EList *current = *element_list;
    for (current; current -> next != NULL && posX != 1; current = current -> next)
        posX -= 1;

    new -> value = value;
    new -> next = current -> next;

    current -> next = new;

    return true;
}

/*
 * Função para remover o primeiro elemento da lista
 * 
 * param element_list: referencia para um ponteiro de um elemento da lista
 * 
 * return true: se conseguiu remover
 * return false: se não conseguiu remover
*/
Bool delFirstList(EList **element_list)
{
    if (listIsEmpty(element_list))
        return false;
    
    EList *aux = *element_list;

    *element_list = aux -> next;

    free(aux);

    return true;
}

/*
 * Função para remover o último elemento da lista
 * 
 * param element_list: referencia para um ponteiro de um elemento da lista
 * 
 * return true: se conseguiu remover
 * return false: se não conseguiu remover
*/
Bool delLastList(EList **element_list)
{
    if (listIsEmpty(element_list))
        return false;
    
    EList *current = *element_list;
    EList *previous = NULL;
    for (current; current -> next != NULL; current = current -> next)
        previous = current;

    if (current == *element_list)
        *element_list = newList();
    else
        previous -> next = current -> next;

    free(current);

    return true;
}

/*
 * Função para remover um elemento qualquer da lista
 * 
 * param element_list: referencia para um ponteiro de um elemento da lista
 * param posX: posição a ser removida
 * 
 * return true: se conseguiu remover
 * return false: se não conseguiu remover
*/
Bool delIndexList(EList **element_list, unsigned int posX)
{
    if (listIsEmpty(element_list))
        return false;
    
    if (posX == 0)
        return delFirstList(element_list);

    EList *current = *element_list;
    EList *previous = NULL;
    for (current; current != NULL && posX != 0; current = current -> next)
    {
        previous = current;
        posX -= 1;
    }

    if (current == NULL)
        return false;

    previous -> next = current -> next;

    free(current);

    return true;
}

/*
 * Função para remover todos os elementos da lista
 * 
 * param element_list: referencia para um ponteiro de um elemento da lista
 * 
 * return qty_removed: quantidade de elementos removidos
*/
unsigned int delAllList(EList **element_List)
{
    unsigned int qty_removed = 0;

    EList *aux = *element_List;
    while (*element_List != NULL)
    {
        *element_List = (*element_List) -> next;

        free(aux);

        aux = *element_List;

        qty_removed += 1;
    }
    
    return qty_removed;
}

/*
 * Função para obter quantos elementos existem na lista
 * 
 * param element_list: referencia para um ponteiro de um elemento da lista
 * 
 * return len: tamanho da lista
*/
unsigned int lenList(EList **element_list)
{
    unsigned int len = 0;

    for (EList *aux = *element_list; aux != NULL; aux = aux -> next)
        len += 1;
    
    return len;
}

/*
 * Função para se obter um elemento da lista
 * 
 * param element_list: referencia para um ponteiro de um elemento da lista
 * 
 * return Elist *: se encontrou o elemento
 * return NULL: se não encontrou o elemento
*/
EList *getElementList(EList **element_list, unsigned int posX)
{    
    EList *aux = *element_list;
    for (aux; aux != NULL && posX != 0; aux = aux -> next)
        posX -= 1;
    
    return aux;
}