#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estructura para almacenar la información de una canción 
typedef struct {
    char id[50]; // ID de la canción
    char artists[100]; // Nombre del artista
    char album_name[100]; // Nombre del álbum de la canción
    char track_name[100]; // nombre de la canción
    float tempo; // Tempo de la canción
    char genre[50]; // Género de la canción
}Song;

// Estructura para almacenar la información de la lista de reproducción
typedef struct {
    char name[100];// Nombre de la lista de reproducción
    List *songs;// Lista de canciones
} Playlist;

// Menu principal
void mostrarMenuPrincipal() {
    limpiarPantalla();
    puts("========================================");
    puts("             SPOTIFIND");
    puts("========================================");
    puts("1) Cargar Canciones");
    puts("2) Buscar por género");
    puts("3) Buscar por artista");
    puts("4) Buscar por tempo");
    puts("5) Crear lista de reproducción");
    puts("6) Agregar canción a lista");
    puts("7) Mostrar canciones de una lista");
    puts("8) Salir");
    printf("Seleccione una opcion: ");
}

// Función para mostrar canciones 
void mostrarCanciones(Song *song){
    printf("ID: %s | Artista: %s | Álbum: %s | Canción: %s | Tempo: %.2f | Género: %s\n",
        song->id, song->artists, song->album_name, song->track_name, song->tempo, song->genre);
}

// Función para comparar dos cadenas de caracteres para ver si son iguales 
int is_equal_str(void *key1, void *key2) {
    // convierte las claves a cadenas de caracteres y compara usando strcmp
    // devuelve 1(verdadero) si son iguales, 0(falso) si no lo son 
    return strcmp((char *)key1, (char *)key2) == 0;
}


void cargarCanciones(Map *mapaCanciones, Map *by_genre, Map *by_artist, FILE *archivo) {
    char linea[1024];
    fgets(linea, sizeof(linea), archivo);  // Saltar cabecera

    while (fgets(linea, sizeof(linea), archivo)) {
        char *campos[30];  // Suponemos que hay hasta 30 columnas
        int i = 0;

        // Eliminar el salto de línea al final de la línea
        linea[strcspn(linea, "\r\n")] = 0;

        char *token = strtok(linea, ",");
        while (token && i < 30) {
            campos[i++] = token;
            token = strtok(NULL, ",");
        }

        if (i < 22) continue; // Requiere al menos hasta el campo 21 (índice 21)

        char *id = campos[0];
        if (!id || map_search(mapaCanciones, id)) continue;

        Song *song = malloc(sizeof(Song));
        if (!song) continue;

        // Copiar los campos relevantes
        strcpy(song->id, id);
        strcpy(song->artists, campos[2]);
        strcpy(song->album_name, campos[3]);
        strcpy(song->track_name, campos[4]);
        song->tempo = atof(campos[19]);
        strcpy(song->genre, campos[21]);


        // Insertar en mapa general
        map_insert(mapaCanciones, strdup(song->id), song);

        // Insertar por género
        List *list_genre = (List*)map_search(by_genre, song->genre);
        if (!list_genre) {
            list_genre = list_create();
            map_insert(by_genre, strdup(song->genre), list_genre);
        }
        list_pushBack(list_genre, song);

        // Insertar por artista
        List *list_artist = (List*)map_search(by_artist, song->artists);
        if (!list_artist) {
            list_artist = list_create();
            map_insert(by_artist, strdup(song->artists), list_artist);
        }
        list_pushBack(list_artist, song);
    }

    printf("Canciones cargadas correctamente.\n");
}


// Buscar por género 
void buscar_por_genero(Map *by_genre) {
    char genero[100];
    printf("Ingrese el género que desea buscar: ");
    scanf(" %[^\n]", genero); // Lee una línea completa con espacios

    List *lista = (List *) map_search(by_genre, genero);
    if (!lista || list_size(lista) == 0) {
        printf("No se encontraron canciones para el género \"%s\".\n", genero);
        return;
    }

    printf("Canciones del género \"%s\":\n", genero);
    Song *song = list_first(lista);
    while (song) {
        mostrarCanciones(song);
        song = list_next(lista);
    }
}

