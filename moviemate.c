#include <stdio.h>
#include <string.h>
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

const char *get_csv_field(char *linea, int indice) {
    // Si la linea es NULL, strtok continua en la cadena anterior
    char *token = strtok(linea, ",");
    for (int i = 0; i < indice; i++) {
        token = strtok(NULL, ",");
    }
    // Retorna el campo o NULL si no se encuentra
    return token;
}   

void cargarShows(Map *showMap) {
    FILE *archivo = fopen("shows.csv", "r");
    if (archivo == NULL) {
        printf("Error: No se pudo encontrar el archivo 'shows.csv'.\n");
        printf("Asegurate de que el archivo este en la misma carpeta que el ejecutable.\n");
        return;
    }

    char lineaBuffer[2048]; // Un buffer para leer cada linea
    fgets(lineaBuffer, sizeof(lineaBuffer), archivo); // Omitimos la primera linea (los encabezados)

    printf("Cargando peliculas y series...\n");
    int contador = 0;

    while (fgets(lineaBuffer, sizeof(lineaBuffer), archivo)) {
        // strdup crea una copia de la linea para trabajar sobre ella sin
        // afectar el buffer original. 
        char *lineaCopiada = strdup(lineaBuffer);
        if (lineaCopiada == NULL) continue;

        Show *nuevoShow = (Show *)malloc(sizeof(Show));
        if (nuevoShow == NULL) {
            free(lineaCopiada);
            continue;
        }
        
        // Usamos strdup para asignar memoria propia a cada campo del struct.
        nuevoShow->id = strdup(get_csv_field(lineaCopiada, 0));
        nuevoShow->type = strdup(get_csv_field(NULL, 1));
        nuevoShow->title = strdup(get_csv_field(NULL, 2));
        nuevoShow->director = strdup(get_csv_field(NULL, 3));
        nuevoShow->cast = strdup(get_csv_field(NULL, 4));
        nuevoShow->country = strdup(get_csv_field(NULL, 5));
        
        const char *yearStr = get_csv_field(NULL, 7); 
        nuevoShow->release_year = (yearStr != NULL) ? atoi(yearStr) : 0;
        
        nuevoShow->rating = strdup(get_csv_field(NULL, 8));
        nuevoShow->duration = strdup(get_csv_field(NULL, 9));
        nuevoShow->genres = strdup(get_csv_field(NULL, 10));
        nuevoShow->description = strdup(get_csv_field(NULL, 11));

        // Inicializar campos de usuario 
        nuevoShow->user_rating = 0;      // Sin calificacion al inicio
        nuevoShow->comments = list_create(); // Creamos una lista vacia para futuros comentarios
        nuevoShow->is_favorite = 0;    // No es favorito al inicio
        
        char *tituloClave = strdup(nuevoShow->title);
        if (tituloClave != NULL) {
            lower_case_str(tituloClave); // Convertimos la clave a minusculas
        // Usamos la clave en minusculas para insertar en el mapa
            map_insert(showMap, tituloClave, nuevoShow); 
        }
        contador++;
        
        // Liberamos la memoria de la linea copiada
        free(lineaCopiada);
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


int main() {
    Map *showMap = map_create(is_equal_str);
    cargarShows(showMap);

    int opcion = -1;
    while (opcion != 0) {
        mostrarMenuPrincipal();
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                buscarShowPorTitulo(showMap);
                break;
            case 2:
                printf("Funcion no implementada aun.\n");
                break;
            case 3:
                printf("Funcion no implementada aun.\n");
                break;
            case 4:
                printf("Funcion no implementada aun.\n");
                break;
            case 5:
                printf("Funcion no implementada aun.\n");
                break;
            case 0:
                printf("Saliendo de MovieMate... Adios!\n");
                break;
            default:
                printf("Opcion no valida. Por favor, intenta de nuevo.\n");
        }
        presioneTeclaParaContinuar();
    }

    // liberar la memoria
    map_destroy(showMap);

    return 0;
}