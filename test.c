#include "VM/include/object.h"
#include "VM/include/value.h"
#include "VM/vm.h"
#include <stdio.h>

int main() {
    Program p;
    vm_init(&p, NULL, 0);

    /* Create deep chain: pair -> pair -> pair -> ... */
    ObjPair *head = new_pair(make_int(0), make_int(0));
    ObjPair *cur = head;

    for (int i = 0; i < 2000; i++) {
        ObjPair *next = new_pair(make_int(i), make_int(i));
        cur->right = make_obj((Obj*)next);
        cur = next;
    }

    /* Put root on stack */
    p.stack[0] = make_obj((Obj*)head);
    p.sp = 1;

    printf("Starting GC on deep object graph...\n");
    gc_collect();
    printf("GC completed successfully.\n");

    return 0;
}
