/*
 * Spotifind - Gestor de canciones y listas de reproducción
 */

#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definición de estructuras
typedef struct {
    char id[100];
    char track_name[200];
    char album_name[200];
    List* artists; // Lista de char*
    float tempo;
    char genre[100];
} Song;

// Menú principal
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
    printf("Seleccione una opción: ");
}

int is_equal_str(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2) == 0;
}

// Función para eliminar espacios y saltos de línea al final de una cadena
void quitarEspaciosFinales(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

void cargarCanciones(Map *mapaCanciones, Map *by_genre, Map *by_artist, List *all_songs, FILE *archivo) {
    char linea[1024];
    if (!fgets(linea, sizeof(linea), archivo)) return;  // Saltar cabecera

    while (fgets(linea, sizeof(linea), archivo)) {
        char *campos[30];
        int i = 0;
        char *linea_copia = strdup(linea);
        if (!linea_copia) continue;
        linea_copia[strcspn(linea_copia, "\r\n")] = 0;

        char *token = strtok(linea_copia, ",");
        while (token && i < 30) {
            campos[i++] = token;
            token = strtok(NULL, ",");
        }

        if (i < 22) {
            free(linea_copia);
            continue;
        }

        char *id = campos[0];
        if (!id || map_search(mapaCanciones, id)) {
            free(linea_copia);
            continue;
        }

        Song *song = malloc(sizeof(Song));
        if (!song) {
            free(linea_copia);
            continue;
        }

        strncpy(song->id, id, sizeof(song->id) - 1);
        song->id[sizeof(song->id) - 1] = '\0';
        quitarEspaciosFinales(song->id); 


        song->artists = list_create();
        list_pushBack(song->artists, strdup(campos[2]));

        strncpy(song->album_name, campos[3], sizeof(song->album_name) - 1);
        song->album_name[sizeof(song->album_name) - 1] = '\0';

        strncpy(song->track_name, campos[4], sizeof(song->track_name) - 1);
        song->track_name[sizeof(song->track_name) - 1] = '\0';

        song->tempo = atof(campos[19]);

        strncpy(song->genre, campos[21], sizeof(song->genre) - 1);
        song->genre[sizeof(song->genre) - 1] = '\0';

        char *id_copia = strdup(song->id);
        if (!id_copia) {
            free(song);
            free(linea_copia);
            continue;
        }
        map_insert(mapaCanciones, id_copia, song);

        MapPair *pair_genre = map_search(by_genre, song->genre);
        List *list_genre = pair_genre ? pair_genre->value : NULL;
        if (!list_genre) {
            list_genre = list_create();
            map_insert(by_genre, strdup(song->genre), list_genre);
        }
        list_pushBack(list_genre, song);

        char *artista = list_first(song->artists);
        if (artista) {
            MapPair *pair_artist = map_search(by_artist, artista);
            List *list_artist = pair_artist ? pair_artist->value : NULL;
            if (!list_artist) {
                list_artist = list_create();
                map_insert(by_artist, strdup(artista), list_artist);
            }
            list_pushBack(list_artist, song);
        }

        list_pushBack(all_songs, song);
        free(linea_copia);
    }

    printf("Canciones cargadas correctamente.\n");
}

void buscarPorGenero(Map *songs_by_genre) {
    char genero[100];
    printf("Ingrese el género: ");
    scanf(" %[^\n]", genero);


    MapPair *pair = map_search(songs_by_genre, genero);
    if (!pair) {
        printf("No se encontraron canciones para el género '%s'.\n", genero);
        return;
    }

    List *lista = pair->value;
    for (Song *s = list_first(lista); s; s = list_next(lista)) {
        printf("%s | %s | %s | %s | %.2f BPM\n",
               s->id, s->track_name, s->album_name, s->genre, s->tempo);
    }
}

void buscarPorArtista(Map *songs_by_artist) {
    char artista[100];
    printf("Ingrese el artista: ");
    scanf(" %[^\n]", artista);

    MapPair *pair = map_search(songs_by_artist, artista);
    if (!pair) {
        printf("No se encontraron canciones para el artista '%s'.\n", artista);
        return;
    }

    List *lista = pair->value;
    for (Song *s = list_first(lista); s; s = list_next(lista)) {
        printf("%s | %s | %s | %s | %.2f BPM\n",
               s->id, s->track_name, s->album_name, s->genre, s->tempo);
    }
}

void buscarPorTempo(List *todas) {
    int opcion;
    printf("Seleccione tipo de tempo:\n1) Lento (<80)\n2) Moderado (80-120)\n3) Rápido (>120)\nOpcion: ");
    scanf("%d", &opcion);

    for (Song *s = list_first(todas); s; s = list_next(todas)) {
        if ((opcion == 1 && s->tempo < 80) ||
            (opcion == 2 && s->tempo >= 80 && s->tempo <= 120) ||
            (opcion == 3 && s->tempo > 120)) {
            printf("%s | %s | %.2f BPM | %s\n", s->id, s->track_name, s->tempo, s->genre);
        }
    }
}

void crearLista(Map *listas) {
    char nombre[100];
    printf("Nombre de la lista: ");
    scanf(" %[^\n]", nombre);


    if (map_search(listas, nombre)) {
        printf("La lista ya existe.\n");
        return;
    }

    map_insert(listas, strdup(nombre), list_create());
    printf("Lista '%s' creada.\n", nombre);
}


void agregarCancion(Map *por_id, Map *listas) {
    char id[100], lista[100];

    printf("ID de canción: ");
    getchar();  // limpia el \n pendiente en stdin
    fgets(id, sizeof(id), stdin);
    quitarEspaciosFinales(id);

    printf("Nombre de la lista: ");
    fgets(lista, sizeof(lista), stdin);
    quitarEspaciosFinales(lista);

    // Depuración opcional
    printf("Buscando ID: '%s'\n", id);
    printf("Buscando lista: '%s'\n", lista);

    MapPair *song_pair = map_search(por_id, id);
    if (!song_pair) {
        printf("No se encontró la canción con ID '%s'.\n", id);
        return;
    }

    MapPair *list_pair = map_search(listas, lista);
    if (!list_pair) {
        printf("No se encontró la lista '%s'.\n", lista);
        return;
    }

    List *listaCanciones = list_pair->value;
    list_pushBack(listaCanciones, song_pair->value);

    printf("Canción '%s' añadida correctamente a la lista '%s'.\n", id, lista);
}


void mostrarLista(Map *listas) {
    char nombre[100];
    printf("Nombre de la lista: ");
    scanf(" %[^\n]", nombre);


    MapPair *pair = map_search(listas, nombre);
    if (!pair) {
        printf("Lista no encontrada.\n");
        return;
    }

    printf("\n--- Lista '%s' ---\n", nombre);
    for (Song *s = list_first(pair->value); s; s = list_next(pair->value)) {
        printf("%s | %s | %s | %s | %.2f BPM\n",
               s->id, s->track_name, s->album_name, s->genre, s->tempo);
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
            case 1: {
                FILE *archivo = fopen("data/song_dataset_.csv", "r");
                if (archivo) {
                    cargarCanciones(mapaCanciones, by_genre, by_artist, all_songs, archivo);
                    fclose(archivo);
                } else {
                    printf("No se pudo abrir el archivo.\n");
                }
                break;
            }
            case 2: buscarPorGenero(by_genre); break;
            case 3: buscarPorArtista(by_artist); break;
            case 4: buscarPorTempo(all_songs); break;
            case 5: crearLista(playlists); break;
            case 6: agregarCancion(mapaCanciones, playlists); break;
            case 7: mostrarLista(playlists); break;
            case 8: printf("Saliendo...\n"); break;
            default: printf("Opción no válida.\n"); break;
        }

        if (opcion != 8) {
            printf("Presione ENTER para continuar...");
            getchar(); getchar();
        }
    } while (opcion != 8);

    return 0;
}
