#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
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
    //presioneTeclaParaContinuar();
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

void calificarShow(Show *show) {
    int calificacion = 0;
    printf("\nIngresa tu calificacion para '%s' (1-5): ", show->title);
    scanf("%d", &calificacion);

    if (calificacion >= 1 && calificacion <= 5) {
        show->user_rating = calificacion;
        printf("Calificacion guardada con exito!\n");
    } else {
        printf("Calificacion no valida. Debe ser un numero entre 1 y 5.\n");
    }
}

void comentarShow(Show *show) {
    char textoComentario[1000];
    printf("\nEscribe tu comentario para '%s': ", show->title);

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    fgets(textoComentario, sizeof(textoComentario), stdin);
    textoComentario[strcspn(textoComentario, "\n")] = 0;

    Comment *nuevoComentario = (Comment *)malloc(sizeof(Comment));
    if (nuevoComentario == NULL) {
        printf("Error de memoria. No se pudo guardar el comentario.\n");
        return;
    }
    
    nuevoComentario->text = strdup(textoComentario);
    list_pushBack(show->comments, nuevoComentario);

    printf("Comentario guardado con exito!\n");
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
        printf("2. Calificar\n");
        printf("3. Comentar\n");
        printf("4. Volver\n");
        printf("Selecciona una opcion: ");
        scanf("%d", &opcionDetalle);

        switch (opcionDetalle) {
            case 1:
                // Llamamos a la funcion para agregar a favoritos
                agregarAFavoritos(showEncontrado, favoritesList);
                break;
            case 2:
                // Llamamos a la funcion para calificar
                calificarShow(showEncontrado);
                break;
            case 3:
                comentarShow(showEncontrado);
                break;
            default:
                printf("Opcion no valida.\n");
        }
        
    } else {
        printf("\nLo sentimos, no se encontro ninguna pelicula o serie con ese titulo.\n");
    }
}

// Funcion para mostrar unicamente los comentarios realizados por el usuario
void mostrarComentarios(Map *showMap) {
    printf("\n--- TUS COMENTARIOS REALIZADOS ---\n");
    int comentariosEncontrados = 0;

    // Recorremos todo el mapa para buscar shows con comentarios
    MapPair *par = map_first(showMap);
    while (par != NULL) {
        Show *showActual = (Show *)par->value;
        Comment *comentario = list_first(showActual->comments);

        // Si el show tiene al menos un comentario, lo mostramos
        if (comentario != NULL) {
            if (!comentariosEncontrados) {
                // Solo imprimimos la primera vez que encontramos un comentario
                comentariosEncontrados = 1;
            }
            printf("\nEn '%s':\n", showActual->title);

            // Recorremos y mostramos todos los comentarios de este show
            while (comentario != NULL) {
                printf(" - \"%s\"\n", comentario->text);
                comentario = list_next(showActual->comments);
            }
        }
        par = map_next(showMap);
    }

    if (!comentariosEncontrados) {
        printf("Aun no has escrito ningun comentario.\n");
    }
    printf("----------------------------------\n");
}

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

void mostrarShowsCalificados(Map *showMap) {
    printf("\n--- TU HISTORIAL DE ACTIVIDAD ---\n");
    int encontrado = 0; // Un flag para saber si encontramos algo

    // Usamos map_first y map_next para recorrer todos los elementos del mapa
    MapPair *par = map_first(showMap);
    while (par != NULL) {
        Show *showActual = (Show *)par->value;

        // Verificamos si el show tiene calificacion o comentarios
        if (showActual->user_rating > 0 || list_first(showActual->comments) != NULL) {
            encontrado = 1;
            printf("\n----------------------------------------\n");
            printf("Titulo: %s\n", showActual->title);
            
            // Mostramos la calificacion si existe
            if (showActual->user_rating > 0) {
                printf("Tu calificacion: %d/5\n", showActual->user_rating);
            }

            // Mostramos los comentarios si existen
            Comment *comentario = list_first(showActual->comments);
            if (comentario != NULL) {
                printf("Tus comentarios:\n");
                while (comentario != NULL) {
                    printf(" - \"%s\"\n", comentario->text);
                    comentario = list_next(showActual->comments);
                }
            }
        }
        par = map_next(showMap);
    }

    if (!encontrado) {
        printf("Aun no has calificado o comentado ningun show.\n");
    }
    printf("\n----------------------------------------\n");
}

void recomendarAleatorio(Map *showMap) {
    printf("\n--- RECOMENDACION ALEATORIA ---\n");

    // Contamos el total de shows en el mapa
    int totalShows = 0;
    MapPair *parContador = map_first(showMap);
    while (parContador != NULL) {
        totalShows++;
        parContador = map_next(showMap);
    }

    if (totalShows == 0) {
        printf("No hay shows cargados en la base de datos.\n");
        return;
    }

    // Inicializamos la semilla del generador de numeros aleatorios
    srand(time(NULL));
    int indiceAleatorio = rand() % totalShows;

    // Recorremos el mapa hasta llegar al indice aleatorio
    int i = 0;
    MapPair *par = map_first(showMap);
    while (i < indiceAleatorio) {
        par = map_next(showMap);
        i++;
    }

    // Mostramos la recomendacion
    Show *showRecomendado = (Show *)par->value;
    printf("Te recomendamos ver:\n");
    mostrarDetallesShow(showRecomendado);
}


void mostrarMenuPrincipal() {
    printf("\n====== MovieMate ======\n");
    printf("Menu Principal\n");
    printf("1. Buscar Pelicula/Serie\n");
    printf("2. Ver Comentarios Realizados\n");
    printf("3. Favoritos\n");
    printf("4. Shows Calificados\n");
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
                mostrarComentarios(showMap);
                break;
            case 3:
                mostrarFavoritos(favoritesList);
                break;
            case 4:
                mostrarShowsCalificados(showMap);
                break;
            case 5:
                recomendarAleatorio(showMap);
                break;
            // opciones aun no implementadas.
        }
    }

    map_clean(showMap);
    list_clean(favoritesList); // Asumiendo que list_clean libera la lista
    // (O la funcion que corresponda para destruir la lista)

    return 0;
}