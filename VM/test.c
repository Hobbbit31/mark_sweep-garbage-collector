#include <stdio.h>
#include <stdlib.h>
#include "object.h"
#include "vm.h"
#include "stack.h"

/* Helper to simulate the VM state for GC tests */
void setup_test_vm(Program* p) {
    p->sp = 0;
    p->csp = 0;
    p->instr_count = 0;
    for (int i = 0; i < MEM_SIZE; i++) {
        p->memory[i].type = VAL_NIL;
        p->memory[i].obj = NULL;
    }
    current_program = p;
}

/* --- Test Case Functions --- */

// 1.6.1 & 1.6.2: Basic Reachability & Unreachable Object Collection
void test_reachability(Program* p) {
    printf("\nRunning: Basic Reachability & Unreachable Collection\n");
    ObjPair* a = new_pair(make_obj(NULL), make_obj(NULL));
    vm_push(p, make_obj((Obj*)a));
    // printf("Pushed Object A to stack. Running GC...\n");
    gc_collect();
    
    vm_pop(p);
    printf("Popped Object A. It should now be unreachable.\n");
    gc_collect();
}

// 1.6.3: Transitive Reachability
void test_transitive(Program* p) {
    printf("\nRunning: Transitive Reachability\n");
    ObjPair* a = new_pair(make_obj(NULL), make_obj(NULL));
    ObjPair* b = new_pair(make_obj((Obj*)a), make_obj(NULL));
    vm_push(p, make_obj((Obj*)b));
    // printf("Pushed Object B (which points to A). Both should survive GC.\n");
    gc_collect();
    
    vm_pop(p);
    printf("Popped Object B. Both A and B are now unreachable.\n");
    gc_collect();
}

// 1.6.4: Cyclic References
void test_cycles(Program* p) {
    printf("\nRunning: Cyclic References\n");
    ObjPair* a = new_pair(make_obj(NULL), make_obj(NULL));
    ObjPair* b = new_pair(make_obj((Obj*)a), make_obj(NULL));
    a->right = make_obj((Obj*)b); // Create cycle: A <-> B
    
    vm_push(p, make_obj((Obj*)a));
    // printf("Created cycle A <-> B and pushed A to stack. Both should survive.\n");
    gc_collect();
    
    vm_pop(p);
    printf("Popped A. The isolated cycle should now be collected.\n");
    gc_collect();
}

// 1.6.5: Deep Object Graph (Stress Test)
void test_deep_graph(Program* p) {
    printf("\nRunning: Deep Object Graph Stress Test\n");
    ObjPair* root = new_pair(make_obj(NULL), make_obj(NULL));
    ObjPair* cur = root;
    
    // Create a long chain of objects
    for (int i = 0; i < 100; i++) {
        ObjPair* next = new_pair(make_obj(NULL), make_obj(NULL));
        cur->right = make_obj((Obj*)next);
        cur = next;
    }
    
    vm_push(p, make_obj((Obj*)root));
    // printf("Created chain of 501 objects. Running GC...\n");
    gc_collect();
    
    vm_pop(p);
    printf("Popped root. Entire chain should now be collected.\n");
    gc_collect();
}

// 1.6.6: Closure Capture
void test_closures(Program* p) {
    printf("\nRunning: Closure Capture\n");
    Obj* env = (Obj*)new_pair(make_int(10), make_int(20));
    Obj* fn = new_function();
    Obj* cl = new_closure(fn, env);
    
    vm_push(p, make_obj(cl));
    // printf("Pushed Closure (referencing Function and Env). All should survive.\n");
    gc_collect();
    
    vm_pop(p);
    printf("Popped Closure. Function and Env should now be collected.\n");
    gc_collect();
}

/* --- Interactive Menu --- */

int main() {
    Program prog;
    setup_test_vm(&prog);
    
    int choice;
    while (1) {
        printf("\n--- GC Interactive Test Suite ---\n");
        printf("1. Basic Reachability (1.6.1 & 1.6.2)\n");
        printf("2. Transitive Reachability (1.6.3)\n");
        printf("3. Cyclic References (1.6.4)\n");
        printf("4. Deep Object Graph (1.6.5)\n");
        printf("5. Closure Capture (1.6.6)\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) break;
        if (choice == 0) break;

        switch (choice) {
            case 1: test_reachability(&prog); break;
            case 2: test_transitive(&prog); break;
            case 3: test_cycles(&prog); break;
            case 4: test_deep_graph(&prog); break;
            case 5: test_closures(&prog); break;
            default: printf("Invalid choice.\n");
        }
        
        // Final safety cleanup after each test run
        if (heap_objects != NULL) {
            printf("\nCleaning up remaining heap objects...\n");
            gc_collect();
        }
    }

    return 0;
}