void buscar_por_artista(Map *by_artist) {
    char artista[100];
    printf("Ingrese el nombre del artista que desea buscar: ");
    scanf(" %[^\n]", artista);  // Lee una línea completa con espacios

    // Eliminar espacios en blanco al inicio y al final del nombre del artista
    char *trimmed_artist = strtok(artista, "\n");
    // Buscar en el mapa por el nombre del artista
    List *lista = (List *) map_search(by_artist, trimmed_artist);
    if (!lista || list_size(lista) == 0) {
        printf("No se encontraron canciones para el artista \"%s\".\n", artista);
        return;
    }

    // Mostrar las canciones encontradas
    printf("Canciones del artista \"%s\":\n", artista);
    Song *song = list_first(lista);
    while (song) {
        mostrarCanciones(song);
        song = list_next(lista);
    }
}

// Función para buscar canciones según el tempo seleccionado por el usuario
void buscar_por_tempo(Map *mapaCanciones) {
    int opcion;
    // Mostrar las opciones de tempo al usuario
    printf("Selecciona el tipo de tempo:\n");
    printf("1) Lentas (<80 BPM)\n2) Moderadas (80-120 BPM)\n3) Rapidas (>120 BPM)\nOpción: ");
    scanf("%d", &opcion);  // Leer la opción elegida por el usuario
    // Obtener el primer par clave-valor del mapa de canciones
    MapPair *pair = map_first(mapaCanciones);
    // Recorrer todas las canciones del mapa
    while (pair) {
        Song *song = (Song *)pair->value;  // Obtener la canción del par actual
        // Verificar si la canción cumple con el tempo seleccionado
        if ((opcion == 1 && song->tempo < 80) ||
            (opcion == 2 && song->tempo >= 80 && song->tempo <= 120) ||
            (opcion == 3 && song->tempo > 120)) {
            mostrarCanciones(song);  // Mostrar la canción si cumple con el criterio
        }
        // Avanzar al siguiente par del mapa
        pair = map_next(mapaCanciones);
    }
}

// Función para crear una nueva lista de reproducción
void crear_lista(Map *playlists) {
    char nombre[100];
    // Pedir al usuario el nombre de la nueva lista
    printf("Ingrese el nombre de la nueva lista: ");
    scanf(" %[^\n]", nombre);  // Leer una línea completa (incluye espacios)
    // Verificar si ya existe una lista con el mismo nombre
    if (map_search(playlists, nombre) != NULL) {
        printf("Ya existe una lista con ese nombre.\n");
        return;  // Salir de la función si la lista ya existe
    }
    // Crear una nueva lista vacía
    List *nueva_lista = list_create();
    // Insertar la nueva lista en el mapa usando una copia del nombre como clave
    map_insert(playlists, strdup(nombre), nueva_lista);  // strdup duplica la cadena para evitar problemas de memoria
    // Confirmar la creación al usuario
    printf("Lista \"%s\" creada correctamente.\n", nombre);
}

// Función para agregar una canción a una lista de reproducción
void agregar_cancion_a_lista(Map *mapaCanciones, Map *playlists) {
    char id[100], nombre_lista[100];
    // Pedir al usuario el ID de la canción
    printf("Ingrese el ID de la canción: ");
    scanf(" %s", id);
    // Pedir al usuario el nombre de la lista de reproducción
    printf("Ingrese el nombre de la lista de reproducción: ");
    scanf(" %[^\n]", nombre_lista);  // Leer una línea completa (incluye espacios)
    // Buscar la canción en el mapa de canciones usando el ID
    MapPair *cancion_pair = map_search(mapaCanciones, id);
    // Buscar la lista de reproducción en el mapa de listas usando el nombre
    MapPair *lista_pair = map_search(playlists, nombre_lista);
    // Verificar si la canción existe
    if (!cancion_pair) {
        printf("No se encontró la canción con ID %s.\n", id);
        return;  // Salir si la canción no existe
    }
    // Verificar si la lista de reproducción existe
    if (!lista_pair) {
        printf("No se encontró la lista \"%s\".\n", nombre_lista);
        return;  // Salir si la lista no existe
    }
    // Agregar la canción a la lista de reproducción (al final de la lista)
    list_pushBack((List *)lista_pair->value, cancion_pair->value);
    // Confirmar al usuario que la canción fue añadida
    printf("Canción añadida a la lista \"%s\".\n", nombre_lista);
}

