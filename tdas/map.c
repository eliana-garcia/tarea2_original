#include "map.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int (*current_lt)(void *, void *) = NULL;

int pair_lt(void *pair1, void *pair2) {
  return (current_lt(((MapPair *)pair1)->key, ((MapPair *)pair2)->key));
}

Map *sorted_map_create(int (*lower_than)(void *key1, void *key2)) {
  Map *newMap = (Map *)malloc(sizeof(Map));
  newMap->lower_than = lower_than;
  newMap->is_equal = NULL;
  newMap->ls = list_create();
  current_lt = lower_than;
  return newMap;
}

Map *map_create(int (*is_equal)(void *key1, void *key2)) {
  Map *newMap = (Map *)malloc(sizeof(Map));
  newMap->is_equal = is_equal;
  newMap->lower_than = NULL;
  newMap->ls = list_create();
  return newMap;
}

void map_insert(Map *map, void *key, void *value) {
  MapPair *pair = (MapPair *)malloc(sizeof(MapPair));
  pair->key = key;
  pair->value = value;
  if (map->lower_than != NULL) {
    list_sortedInsert(map->ls, pair, pair_lt);
  } else {
    list_pushBack(map->ls, pair);
  }
}

MapPair *map_remove(Map *map, void *key) {
  List *list = map->ls;
  list_first(list);
  MapPair *pair;
  while ((pair = list_next(list)) != NULL) {
    if (map->is_equal(pair->key, key)) {
      list_popCurrent(list);
      return pair;
    }
  }
  return NULL;
}

MapPair *map_search(Map *map, void *key) {
  List *list = map->ls;
  list_first(list);
  MapPair *pair;
  while ((pair = list_next(list)) != NULL) {
    if (map->is_equal(pair->key, key)) {
      return pair;
    }
  }
  return NULL;
}

MapPair *map_first(Map *map) {
  List *list = map->ls;
  list_first(list);
  return list_next(list);
}

MapPair *map_next(Map *map) {
  List *list = map->ls;
  return list_next(list);
}

void map_clean(Map *map) {
  list_clean(map->ls);
  free(map);
}