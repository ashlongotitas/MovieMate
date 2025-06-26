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

char favoritos[MAX_FAVORITOS][100];
int cantidadFavoritos = 0;

Comentario comentarios[MAX_COMENTARIOS];
int cantidadComentarios = 0;

char historial[MAX_HISTORIAL][TAMANO_TEXTO];
int cantidadHistorial = 0;

char nombreUsuario[MAX_USUARIO] = "Invitado";

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void esperarEnter() {
    printf("\nPresiona ENTER para continuar...");
    limpiarBuffer();
}

void agregarAHistorial(const char *accion) {
    if (cantidadHistorial < MAX_HISTORIAL) {
        strncpy(historial[cantidadHistorial], accion, TAMANO_TEXTO);
        cantidadHistorial++;
    }
}

void mostrarMenuPrincipal() {
    printf("\n====== MovieMate ======\n");
    printf("Bienvenido, %s\n", nombreUsuario);
    printf("Menú Principal\n");
    printf("1. Buscar Película/Serie\n");
    printf("2. Ver Comentarios Realizados\n");
    printf("3. Favoritos\n");
    printf("4. Historial de Actividad\n");
    printf("5. Recomendaciones\n");
    printf("6. Configuración\n");
    printf("0. Salir\n");
}

void mostrarMenuBusqueda() {
    printf("\n>> Buscar Película/Serie\n");
    printf("Selecciona el tipo de búsqueda:\n");
    printf("1. Buscar por Nombre\n");
    printf("2. Buscar por Género\n");
    printf("3. Buscar por Palabra Clave\n");
    printf("4. Volver al Menú Principal\n");
}

void mostrarVistaDetalleEjemplo() {
    printf("\n=== Resultado Detallado ===\n");
    printf("Título     : Destino Final: lazos de sangre\n");
    printf("Género     : Terror / Suspenso\n");
    printf("Año        : 2025\n");
    printf("Director   : Zach Lipovsky, Adam B. Stein\n");
    printf("Sinopsis   : Atormentada por una violenta pesadilla recurrente, Stefani regresa a casa para salvar a su familia.\n\n");
    printf("Opciones:\n");
    printf("1. Agregar a Favoritos\n");
    printf("2. Calificar y Comentar\n");
    printf("3. Volver a Resultados\n");
}

void agregarAFavoritos(const char *titulo) {
    if (cantidadFavoritos < MAX_FAVORITOS) {
        strncpy(favoritos[cantidadFavoritos], titulo, 100);
        cantidadFavoritos++;

        char accion[TAMANO_TEXTO];
        snprintf(accion, TAMANO_TEXTO, "Agregaste a favoritos: %s", titulo);
        agregarAHistorial(accion);

        printf("Película agregada a favoritos.\n");
    } else {
        printf("No se pueden agregar más favoritos.\n");
    }
}

void mostrarFavoritos() {
    printf("\n--- Favoritos ---\n");
    if (cantidadFavoritos == 0) {
        printf("No hay películas en favoritos.\n");
    } else {
        for (int i = 0; i < cantidadFavoritos; i++) {
            printf("%d. %s\n", i + 1, favoritos[i]);
        }
    }
    esperarEnter();
}

void calificarYComentar(const char *titulo) {
    if (cantidadComentarios < MAX_COMENTARIOS) {
        int calificacion = 0;
        char buffer[10];

        printf("Calificación para '%s' (1-5): ", titulo);
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%d", &calificacion);

        if (calificacion < 1 || calificacion > 5) {
            printf("Calificación inválida.\n");
            return;
        }

        printf("Comentario: ");
        fgets(comentarios[cantidadComentarios].comentario, TAMANO_TEXTO, stdin);
        comentarios[cantidadComentarios].comentario[strcspn(comentarios[cantidadComentarios].comentario, "\n")] = 0;

        comentarios[cantidadComentarios].calificacion = calificacion;
        strncpy(comentarios[cantidadComentarios].titulo, titulo, 100);
        cantidadComentarios++;

        char accion[TAMANO_TEXTO];
        snprintf(accion, TAMANO_TEXTO, "Comentaste '%s': %d estrellas - %s", titulo,
                 calificacion, comentarios[cantidadComentarios - 1].comentario);
        agregarAHistorial(accion);

        printf("Comentario registrado.\n");
    } else {
        printf("No se pueden registrar más comentarios.\n");
    }
}

