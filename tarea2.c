#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    char id[100];
    List *artists;
    char album_name[256];
    char track_name[256];
    float tempo;
    char genre[100];
}Song;

typedef struct {
    char name[100];
    List *songs; 
}Playlist;


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


void cargar_canciones(Map *by_id, Map *by_genre, Map *by_artist, List *all_songs) {
  FILE *archivo = fopen("data/song_dataset_.csv", "r");
  if (archivo == NULL) {
      perror("Error al abrir el archivo");
      return;
  }

  char **campos;
  campos = leer_linea_csv(archivo, ','); // Leer encabezados

  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
      if (!campos[1] || !campos[2] || !campos[3] || !campos[4] || !campos[18] || !campos[20])
          continue;

      Song *s = malloc(sizeof(Song));
      if (!s) continue;

      strcpy(s->id, campos[1]);
      strcpy(s->track_name, campos[4]);
      s->artists = split_string(campos[2], ";");
      strcpy(s->album_name, campos[3]);
      s->tempo = atof(campos[18]);
      strcpy(s->genre, campos[20]);

      list_pushBack(all_songs, s);
      map_insert(by_id, s->id, s);

      // por género
      MapPair *pair = map_search(by_genre, s->genre);  // Buscar en el mapa por género
      List *genero_list = NULL;

      if (pair) {
          genero_list = (List *) pair->value;  // Si ya existe el género, obtener la lista
      } else {
          genero_list = list_create();  // Si no existe, crear una nueva lista
          map_insert(by_genre, s->genre, genero_list);  // Insertar el género con su lista
      }
      list_pushBack(genero_list, s);  // Agregar la canción a la lista de género

      // por artista
      for (char *artista = list_first(s->artists); artista != NULL; artista = list_next(s->artists)) {
          MapPair *pair_artista = map_search(by_artist, artista);  // Buscar en el mapa por artista
          List *artista_list = NULL;

          if (pair_artista) {
              artista_list = (List *) pair_artista->value;  // Si ya existe el artista, obtener la lista
          } else {
              artista_list = list_create();  // Si no existe, crear una nueva lista
              map_insert(by_artist, artista, artista_list);  // Insertar el artista con su lista
          }
          list_pushBack(artista_list, s);  // Agregar la canción a la lista de artista
      }
  }
  fclose(archivo);
  puts("Canciones cargadas con éxito.");
}   

void buscar_por_genero(Map *by_genre) {
    char genero[100];
    printf("Ingrese el género: ");
    scanf(" %[^\n]s", genero);

    MapPair *pair = map_search(by_genre, genero);
    if (!pair) {
        printf("No se encontraron canciones con el género '%s'\n", genero);
        return;
    }

    List *lista = pair->value;
    Song *cancion = list_first(lista);
    while (cancion) {
        printf("🎵 %s - %s [%s | Tempo %.2f]\n", list_first(cancion->artists), cancion->track_name, cancion->album_name, cancion->tempo);
        cancion = list_next(lista);
    }
}

void buscar_por_artista(Map *by_artist) {
    char artista[100];
    printf("Ingrese el nombre del artista: ");
    scanf(" %[^\n]s", artista);

    MapPair *pair = map_search(by_artist, artista);
    if (!pair) {
        printf("No se encontraron canciones de ese artista.\n");
        return;
    }

    List *lista = pair->value;
    Song *c = list_first(lista);
    while (c) {
        char *a = list_first(c->artists);
        while (a) {
            printf("%s", a);
            a = list_next(c->artists);
            if (a) printf(", ");
        }
        printf(" - %s [%s | Tempo %.2f]\n", c->track_name, c->album_name, c->tempo);
        c = list_next(lista);
    }
}

