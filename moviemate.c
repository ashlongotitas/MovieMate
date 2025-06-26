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

        // --- Inicializar campos de usuario ---
        nuevoShow->user_rating = 0;      // Sin calificacion al inicio
        nuevoShow->comments = list_create(); // Creamos una lista vacia para futuros comentarios
        nuevoShow->is_favorite = 0;    // No es favorito al inicio

        // Insertar en el mapa usando el titulo como clave
        map_insert(showMap, nuevoShow->title, nuevoShow);
        contador++;
        
        // Liberamos la memoria de la linea copiada
        free(lineaCopiada);
    }

    fclose(archivo);
    printf("%d shows cargados correctamente.\n", contador);
    presioneTeclaParaContinuar(); 
}


void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void esperarEnter() {
    printf("\nPresiona ENTER para continuar...");
    limpiarBuffer();
}

int main() {
    MapshowMap = map_create(is_equal_str, lower_case_str);
    cargarShows(showMap);

    // menu principal
    // mostrarMenuPrincipal(showMap);

    printf("Bienvenido a MovieMate.\n");

    // liberar la memoria
    map_destroy(showMap);

    return 0;
}