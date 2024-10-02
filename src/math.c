#include "defs.h"

vector2_t Vector2(int32_t *array) {
    if (sizeof(array) / sizeof(uint32_t) != 2) {
        printf("Array to large/small for vector2 type");
        exit(-1);
    } else {
        vector2_t v;
        v.x = array[0];
        v.y = array[1];

        return v;
    }
}

bool Vector2_is_equal(vector2_t v1, vector2_t v2) {
    bool is_equal;
    if (v1.x == v2.x) {
        if (v1.y == v2.y) {
            is_equal = true;
        } else {
            is_equal = false;
        }
    } else {
        is_equal = false;
    }
    return is_equal;
}