void buscar_por_tempo(List *all_songs) {
  float tempo_min, tempo_max;
  printf("Ingrese el rango de tempo (mínimo máximo): ");
  scanf("%f %f", &tempo_min, &tempo_max);

  int found = 0;  // Para verificar si se encontró alguna canción en el rango

  Song *cancion = list_first(all_songs);
  while (cancion) {
      if (cancion->tempo >= tempo_min && cancion->tempo <= tempo_max) {
          // Mostrar detalles de la canción
          char *artista = list_first(cancion->artists);
          while (artista) {
              printf("%s", artista);
              artista = list_next(cancion->artists);
              if (artista) printf(", ");
          }
          printf(" - %s [%s | Tempo %.2f BPM | %s]\n", cancion->track_name, cancion->album_name, cancion->tempo, cancion->genre);
          found = 1;
      }
      cancion = list_next(all_songs); // Avanzar a la siguiente canción
  }

  if (!found) {
      printf("No se encontraron canciones en el rango de tempo %.2f - %.2f\n", tempo_min, tempo_max);
  }
}

void crear_playlist(Map *playlists){
    char nombre[100];
    printf("Ingrese el nombre de la nueva lista de reproducción: ");
    scanf(" %[^\n]s", nombre);

    // Verificar si la lista ya existe
    if (map_search(playlists, nombre)) {
        printf("¡Ya existe una lista con ese nombre!\n");
        return;
    }

    // Crear lista de reproducción vacía
    List *nueva_lista = list_create();
    map_insert(playlists, nombre, nueva_lista);

    printf("Lista de reproducción '%s' creada con éxito.\n", nombre);

}
void agregar_a_playlist(Map *playlists, Map *by_id){
    char playlist_name[100], song_id[100];
    printf("Ingrese el nombre de la lista de reproducción: ");
    scanf(" %[^\n]s", playlist_name);

    MapPair *pair = map_search(playlists, playlist_name);
    if (!pair) {
        printf("La lista de reproducción no existe.\n");
        return;
    }

    List *playlist = pair->value;
    printf("Ingrese el ID de la canción que desea agregar: ");
    scanf(" %[^\n]s", song_id);

    MapPair *song_pair = map_search(by_id, song_id);
    if (!song_pair) {
        printf("No se encontró una canción con ese ID.\n");
        return;
    }

    Song *song = song_pair->value;
    list_pushBack(playlist, song);
    printf("Canción '%s' agregada a la lista '%s'.\n", song->track_name, playlist_name);
}

void mostrar_playlist(Map *playlists) {
    char playlist_name[100];
    printf("Ingrese el nombre de la lista de reproducción: ");
    scanf(" %[^\n]s", playlist_name);

    MapPair *pair = map_search(playlists, playlist_name);
    if (!pair) {
        printf("La lista de reproducción no existe.\n");
        return;
    }

    List *playlist = pair->value;
    Song *song = list_first(playlist);
    while (song) {
        // Imprimir artistas
        char *artist = list_first(song->artists);
        while (artist) {
            printf("%s", artist);
            artist = list_next(song->artists);
            if (artist) printf(", ");
        }
        printf(" - %s [%s | Tempo %.2f BPM | %s]\n", song->track_name, song->album_name, song->tempo, song->genre);
        song = list_next(playlist); // Avanzar a la siguiente canción en la lista
    }
}

int main() {
  Map *by_id = map_create(is_equal_str);
  Map *by_genre = map_create(is_equal_str);
  Map *by_artist = map_create(is_equal_str);
  Map *playlists = map_create(is_equal_str);
  List *all_songs = list_create();

  char opcion;

  do {
      mostrarMenuPrincipal();
      scanf(" %c", &opcion);

      switch (opcion) {
          case '1':
              cargar_canciones(by_id, by_genre, by_artist, all_songs);
              break;
          case '2':
              buscar_por_genero(by_genre);
              break;
          case '3':
              buscar_por_artista(by_artist);
              break;
          case '4':
              buscar_por_tempo(all_songs);
              break;
          case '5':
              crear_playlist(playlists);
              break;
          case '6':
              agregar_a_playlist(playlists, by_id);
              break;
          case '7':
              mostrar_playlist(playlists);
              break;
          case '8':
              puts("Saliendo...");
              break;
          default:
              puts("Opción inválida.");
              break;
      }
      presioneTeclaParaContinuar();
  } while (opcion != '8');

  return 0;
}
