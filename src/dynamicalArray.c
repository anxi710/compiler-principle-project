#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "include/dynamicalArray.h"

struct DynamicalArray*
newDynamicalArray(int dataTypeSize) {
    struct DynamicalArray* array = (struct DynamicalArray*)malloc(sizeof(struct DynamicalArray));
    assert(NULL != array);

    array->data         = malloc(DYNAMICAL_ARRAY_INIT_CAPACITY * dataTypeSize);
    assert(NULL != array->data);

    array->size         = 0;
    array->capacity     = DYNAMICAL_ARRAY_INIT_CAPACITY;
    array->dataTypeSize = dataTypeSize;

    return array;
}

void
freeDynamicalArray(struct DynamicalArray* array) {
    if (NULL == array) {
        return;
    }

    if (NULL != array->data) {
        free(array->data);
    }
    free(array);
}

void
addElement(struct DynamicalArray* array, void* element) {
    assert(NULL != element);
    assert(NULL != array);

    if (array->size >= array->capacity) {
        array->capacity *= 2; // 每次扩展两倍从而实现摊销
        array->data = realloc(array->data, array->dataTypeSize * array->capacity);
        assert(NULL != array->data);
    }
    memcpy((char*)array->data + (array->size - 1) * array->dataTypeSize, element, array->dataTypeSize);
    array->size++;
}

void*
getElement(struct DynamicalArray* array, int index) {
    assert(NULL != array);
    assert(index >= 0 && index < array->size);

    return (char*)array->data + index * array->dataTypeSize;
}
