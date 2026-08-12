
#include "../LinkedList.h"
#include <stdio.h>
#include <stdlib.h>

int lltests_passed = 0;
void test_ll(){

  fprintf(stdout, "Testing LinkedList allocate\n");
  LinkedList* ll = LinkedList_allocate();
  if(ll != NULL) {
    fprintf(stdout, "Test 1 passed\n");
  } else { 
    fprintf(stdout, "Test 1 failed, exiting");
    exit(EXIT_FAILURE);
  }

  fprintf(stdout, "testing insert\n");
  for(int i = 0; i < 100; i++) {
    int* val = (int*)malloc(sizeof(int));
    *val = i;
    ll_push(ll, val);
  }
  fprintf(stdout, "passed insert\n");
  for(int i = 99; i >= 0; i--) {
    int* val = pop(ll); 
    if(*val != i) {
      fprintf(stderr, "pop failed\n");
      exit(EXIT_FAILURE);
    }
    free(val);
  }

  fprintf(stdout, "passed pop\n");



}


int main () {
  test_ll();
}
