typedef struct node {
    char *val;
    struct node * next;
} node_t;

// node_t * inicia(){
//     node_t * inicio = (node_t *) malloc(sizeof(node_t));
//     inicio->val = 0;
//     inicio->next = NULL;
//     return inicio;
// }

void imprime(node_t * head) {
    node_t * current = head;
    while (current != NULL) {
        printf("%s\n", current->val);
        current = current->next;
    }
}

void insere(node_t * head, char *val) {
    if (head == NULL)
    {
      node_t * new = (node_t *) malloc(sizeof(node_t));
      strcpy(new->val,val);
      new->next = NULL;
      head = new;
      return;
    }
    
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    

    /* now we can add a new variable */
    current->next = (node_t *) malloc(sizeof(node_t));
    current->next->val = val;
    current->next->next = NULL;
}

int deleta_inicio(node_t ** head) {
    int retval = -1;
    node_t * next_node = NULL;

    if (*head == NULL) {
        return -1;
    }

    next_node = (*head)->next;
    retval = (*head)->val;
    free(*head);
    *head = next_node;

    return retval;
}
