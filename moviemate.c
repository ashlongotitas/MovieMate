#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tdas/list.h"
#include "tdas/map.h"
#include "tdas/heap.h"
#include "tdas/extra.h"

#define MAX_FAVORITOS 100
#define MAX_COMENTARIOS 100
#define MAX_HISTORIAL 200
#define TAMANO_TEXTO 200
#define MAX_USUARIO 50

// Estructura para un comentario
typedef struct {
    char *text;
} Comment;

// Estructura para una película o serie (actualizada)
typedef struct {
    char *id;
    char *type;
    char *title;
    char *director;
    char *cast;
    char *country;
    int release_year;   
    char *rating;
    char *duration;
    char *genres; // "listed_in" del CSV
    char *description;

    // Campos para la interacción del usuario
    int user_rating;   // Calificación del 1 al 5 (0 si no está calificada)
    List *comments;    // Lista de comentarios (del tipo Comment)
    int is_favorite; // 1 si es favorita, 0 si no
} Show;

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void esperarEnter() {
    printf("\nPresiona ENTER para continuar...");
    limpiarBuffer();
}

// Funcion para comparar si dos claves (strings) son iguales
int is_equal_str(void *key1, void *key2) {
    if (strcmp((char *)key1, (char *)key2) == 0) {
        return 1;
    }
    return 0;
}

