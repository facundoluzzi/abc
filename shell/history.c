#include "defs.h"
#include "linked_list/llist.h"
#include "linked_list/llist.c"
#include "history.h"

static FILE * history_file = NULL;
static char history_file_dir[PRMTLEN] = {0};
static lista_t *history = NULL;
static ssize_t actual = 0;
char ret[1] = {0};

void exit_history(){
    if(history){
        while(!empty_list(history)){
            free(see_first_element(history));
            delete_first(history);
        }
        destroy_list(history);
    }
    if (history_file){
        fclose(history_file);
    }
    return;
}

void initialize_history(){
    // De estar definida la variable de entorno HISTFILE, la misma contendrá la ruta al archivo 
    // con los comandos ejecutados en el pasado. En caso contrario, utilizar como ruta por omisión a ~/.fisop_history.
    if(getenv("HISTFILE")){
        strcpy(history_file_dir, getenv("HISTFILE"));
    }
    else {
        strcpy(history_file_dir, getenv("HOME") );
        strcat(history_file_dir, "/.fisop_history" );
    }
    // Abrimos el archivo para leerlo, y poder escribir al final del mismo ('a')
    history_file = fopen(history_file_dir, "r");
    history = create_list();

    char *line;
    size_t len = 0;

    int t = 0;
    while (getline(&line, &len, history_file) != -1 ){
        insert_first(history, strdup(line));
        t++;
    }
    printf("Cantidad de lineas leidas: %d", t);
    // Nos aseguramos de vaciar la lista y liberar la memoria reservada para cada elemento
    atexit(exit_history);
    return;
}

char *get_with_up_key(){
    if(!history_file || !history->length){
        return ret;
    }
    actual--;
    if(actual < 0){
        actual = 0;
        return ret;
    }
    return see_element_in_position(history, actual);
}

char *get_with_down_key(){
    if(!history_file || !history->length){
        return ret;
    }
    if(actual == history->length){
        return ret;
    }
    actual++;
    return see_element_in_position(history, actual);
}

void push_cmd(char *command){
    if(!history_file || !command){
        return;
    }
    insert_first(history, strdup(command));
    return;
}

int history_cmd(){
    size_t length = length_list(history);
    printf("History CMD con %d de longitud!\n\n", length);

    while(length){
        char *value = see_element_in_position(history, length);
        printf("%d %s", length, value);
        length--;
    }

    return 0;
}