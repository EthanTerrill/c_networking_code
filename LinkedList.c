#include "LinkedList.h"

#include <stdlib.h>


LinkedList* LinkedList_allocate() {
  LinkedList* ll;

  ll = (LinkedList*)malloc(sizeof(LinkedList));
  if (ll == NULL) {
    return NULL;
  }

  ll->val   = NULL;
  ll->next  = NULL;
  ll->prev  = NULL;

  return ll;
}


int List_is_empty(LinkedList* ll) {
  if (ll == NULL) {
    return 1;
  }
  return (ll->next == NULL) &&
          (ll->prev == NULL) &&
          (ll->val == NULL);
}


int ll_push(LinkedList* ll, void* val) { 
  LinkedList* cursor;
  LinkedList* new;
  
  if(ll == NULL || val == NULL) {
    return -1;
  }

  cursor = ll;

  if (cursor->val == NULL){
    cursor->val = val;
    return 0;
  }

  while (cursor->next != NULL) {
    cursor = cursor->next;  
  }
  new = (LinkedList*)malloc(sizeof(LinkedList));
  cursor->next = new;
  new->prev = cursor;
  new->val = val; 

  return 0;
}




int ll_pre_append(LinkedList* ll, void* val) {
  LinkedList* cursor;
  LinkedList* new;
  
  if(ll == NULL || val == NULL) {
    return -1;
  }

  cursor = ll;
  if (cursor->val == NULL){
    cursor->val = val;
    return 0;
  }

  while (cursor->prev != NULL) {
    cursor = cursor->prev;  
  }
  new = (LinkedList*)malloc(sizeof(LinkedList));
  cursor->prev = new;
  new->next = cursor;
  new->val = val; 

  return 0;
}


void* pop(LinkedList* ll) {
  LinkedList* cursor;
  void* ret;
  
  if(ll == NULL) {
    return NULL;
  }

  cursor = ll;
  if (cursor->val == NULL){
    return NULL;
  }

  while (cursor->next != NULL) {
    cursor = cursor->next;
  }
  ret = cursor->val;
  if(cursor->prev == NULL) {
    free(cursor);
  } else {
    cursor = cursor->prev;
    free(cursor->next);
    cursor->next = NULL;
  }

  return ret;
}

void* LinkedList_remove(LinkedList* ll) {
  LinkedList* cursor;
  void* ret;
  
  if(ll == NULL) {
    return NULL;
  }

  cursor = ll;
  if (cursor->val == NULL){
    return NULL;
  }

  while (cursor->prev != NULL) {
    cursor = cursor->prev;
  }

  ret = cursor->val;
  if(cursor->next == NULL) {
    free(cursor);
  } else {
    cursor = cursor->next;
    free(cursor->prev);
    cursor->prev = NULL;
  }

  return ret;
}
