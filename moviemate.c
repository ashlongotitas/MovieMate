#include <stdio.h>
#include <string.h>

#define MAX_FAVORITOS 100
#define MAX_COMENTARIOS 100
#define MAX_HISTORIAL 200
#define TAMANO_TEXTO 200
#define MAX_USUARIO 50

typedef struct {
    char titulo[100];
    int calificacion;
    char comentario[TAMANO_TEXTO];
} Comentario;

typedef struct {
    char id;
    int tipo; // 1: Película, 2: Serie
    char titulo[100];
    char director[100];
    char genero[50];
    char cast;
    char pais[50];
    int anio;
} Contenido;

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