// Función para mostrar las canciones de una lista de reproducción
void mostrar_canciones_lista(Map *playlists) {
    char nombre_lista[100];
    // Pedir al usuario el nombre de la lista de reproducción
    printf("Ingrese el nombre de la lista de reproducción: ");
    scanf(" %[^\n]", nombre_lista);  // Leer una línea completa (incluye espacios)
    // Buscar la lista de reproducción en el mapa usando el nombre
    MapPair *pair = map_search(playlists, nombre_lista);
    // Verificar si la lista existe
    if (!pair) {
        printf("No se encontró la lista \"%s\".\n", nombre_lista);
        return;  // Salir si la lista no existe
    }
    // Obtener la lista de canciones asociada a la lista de reproducción
    List *lista = (List *)pair->value; 
    // Obtener la primera canción de la lista
    Song *song = list_first(lista);
    // Recorrer la lista de canciones y mostrarlas una por una
    while (song) {
        mostrarCanciones(song);  // Mostrar los detalles de la canción actual
        song = list_next(lista); // Avanzar a la siguiente canción
    }
}

int main() {
    // Crear los mapas para almacenar canciones por diferentes criterios
    Map *mapaCanciones = map_create(is_equal_str);  // Mapa con todas las canciones por ID
    Map *by_genre = map_create(is_equal_str);       // Mapa para buscar canciones por género
    Map *by_artist = map_create(is_equal_str);      // Mapa para buscar canciones por artista
    Map *playlists = map_create(is_equal_str);      // Mapa para las listas de reproducción
    // Crear una lista para almacenar todas las canciones 
    int opcion;
    // Bucle principal del menú
    do {
        // Mostrar el menú de opciones al usuario
        mostrarMenuPrincipal();
        scanf("%d", &opcion);
        // Ejecutar la opción seleccionada
        switch (opcion) {
            case 1: {
                // Cargar las canciones desde el archivo CSV
                FILE *archivo = fopen("data/song_dataset_.csv", "r");
                if (archivo == NULL) {
                    printf("El archivo no pudo abrirse.\n");
                    break;
                }
                cargarCanciones(mapaCanciones, by_genre, by_artist, archivo);
                fclose(archivo);
                break;
            }
            case 2:
                // Buscar canciones por género
                buscar_por_genero(by_genre);
                break;

            case 3:
                // Buscar canciones por artista
                buscar_por_artista(by_artist);  
                break;

            case 4:
                // Buscar canciones por tempo
                buscar_por_tempo(mapaCanciones);
                break;

            case 5:
                // Crear una nueva lista de reproducción
                crear_lista(playlists);
                break;

            case 6:
                // Agregar una canción a una lista de reproducción
                agregar_cancion_a_lista(mapaCanciones, playlists);
                break;

            case 7:
                // Mostrar las canciones de una lista de reproducción
                mostrar_canciones_lista(playlists);
                break;

            case 8:
                // Salir del programa
                printf("¡Hasta luego!\n");
                break;

            default:
                // Opción no válida
                printf("Opción no válida. Intente nuevamente.\n");
                break;
        }
        // Pausar antes de volver a mostrar el menú (si no es la opción de salir)
        if (opcion != 8) {
            printf("\nPresione ENTER para continuar...");
            getchar(); getchar();  // Esperar dos veces para capturar el ENTER
        }

    } while (opcion != 8);  // Repetir mientras no se elija salir

    return 0;
}