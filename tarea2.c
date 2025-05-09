#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char id[100];
    char track_name[200];   // nombre de la canción
    char album_name[200];   // nombre del álbum
    char artist[200];       // nombre del artista principal
    int popularity;
    int duration_ms;
    int explicit;
    float tempo;
    char genre[100];
    List* artists;          // lista de artistas (si tienes varios)
} Song;

typedef struct {
    char name[100];
    List *songs;
} Playlist;

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

int is_equal_str(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2) == 0;
}

void cargarCanciones(FILE *archivo, Map *songs_by_genre, Map *songs_by_artist, List *todas) {
    if (!archivo) {
        printf("Error al abrir el archivo.\n");
        return;
    }

    char linea[1024];
    fgets(linea, sizeof(linea), archivo); // Saltar encabezado

    while (fgets(linea, sizeof(linea), archivo)) {
        Song *song = malloc(sizeof(Song));
        if (!song) continue;

        char *token;

        token = strtok(linea, ",");
        if (!token) continue;
        strcpy(song->id, token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strcpy(song->track_name, token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strcpy(song->artist, token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strcpy(song->album_name, token);

        token = strtok(NULL, ",");
        if (!token) continue; // Este campo no se usa
        // ignorado

        token = strtok(NULL, ",");
        if (!token) continue;
        song->popularity = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        song->duration_ms = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        song->explicit = strcmp(token, "True") == 0;

        for (int i = 0; i < 11; i++) token = strtok(NULL, ",");
        if (!token) continue;
        song->tempo = atof(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strcpy(song->genre, token);

        song->artists = list_create();
        list_pushBack(song->artists, strdup(song->artist)); // Solo uno por ahora

        list_pushBack(todas, song);

        MapPair *pair_genre = map_search(songs_by_genre, song->genre);
        List *list_genre = pair_genre ? (List *)pair_genre->value : NULL;

        if (!list_genre) {
            list_genre = list_create();
            map_insert(songs_by_genre, song->genre, list_genre);
        }
        list_pushBack(list_genre, song);

        MapPair *pair_artist = map_search(songs_by_artist, song->artist);
        List *list_artist = pair_artist ? (List *)pair_artist->value : NULL;

        if (!list_artist) {
            list_artist = list_create();
            map_insert(songs_by_artist, song->artist, list_artist);
        }
        list_pushBack(list_artist, song);
    }

    printf("Canciones cargadas exitosamente.\n");
}

void buscar_por_genero(Map *songs_by_genre) {
    char genero[100];
    printf("Ingrese el género: ");
    scanf(" %[^\n]", genero);

    MapPair *pair = map_search(songs_by_genre, genero);
    if (!pair) {
        printf("No se encontraron canciones para el género '%s'.\n", genero);
        return;
    }

    List *lista = (List *)pair->value;
    Song *song = list_first(lista);
    while (song) {
        printf("ID: %s | Canción: %s | Álbum: %s | Género: %s | Tempo: %.2f\n",
               song->id, song->track_name, song->album_name, song->genre, song->tempo);
        song = list_next(lista);
    }
}


void buscar_por_artista(Map *songs_by_artist) {
    char artista[200];
    printf("Ingrese el nombre del artista: ");
    scanf(" %[^\n]", artista);

    MapPair *pair = map_search(songs_by_artist, artista);
    if (!pair) {
        printf("No se encontraron canciones para el artista '%s'.\n", artista);
        return;
    }

    List *lista = (List *)pair->value;
    Song *song = list_first(lista);
    while (song) {
        printf("ID: %s | Canción: %s | Álbum: %s | Género: %s | Tempo: %.2f\n",
               song->id, song->track_name, song->album_name, song->genre, song->tempo);
        song = list_next(lista);
    }
}

void buscar_por_tempo(List *todasLasCanciones) {
    int opcion;
    printf("Selecciona el tipo de tempo:\n");
    printf("1) Lentas (<80 BPM)\n2) Moderadas (80-120 BPM)\n3) Rapidas (>120 BPM)\nOpción: ");
    scanf("%d", &opcion);

    Song *song = list_first(todasLasCanciones);
    while (song) {
        if ((opcion == 1 && song->tempo < 80) ||
            (opcion == 2 && song->tempo >= 80 && song->tempo <= 120) ||
            (opcion == 3 && song->tempo > 120)) {
            printf("ID: %s | %s | Tempo: %.2f | Genero: %s\n", song->id, song->track_name, song->tempo, song->genre);
        }
        song = list_next(todasLasCanciones);
    }
}

void crear_lista(Map* listas_reproduccion) {
    char nombre[100];
    printf("Ingrese el nombre de la nueva lista: ");
    scanf(" %[^\n]", nombre);

    if (map_search(listas_reproduccion, nombre) != NULL) {
        printf("Ya existe una lista con ese nombre.\n");
        return;
    }

    List* nueva_lista = list_create();
    map_insert(listas_reproduccion, strdup(nombre), nueva_lista);
    printf("Lista \"%s\" creada correctamente.\n", nombre);
}

void agregar_cancion_a_lista(Map* canciones_por_id, Map* listas_reproduccion) {
    char id[100], nombre_lista[100];

    printf("Ingrese el ID de la canción: ");
    scanf(" %s", id);
    printf("Ingrese el nombre de la lista de reproducción: ");
    scanf(" %[^\n]", nombre_lista);

    MapPair* cancion_pair = map_search(canciones_por_id, id);
    MapPair* lista_pair = map_search(listas_reproduccion, nombre_lista);

    if (!cancion_pair) {
        printf("No se encontro la canción con ID %s.\n", id);
        return;
    }
    if (!lista_pair) {
        printf("No se encontro la lista \"%s\".\n", nombre_lista);
        return;
    }

    list_pushBack((List*)lista_pair->value, cancion_pair->value);
    printf("Canción añadida correctamente a la lista.\n");
}

void mostrar_lista(Map* listas_reproduccion) {
    char nombre_lista[100];
    printf("Ingrese el nombre de la lista: ");
    scanf(" %[^\n]", nombre_lista);

    MapPair* lista_pair = map_search(listas_reproduccion, nombre_lista);
    if (!lista_pair) {
        printf("No se encontro la lista \"%s\".\n", nombre_lista);
        return;
    }

    List* canciones = (List*)lista_pair->value;
    Song* cancion = list_first(canciones);

    printf("\nLista \"%s\":\n", nombre_lista);
    while (cancion != NULL) {
        printf("ID: %s\n", cancion->id);
        printf("Nombre: %s\n", cancion->track_name);
        printf("Álbum: %s\n", cancion->album_name);
        printf("Género: %s\n", cancion->genre);
        printf("Tempo: %.2f\n", cancion->tempo);
        printf("Artistas:\n");

        for (char* artista = list_first(cancion->artists); artista != NULL; artista = list_next(cancion->artists)) {
            printf("  - %s\n", artista);
        }
        printf("---------------\n");

        cancion = list_next(canciones);
    }
}

int main() {
    Map *mapaCanciones = map_create(is_equal_str);
    Map *by_genre = map_create(is_equal_str);
    Map *by_artist = map_create(is_equal_str);
    Map *playlists = map_create(is_equal_str);
    List *all_songs = list_create();

    int opcion;
    do {
        mostrarMenuPrincipal();
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
            {/*
                FILE *archivo = fopen("data/song_dataset_.csv", "r");
                if (archivo == NULL) {
                    printf("No se pudo abrir el archivo \n");
                    break;
                }
                cargarCanciones(archivo, by_genre, by_artist, all_songs);
                fclose(archivo);
                printf("Canciones cargadas exitosamente\n");
                break;
                */
                FILE *archivo = fopen("data/song_dataset_.csv", "r");
                cargarCanciones(archivo, by_genre, by_artist, all_songs);
                fclose(archivo);
                break;
             
            }

            case 2:
                buscar_por_genero(by_genre);
                break;
            case 3:
                buscar_por_artista(by_artist);
                break;
            case 4:
                buscar_por_tempo(all_songs);
                break;
            case 5:
                crear_lista(playlists);
                break;
            case 6:
                agregar_cancion_a_lista(mapaCanciones, playlists);
                break;
            case 7:
                mostrar_lista(playlists);
                break;
            case 8:
                printf("¡Hasta luego!\n");
                break;
            default:
                printf("Opción no valida. Intente nuevamente.\n");
                break;
        }

        if (opcion != 8) {
            printf("\nPresione ENTER para continuar...");
            getchar(); getchar();
        }

    } while (opcion != 8);

    return 0;
}
