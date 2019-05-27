#include "list.h"

struct list{
  void** data;
  size_t size;
  size_t capacity;
  FreeFunc freef;
  EqualFunc equalf;
};

List *list_init(size_t initial_size, FreeFunc freer, EqualFunc equalf){
  List* list = malloc(sizeof(List));
  assert(list != NULL);

  list->data = malloc(initial_size * sizeof(void *));
  assert(list->data != NULL);

  list->capacity = initial_size;
  list->size = 0;

  list->freef = freer;
  list->equalf = equalf;
  return list;
}

void list_free(List* list){
  if(list->freef != NULL){
    for(int i = 0; i < list->size; i++){
        list->freef(list->data[i]);
    }
  }
  free(list->data);
  free(list);
}

size_t list_size(List *list){
  return list->size;
}

void *list_get(List *list, size_t index){
  assert(index < list->size);
  return list->data[index];
}

void list_set(List *list, size_t index, void* value){
  assert(index < list->size);
  list->freef(list->data[index]);
  list->data[index] = value;
  list->freef(value);
}

void list_add(List *list, void* value){
  if(list_size(list) >= list->capacity){
    list->data = realloc(list->data, list->capacity * 2 * sizeof(void*));
    assert(list->data != NULL);
    list->capacity *= 2;
  }
  list->data[list_size(list)] = value;
  list->size++;
}

void list_remove(List *list, size_t ind_rem){
  assert(list_size(list) > 0 && ind_rem < list_size(list));
  void* remove = list_get(list, ind_rem);
  for(int i = ind_rem; i < list_size(list) - 1; i++){
      list->data[i] = list_get(list, i + 1);
  }
  list->size--;
  list->freef(remove);
}

bool list_contains(List *list, void* value){
    if (list->equalf != NULL) {
        for (int i = 0; i < list->size; i++){
            if (list->equalf(list->data[i], value)){
                return true;
            }
        }
    }
    return false;
}

EqualFunc list_get_equalf(List *list) {
    return list->equalf;
}
