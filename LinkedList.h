////////////////////////////////////////////////////////////
// basic Implementation of a LinkedList
////////////////////////////////////////////////////////
typedef struct LinkedList {
  void* val;
  struct LinkedList* next;
  struct LinkedList* prev;

} LinkedList;

//////////////////////////////////////////////////////
// DESCRIPTION
//    LinkedList_allocate() returns a pointer
//    to a newly allocated LinkedList with
//    nothing in it
///////////////////////////////////////////////////
LinkedList* LinkedList_allocate();


///////////////////////////////////////////////////////////
// DESCRIPTION
//    returns true if a LinkedList is empty and 
//    false otherwise
//
//    returns true if ll is NULL
/////////////////////////////////////////////////////////
int List_is_empty(LinkedList* ll);

//////////////////////////////////////////////////////////
// DESCRIPTION
//    ll_push appends the given element to the end
//    of a LinkedList (note that val may not 
//    be NULL)
//
// RETURNS
//    upon succcesfully appending an item to the
//    LinkedList ll_push will return 0. on failure
//    ll_push returns -1 
///////////////////////////////////////////////////
int ll_push(LinkedList* ll, void* val);

////////////////////////////////////////////////////////
// DESCRIPTION
//    ll_pre_append appends an element to the end
//    of a LinkedList allowing for 
///////////////////////////////////////////////////
int ll_pre_append(LinkedList* ll, void* val);


///////////////////////////////////////////////////////
// DESCRIPTION
//    pop() removes and returns the lst element of
//    a LinkedList 
// 
// RETURNS
//    upon succcesfully removing an element pop()
//    return the element removed from the 
//    LinkedList on failure pop() returns NULL
///////////////////////////////////////////////////
void* pop(LinkedList* ll);


/////////////////////////////////////////////////////
// DESCRIPTION
//    remove() returns and removes the first
//    element in a LinkedList
//
// RETURNS
//    upon succcesfully removing an element remove
//    returns that element, on failure NULL is 
//    returned
//////////////////////////////////////////////////
void* LinkedList_remove(LinkedList* ll);




//////////////////////////////////////////////////////////
// DESCRIPTION
//    frees all memory associated with a LinkedList
///////////////////////////////////////////////////////
void LinkedList_free(LinkedList* ll);
