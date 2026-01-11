#include "value.h"

/* Create an integer value */
Value make_int(int32_t x) {
    Value v;
    v.type = VAL_INT;
    v.int_val = x;
    return v;
}

/* Create an object reference value */
Value make_obj(Obj* o) {
    Value v;
    v.type = VAL_OBJ;
    v.obj = o;
    return v;
}
