#ifndef OBJECT_H
#define OBJECT_H

#include "value.h"

/* All heap object types */
typedef enum {
    OBJ_PAIR,
    OBJ_FUNCTION,
    OBJ_CLOSURE
} ObjType;

/* Base header for every heap object */
typedef struct Obj {
    ObjType type;
    int marked;      /* for garbage collection (GC mark bit)*/
    struct Obj* next;   /* linked list of all heap objects */
} Obj;

/* Pair object */
typedef struct {
    Obj base;
    Value left;
    Value right;
} ObjPair;

/* ===== Heap tracking ===== */
extern Obj* heap_objects;

void heap_register(Obj* o);


/* Allocation */
ObjPair* new_pair(Value l, Value r);


/* graph creation from list(heap) .*/
void obj_visit_children(Obj* o, void (*visit)(Obj*));


/* GC roots & mark phase */
//garbage collector functions would go here
void gc_mark_from_roots();

void gc_add_root(Obj* o);

//This is where memory is actually freed.
void gc_sweep();

// Complete garbage collection cycle
void gc_collect();






#endif
