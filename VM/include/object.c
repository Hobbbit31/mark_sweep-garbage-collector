#include <stdlib.h>
#include "object.h"

/* ===== Heap list ===== */
Obj* heap_objects = NULL;

/* Register object in heap list */
void heap_register(Obj* o) {
    o->next = heap_objects;
    heap_objects = o;
}

/* ===== Pair allocation ===== */
ObjPair* new_pair(Value l, Value r) {
    ObjPair* pair = (ObjPair*)malloc(sizeof(ObjPair));
    if (!pair) return NULL;

    pair->base.type = OBJ_PAIR;
    pair->base.marked = 0;

    heap_register((Obj*)pair);

    pair->left = l;
    pair->right = r;

    return pair;
}

/* ===== Object graph traversal ===== */
void obj_visit_children(Obj* o, void (*visit)(Obj*)) {
    if (!o) return;

    switch (o->type) {
        case OBJ_PAIR: {
            ObjPair* p = (ObjPair*)o;

            if (p->left.type == VAL_OBJ && p->left.obj)
                visit(p->left.obj);

            if (p->right.type == VAL_OBJ && p->right.obj)
                visit(p->right.obj);

            break;
        }

        case OBJ_FUNCTION:
        case OBJ_CLOSURE:
            break;
    }
}

/* ===== GC mark phase ===== */

static Obj* gc_roots[32];
static int gc_root_count = 0;

void gc_add_root(Obj* o) {
    gc_roots[gc_root_count++] = o;
}

static void gc_mark(Obj* o) {
    if (!o) return;
    if (o->marked) return;

    o->marked = 1;
    obj_visit_children(o, gc_mark);
}

void gc_mark_from_roots() {
    for (int i = 0; i < gc_root_count; i++) {
        gc_mark(gc_roots[i]);
    }
}


/* ===== GC SWEEP PHASE ===== */

void gc_sweep() {
    Obj** obj = &heap_objects;

    while (*obj) {
        if (!(*obj)->marked) {
            /* unreachable → free it */
            Obj* unreached = *obj;
            *obj = unreached->next;
            free(unreached);
        } else {
            /* still alive → unmark for next GC */
            (*obj)->marked = 0;
            obj = &(*obj)->next;
        }
    }
}


void gc_collect() {
    gc_mark_from_roots();
    gc_sweep();
}