void mostrarComentarios() {
    printf("\n--- Comentarios Realizados ---\n");
    if (cantidadComentarios == 0) {
        printf("Aún no has realizado comentarios.\n");
    } else {
        for (int i = 0; i < cantidadComentarios; i++) {
            printf("%d. %s - %d estrellas\n   \"%s\"\n", i + 1, comentarios[i].titulo,
                   comentarios[i].calificacion, comentarios[i].comentario);
        }
    }
    esperarEnter();
}

void mostrarHistorial() {
    printf("\n--- Historial de Actividad ---\n");
    if (cantidadHistorial == 0) {
        printf("No hay historial aún.\n");
    } else {
        for (int i = 0; i < cantidadHistorial; i++) {
            printf("%d. %s\n", i + 1, historial[i]);
        }
    }
    esperarEnter();
}

void mostrarRecomendaciones() {
    printf("\n--- Recomendaciones para %s ---\n", nombreUsuario);
    printf("1. The Midnight Library (Drama/Fantasía)\n");
    printf("2. La Cabaña Sangrienta (Terror)\n");
    printf("3. Sueños Rotos (Romance/Drama)\n");
    printf("4. Destino Final: lazos de sangre (¡Ya consultada!)\n");

    agregarAHistorial("Consultaste recomendaciones.");

    esperarEnter();
}

void configurarUsuario() {
    printf("\nConfiguración de Usuario\n");
    printf("Ingresa tu nombre de usuario: ");
    fgets(nombreUsuario, MAX_USUARIO, stdin);
    nombreUsuario[strcspn(nombreUsuario, "\n")] = 0;

    char accion[TAMANO_TEXTO];
    snprintf(accion, TAMANO_TEXTO, "Cambiado nombre de usuario a: %s", nombreUsuario);
    agregarAHistorial(accion);
}

int main() {
    char buffer[10];
    int opcionMenuPrincipal = -1;
    int opcionBusqueda, opcionDetalle;

    while (opcionMenuPrincipal != 0) {
        mostrarMenuPrincipal();
        printf("Selecciona una opción: ");
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%d", &opcionMenuPrincipal);

        switch (opcionMenuPrincipal) {
            case 1: // Buscar
                do {
                    mostrarMenuBusqueda();
                    printf("Selecciona una opción: ");
                    fgets(buffer, sizeof(buffer), stdin);
                    sscanf(buffer, "%d", &opcionBusqueda);

                    if (opcionBusqueda == 4) break;

                    printf("\nBuscando...\n");
                    mostrarVistaDetalleEjemplo();

                    printf("Selecciona opción de detalle: ");
                    fgets(buffer, sizeof(buffer), stdin);
                    sscanf(buffer, "%d", &opcionDetalle);

                    if (opcionDetalle == 3) {
                        printf("Volviendo a resultados de búsqueda...\n");
                        continue;
                    } else if (opcionDetalle == 1) {
                        agregarAFavoritos("Destino Final: lazos de sangre");
                    } else if (opcionDetalle == 2) {
                        calificarYComentar("Destino Final: lazos de sangre");
                    } else {
                        printf("Opción inválida.\n");
                    }

                    esperarEnter();

                } while (opcionBusqueda != 4);
                break;

            case 2:
                mostrarComentarios();
                break;

            case 3:
                mostrarFavoritos();
                break;

            case 4:
                mostrarHistorial();
                break;

            case 5:
                mostrarRecomendaciones();
                break;

            case 6:
                configurarUsuario();
                break;

            case 0:
                printf("Saliendo del programa...\n");
                break;

            default:
                printf("\nOpción inválida, intenta de nuevo.\n");
                esperarEnter();
        }
    }

    return 0;
}
