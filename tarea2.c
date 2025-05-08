#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char id[100];
    char track_name[200];
    char album_name[200];
    List* artists;
    float tempo;
    char genre[100];
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
    printf("Seleccione una opción: ");
}

/**
 * Compara dos claves de tipo string para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo string.
 *
 * @param key1 Primer puntero a la clave string.
 * @param key2 Segundo puntero a la clave string.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_str(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2) == 0;
}
  
  /**
   * Compara dos claves de tipo entero para determinar si son iguales.
   * Esta función se utiliza para inicializar mapas con claves de tipo entero.
   *
   * @param key1 Primer puntero a la clave entera.
   * @param key2 Segundo puntero a la clave entera.
   * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
   */
int is_equal_int(void *key1, void *key2) {
    return *(int *)key1 == *(int *)key2; // Compara valores enteros directamente
}
  
  
void cargar_canciones(Map *songs_by_id, Map *songs_by_genre, Map *songs_by_artist, List *todasLasCanciones) {
    FILE *archivo = fopen("data/song_dataset_.csv", "r");
    if (!archivo) {
        perror("Error al abrir el archivo");
        return;
    }

    char **campos = leer_linea_csv(archivo, ',');
    if (!campos) {
        puts("El archivo CSV está vacío.");
        fclose(archivo);
        return;
    }

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        if (!campos[0] || !campos[2] || !campos[3] || !campos[4] || !campos[18] || !campos[20]) continue;

        if (map_search(songs_by_id, campos[0]) != NULL) continue;

        Song *song = malloc(sizeof(Song));
        if (!song) {
            perror("Error al reservar memoria para canción");
            continue;
        }

        strncpy(song->id, campos[0], sizeof(song->id) - 1);
        song->id[sizeof(song->id) - 1] = '\0';

        song->artists = split_string(campos[2], ";");

        strncpy(song->album_name, campos[3], sizeof(song->album_name) - 1);
        song->album_name[sizeof(song->album_name) - 1] = '\0';

        strncpy(song->track_name, campos[4], sizeof(song->track_name) - 1);
        song->track_name[sizeof(song->track_name) - 1] = '\0';

        song->tempo = atof(campos[18]);

        strncpy(song->genre, campos[20], sizeof(song->genre) - 1);
        song->genre[sizeof(song->genre) - 1] = '\0';

    
        list_pushBack(todasLasCanciones, song);

 
        map_insert(songs_by_id, strdup(song->id), song);

      
        MapPair *pair_genre = map_search(songs_by_genre, song->genre);
        if (!pair_genre) {
            List *list_genre = list_create();
            list_pushBack(list_genre, song);
            map_insert(songs_by_genre, strdup(song->genre), list_genre);
        } else {
            list_pushBack((List *)pair_genre->value, song);
        }

     
        for (char *artista = list_first(song->artists); artista != NULL; artista = list_next(song->artists)) {
            MapPair *pair_artist = map_search(songs_by_artist, artista);
            if (!pair_artist) {
                List *list_artist = list_create();
                list_pushBack(list_artist, song);
                map_insert(songs_by_artist, strdup(artista), list_artist);
            } else {
                list_pushBack((List *)pair_artist->value, song);
            }
        }
    }

    fclose(archivo);
    puts("Canciones cargadas exitosamente.");
}

void buscar_por_genero(Map *songs_by_genre) {
    char genero[100];
    printf("Ingrese el género: ");
    scanf(" %s", genero);

    MapPair *pair = map_search(songs_by_genre, genero);
    if (!pair) {
        printf("No se encontraron canciones para el género '%s'.\n", genero);
        return;
    }

    List *lista = pair->value;
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
    scanf(" %[^\n]s", artista);

    MapPair *pair = map_search(songs_by_artist, artista);
    if (!pair) {
        printf("No se encontraron canciones para el artista '%s'.\n", artista);
        return;
    }

    List *lista = pair->value;
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
    printf("1) Lentas (<80 BPM)\n2) Moderadas (80-120 BPM)\n3) Rápidas (>120 BPM)\nOpción: ");
    scanf("%d", &opcion);

    Song *song = list_first(todasLasCanciones);
    while (song) {
        if ((opcion == 1 && song->tempo < 80) ||
            (opcion == 2 && song->tempo >= 80 && song->tempo <= 120) ||
            (opcion == 3 && song->tempo > 120)) {
            printf("ID: %s | %s | Tempo: %.2f | Género: %s\n", song->id, song->track_name, song->tempo, song->genre);
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
        printf("No se encontró la canción con ID %s.\n", id);
        return;
    }
    if (!lista_pair) {
        printf("No se encontró la lista \"%s\".\n", nombre_lista);
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
        printf("No se encontró la lista \"%s\".\n", nombre_lista);
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
                cargar_canciones(mapaCanciones, by_genre, by_artist, all_songs);
                break;
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
                printf("Opción no válida. Intente nuevamente.\n");
                break;
        }

        if (opcion != 8) {
            printf("\nPresione ENTER para continuar...");
            getchar();  
            getchar();  
        }

    } while (opcion != 8);

    return 0;
}