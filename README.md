
# 🎬 MovieMate – Gestor de Películas y Series en C

## 📌 Descripción

**MovieMate** es una aplicación de escritorio desarrollada en lenguaje C, diseñada para ayudar a los usuarios a gestionar, calificar y recibir recomendaciones personalizadas de películas y series. Ofrece una experiencia interactiva a través de menús simples, permitiendo buscar contenido, dejar comentarios, agregar a favoritos, y descubrir nuevas películas según los gustos del usuario.

---

## ⚙️ Cómo compilar y ejecutar

### ✅ Requisitos

- [Visual Studio Code](https://code.visualstudio.com/)
- Extensión **C/C++**
- Compilador **gcc**
- Archivos `.c` y `.h` en la carpeta `tdas/`
- Archivo CSV `shows.csv` en el directorio raíz (contiene el catálogo de películas y series)

### 💻 Compilación

```bash
gcc tdas/*.c moviemate.c -o moviemate
```

### ▶️ Ejecución

```bash
./moviemate
```

---

## 🧪 Estructura del CSV

```csv
show_id,type,title,director,cast,country,date_added,release_year,rating,duration,listed_in,description
s1,Movie,Inception,Christopher Nolan,Leonardo DiCaprio,USA,August 1, 2010,2010,PG-13,148 min,"Action, Sci-Fi","A mind-bending thriller..."
...
```

---

## 📚 Funciones implementadas

### Lectura y carga de datos

- `void cargarShows(Map *showMap)`  
  Carga el contenido del archivo CSV en un mapa, indexado por título.

- `const char *get_csv_field(char **linea_ptr, int indice)`  
  Extrae campos individuales desde una línea CSV.

---

### Funcionalidades de usuario

- `void buscarShowPorTitulo(Map *showMap, List *favoritesList)`  
  Busca un contenido por su título y permite al usuario interactuar con él.

- `void mostrarDetallesShow(Show *show)`  
  Muestra toda la información disponible de una película o serie.

- `void agregarAFavoritos(Show *show, List *favoritesList)`  
  Agrega el contenido a la lista de favoritos del usuario.

- `void calificarShow(Show *show)`  
  Permite asignar una calificación del 1 al 5.

- `void comentarShow(Show *show)`  
  Permite escribir un comentario libre sobre un contenido.

- `void mostrarComentarios(Map *showMap)`  
  Muestra todos los comentarios realizados por el usuario.

- `void mostrarFavoritos(List *favoritesList)`  
  Muestra la lista actual de favoritos.

- `void mostrarShowsCalificados(Map *showMap)`  
  Muestra el historial completo de calificaciones y comentarios.

---

### Recomendaciones

- `void recomendarAleatorio(Map *showMap)`  
  Recomienda una película o serie al azar desde el catálogo.

- `void recomendarPersonalizado(Map *showMap, List *favoritesList)`  
  Analiza los géneros más frecuentes entre los favoritos y sugiere contenido similar.

---

### Utilidades

- `void limpiarBuffer()`  
  Limpia el buffer de entrada.

- `void esperarEnter()`  
  Pausa el programa hasta que se presione ENTER.

- `void lower_case_str(char *str)`  
  Convierte un string a minúsculas (para comparación).

- `int is_equal_str(void *a, void *b)`  
  Función para comparar cadenas, utilizada por los mapas.

---

## 🧩 Menú Principal

- `1. Buscar Película/Serie` → Búsqueda interactiva por título
- `2. Ver Comentarios Realizados` → Revisión de comentarios personales
- `3. Favoritos` → Lista personalizada de películas/series marcadas
- `4. Shows Calificados` → Historial completo de interacción
- `5. Recomendaciones`  
  - `1. Personalizadas`
  - `2. Aleatorias`
- `0. Salir` → Finaliza el programa

---

## 🧱 TDAs utilizados

- `Map` (Hash Map): para indexar los contenidos por nombre
- `List`: para almacenar favoritos y comentarios
- `Heap` (opcional): considerado para recomendaciones futuras

---

## 👥 Autores

Proyecto desarrollado por estudiantes de la **Escuela de Informática – PUCV**

- **Piero Yany** – Coordinador y desarrollo principal
- **Ashlee Cortés** – Integración de código y pruebas
- **Fernando López** – Diseño de menús y recomendaciones
- **Maximiliano Fargo** – Revisión de estructuras y calidad

Valparaíso, Chile – Junio 2025

---
