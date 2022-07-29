#include <stdbool.h>
#include <stdlib.h>
#include "llist.h"

//......................Primitivas de la lista.................

nodo_t* create_node(void* value){
	nodo_t* nodo = malloc(sizeof(nodo_t)); 
	if (!nodo) return NULL; 
	nodo->value = value;
	nodo->next = NULL; 
	return nodo; 
}
lista_t* create_list(){
	lista_t* lista = malloc(sizeof(lista_t));
	if(!lista)	return NULL;
	lista->first = NULL;
	lista->last = NULL;
	lista->length = 0;
	return lista;
}

size_t length_list(const lista_t *lista){
	return lista->length;
}
bool empty_list(const lista_t *lista){
	return length_list(lista) == 0;
}
bool insert_first(lista_t *lista, void *value){
	nodo_t* nodo = create_node(value);
	if(!nodo)	return false;	
	if(empty_list(lista))	lista->last = nodo;
	nodo->next = lista->first;
	lista->first = nodo;
	lista->length ++;
	return true;
}
bool insert_last(lista_t *lista, void *value){
	nodo_t* nodo = create_node(value);
	if(!nodo) return false;
	if(empty_list(lista))	lista->first = nodo;
	else lista->last->next = nodo;
	lista->last = nodo;	
	lista->length ++;
	return true;
}

void *see_element_in_position(lista_t *lista, size_t position){
	// Chequear si es position >= o >
	if(!lista || position > lista->length) {
		return NULL;
	}
	if(position == lista->length){
		return see_last_element(lista);
	}
	size_t counter = 0;
	nodo_t *nodo = lista->first;
	while(counter < position){
		if (!nodo){
			return NULL;
		}
		nodo = nodo->next;
		counter++;
	}
	return nodo->value;
	
}

void *delete_first(lista_t *lista){
	if(empty_list(lista)) return NULL;
	nodo_t* aux = lista->first;
	void* value = lista->first->value;
	if(length_list(lista) == 1){
		lista->first = NULL;
		lista->last = NULL;
	}
	else lista->first = lista->first->next;
	free(aux);
	lista->length --;
	return value;
}
void *see_first_element(const lista_t *lista){
	if(empty_list(lista))	return NULL;
	return lista->first->value;              
}
void *see_last_element(const lista_t *lista){
	if(empty_list(lista))	return NULL;
	return lista->last->value;
}
void destroy_list(lista_t *lista){
    if(!lista ) return;
    nodo_t* nodo_i = lista->first;
    nodo_t* nodo_aux = NULL;
    while (nodo_i) {
        nodo_aux = nodo_i;
        nodo_i = nodo_i->next;
        free(nodo_aux);
    }
    free(lista);
    return;
}