// Funcion para convertir un string a minusculas
void lower_case_str(char *str) {
    if (str == NULL) return;
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

const char *get_csv_field(char **linea_ptr, int indice) {
    char *linea = *linea_ptr;
    if (linea == NULL || *linea == '\0' || *linea == '\n') {
        return NULL;
    }

    char *campo_buffer = (char*)malloc(2048 * sizeof(char));
    if (campo_buffer == NULL) return NULL;

    char *ptr_buffer = campo_buffer;
    int en_comillas = 0;

    if (*linea == '\"') {
        en_comillas = 1;
        linea++; // Saltar la comilla inicial
    }

    while (*linea != '\0') {
        // Si estamos dentro de comillas
        if (en_comillas) {
            // Si encontramos una comilla, puede ser el final o una comilla doble
            if (*linea == '\"') {
                if (*(linea + 1) == '\"') { // Es una comilla doble "", la tratamos como un solo caracter "
                    *ptr_buffer++ = '\"';
                    linea += 2;
                } else { // Es la comilla de cierre
                    en_comillas = 0;
                    linea++; // Moverse mas alla de la comilla de cierre
                }
            } else {
                *ptr_buffer++ = *linea++;
            }
        } else { // Si no estamos en comillas
            // Si encontramos una coma, es el final del campo
            if (*linea == ',') {
                linea++; // Moverse mas alla de la coma para la proxima llamada
                break;
            }
            // Si es un salto de linea, es el final de la linea
            if (*linea == '\n' || *linea == '\r') {
                *linea = '\0'; // Terminamos la linea aqui
                break;
            }
            *ptr_buffer++ = *linea++;
        }
    }

    *ptr_buffer = '\0';
    *linea_ptr = linea; // Actualizamos el puntero de la linea original para la proxima llamada
    return campo_buffer;
}

void cargarShows(Map *showMap) {
    FILE *archivo = fopen("shows.csv", "r");
    if (archivo == NULL) {
        printf("Error: No se pudo encontrar el archivo 'shows.csv'.\n");
        printf("Asegurate de que el archivo este en la misma carpeta que el ejecutable.\n");
        return;
    }

    char lineaBuffer[4096];
    fgets(lineaBuffer, sizeof(lineaBuffer), archivo); // Omitir cabecera

    printf("Cargando peliculas y series...\n");
    int contador = 0;

    while (fgets(lineaBuffer, sizeof(lineaBuffer), archivo)) {
        
        char *linea_ptr = lineaBuffer;

        Show *nuevoShow = (Show *)malloc(sizeof(Show));
        if (nuevoShow == NULL) continue;
        
        // Pasamos el puntero a la linea y un indice (aunque el indice ya no se usa, lo mantenemos por si acaso)
        nuevoShow->id = (char*)get_csv_field(&linea_ptr, 0);
        nuevoShow->type = (char*)get_csv_field(&linea_ptr, 1);
        nuevoShow->title = (char*)get_csv_field(&linea_ptr, 2);
        nuevoShow->director = (char*)get_csv_field(&linea_ptr, 3);
        nuevoShow->cast = (char*)get_csv_field(&linea_ptr, 4);
        nuevoShow->country = (char*)get_csv_field(&linea_ptr, 5);
        
        free((void*)get_csv_field(&linea_ptr, 6)); // Saltamos y liberamos la memoria de date_added
        
        char* yearStr = (char*)get_csv_field(&linea_ptr, 7);
        nuevoShow->release_year = (yearStr != NULL && strlen(yearStr) > 0) ? atoi(yearStr) : 0;
        free(yearStr);
        
        nuevoShow->rating = (char*)get_csv_field(&linea_ptr, 8);
        nuevoShow->duration = (char*)get_csv_field(&linea_ptr, 9);
        nuevoShow->genres = (char*)get_csv_field(&linea_ptr, 10);
        nuevoShow->description = (char*)get_csv_field(&linea_ptr, 11);

        nuevoShow->user_rating = 0;
        nuevoShow->comments = list_create();
        nuevoShow->is_favorite = 0;
        
        char *tituloClave = strdup(nuevoShow->title);
        if (tituloClave != NULL) {
            lower_case_str(tituloClave);
            map_insert(showMap, tituloClave, nuevoShow);
        }
        contador++;
    }

    fclose(archivo);
    printf("%d shows cargados correctamente.\n", contador);
    presioneTeclaParaContinuar();
}

// --- Funcion auxiliar para mostrar los detalles de un Show 
void mostrarDetallesShow(Show *show) {
    printf("----------------------------------------\n");
    printf("Titulo: %s\n", show->title);
    printf("Tipo: %s\n", show->type);
    printf("Ano de lanzamiento: %d\n", show->release_year);
    printf("Director: %s\n", show->director);
    printf("Elenco: %s\n", show->cast);
    printf("Generos: %s\n", show->genres);
    printf("Descripcion: %s\n", show->description);
    printf("----------------------------------------\n");
}

// Funcion para agregar un show a la lista de favoritos
void agregarAFavoritos(Show *show, List *favoritesList) {
    // Verificamos si ya es un favorito usando nuestro flag
    if (show->is_favorite == 1) {
        printf("\n'%s' ya esta en tu lista de favoritos.\n", show->title);
        return;
    }

    // Si no lo es, lo marcamos y lo agregamos a la lista
    show->is_favorite = 1;
    list_pushBack(favoritesList, show); // Usamos la funcion de tu list.h

    printf("\n'%s' ha sido agregado a tus favoritos!\n", show->title);
}

void buscarShowPorTitulo(Map *showMap, List *favoritesList) {
    char tituloBuscado[200];
    printf("Ingresa el titulo de la pelicula o serie que deseas buscar: ");
    
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Limpiar buffer
    
    fgets(tituloBuscado, sizeof(tituloBuscado), stdin);
    tituloBuscado[strcspn(tituloBuscado, "\n")] = 0;

    lower_case_str(tituloBuscado); 

    MapPair *parEncontrado = map_search(showMap, tituloBuscado);

    if (parEncontrado != NULL) {
        Show *showEncontrado = (Show *)parEncontrado->value;
        printf("\nShow encontrado:\n");
        mostrarDetallesShow(showEncontrado);

        // --- SUBMENU DE ACCIONES ---
        int opcionDetalle = -1;
        printf("Que deseas hacer?\n");
        printf("1. Agregar a Favoritos\n");
        printf("2. Calificar o Comentar (no implementado aun)\n");
        printf("3. Volver\n");
        printf("Selecciona una opcion: ");
        scanf("%d", &opcionDetalle);

        switch (opcionDetalle) {
            case 1:
                // Llamamos a la funcion para agregar a favoritos
                agregarAFavoritos(showEncontrado, favoritesList);
                break;
            case 2:
                printf("Funcion no implementada aun.\n");
                break;
            case 3:
                break; // Simplemente vuelve
            default:
                printf("Opcion no valida.\n");
        }
        
    } else {
        printf("\nLo sentimos, no se encontro ninguna pelicula o serie con ese titulo.\n");
    }
}



// Funcion para mostrar la lista de favoritos
void mostrarFavoritos(List *favoritesList) {
    printf("\n--- TUS FAVORITOS ---\n");

    // Nos posicionamos al inicio de la lista
    Show *primerShow = list_first(favoritesList);

    if (primerShow == NULL) {
        printf("Aun no has agregado nada a tus favoritos.\n");
    } else {
        int contador = 1;
        Show *showActual = primerShow;
        while (showActual != NULL) {
            printf("%d. %s\n", contador, showActual->title);
            // Avanzamos al siguiente elemento de la lista
            showActual = list_next(favoritesList);
            contador++;
        }
    }
    printf("----------------------\n");
}

void mostrarMenuPrincipal() {
    printf("\n====== MovieMate ======\n");
    printf("Bienvenido");
    printf("Menu Principal\n");
    printf("1. Buscar Pelicula/Serie\n");
    printf("2. Ver Comentarios Realizados\n");
    printf("3. Favoritos\n");
    printf("4. Historial de Actividad\n");
    printf("5. Recomendaciones\n");
    printf("6. Configuración\n");
    printf("0. Salir\n");
}

int main() {
    Map *showMap = map_create(is_equal_str);
    
    // --- CREAMOS LA LISTA DE FAVORITOS ---
    List *favoritesList = list_create();

    cargarShows(showMap);

    int opcion = -1;
    while (opcion != 0) {
        mostrarMenuPrincipal();
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                buscarShowPorTitulo(showMap, favoritesList);
                break;
            case 2:
                printf("Funcion no implementada aun.\n");
                break;
            case 3:
                // Llamamos a la nueva funcion para mostrar favoritos
                mostrarFavoritos(favoritesList);
                break;
            // opciones aun no implementadas.
        }
    }

    map_clean(showMap);
    list_clean(favoritesList); // Asumiendo que list_clean libera la lista
    // (O la funcion que corresponda para destruir la lista)

    return 0;
}