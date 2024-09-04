#pragma once

#include "core.h"

typedef struct Node {

} Node;

typedef struct GDF_HashMap_T {
    u16 stride;
    Node* node_ll;
} GDF_HashMap_T;

typedef struct GDF_HashMap_T* GDF_HashMap;

// The key and value are memcpy'd on insertion. 
void GDF_HashmapInsert(GDF_HashMap hashmap, void* key, void* value);

void* GDF_HashmapGet(GDF_HashMap hashmap, void* key);
void GDF_HashmapRemove(GDF_HashMap hashmap, void* key);