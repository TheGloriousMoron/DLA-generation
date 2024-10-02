#include "defs.h"

vector2_t Vector2_arr(int32_t *array) {
    if (sizeof(array) / sizeof(uint32_t) != 2) {
        printf("Array to large/small for vector2 type\n");
        exit(-1);
    } else {
        vector2_t v;
        v.x = array[0];
        v.y = array[1];

        return v;
    }
}

vector2_t Vector2_int(int32_t x, int32_t y) {
    vector2_t v = {x, y};
    return v;
}

bool Vector2_is_equal(vector2_t v1, vector2_t v2) {
    return (v1.x == v2.x && v1.y == v2.y);
}
