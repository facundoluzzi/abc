#ifndef LLIST_H
#define LLIST_H
#include <stdbool.h>
#include <stdio.h>

//...................Declaracion de estructuras....................

typedef struct nodo {
    void* value;
    struct nodo* next;
} nodo_t;

typedef struct lista {
    struct nodo* first;
    struct nodo* last;
    size_t length;
} lista_t;

nodo_t* create_node(void* value);

// Crea una lista
// Post: devuelve una lista vacia
lista_t* create_list(void);

// Devuelve verdadero si la lista no tiene elementos enlistados. Si tiene, devuelve false
// Pre: la lista fue creada
bool empty_list(const lista_t *lista);

// Inserta un elemento al principio de la lista. Devuelve falso en caso de error
// Pre: la lista fue creada
// Post: se agregó un elemento a la lista, el elemento insertado es el primero de la lista
bool insert_first(lista_t *lista, void *value);

// Inserta un elemento al final de la lista. Devuelve falso en caso de error
// Pre: la lista fue creada
// Post: se agregó un elemento a la lista, el elemento insertado es el ultimo de la lista
bool insert_last(lista_t *lista, void *value);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se saca el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// tiene un elemento menos, si la lista no estaba vacía.
void *delete_first(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si tiene elementos,
// devuelve el valor del primero, si esta vacía devuelve NULL.
// Pre: la lista fue creada
// Post: se devolvió el primer elemento de la lista, en caso que no esté vacía.
void *see_first_element(const lista_t *lista);

// Obtiene el valor del ultimo elemento de la lista. Si tiene elementos,
// devuelve el valor del ultimo, si esta vacía devuelve NULL.
// Pre: la lista fue creada
// Post: se devolvió el ultimo elemento de la lista, en caso que no esté vacía.
void *see_last_element(const lista_t* lista);

// Obtiene el valor del elemento position de la lista. Si tiene elementos,
// devuelve el valor del elemento position, si esta vacía devuelve NULL.
// Pre: la lista fue creada
// Post: devuelve el elemento position de la lista, en caso que no esté vacía.
void *see_element_in_position(lista_t *lista, size_t position);

// Devuelve el largo de la lista
// Pre: la lista fue creada
// Post: devuelve la cantidad de elementos que hay en la lista
size_t length_list(const lista_t *lista);

// Destruye la lista
// Si se recibe la función destroy_list por parámetro,
// llama a destroy_list para cada uno de los elementos de la lista
// Pre: la lista fue creada, destroy_list es una función capaz de destruir
// los values de la lista, o NULL si no se la utiliza.
// Post: se eliminaron todos los elementos de la lista.
void destroy_list(lista_t *lista);

#endif //LLIST_H