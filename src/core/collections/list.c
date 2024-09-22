#include "list.h"

void* __list_create(u64 length, u64 stride) {
    u64 header_size = LIST_FIELD_LENGTH * sizeof(u64);
    u64 list_size = length * stride;
    u64* new_list = GDF_Malloc(header_size + list_size, GDF_MEMTAG_LIST);
    new_list[LIST_CAPACITY] = length;
    new_list[LIST_LENGTH] = 0;
    new_list[LIST_STRIDE] = stride;
    return (void*)(new_list + LIST_FIELD_LENGTH);
}

void __list_destroy(void* list) {
    u64* header = (u64*)list - LIST_FIELD_LENGTH;
    GDF_Free(header);
}

u64 __list_field_get(void* list, u64 field) {
    u64* header = (u64*)list - LIST_FIELD_LENGTH;
    return header[field];
}

void __list_field_set(void* list, u64 field, u64 value) {
    u64* header = (u64*)list - LIST_FIELD_LENGTH;
    header[field] = value;
}

void* __list_resize(void* list) {
    u64 length = GDF_LIST_GetLength(list);
    u64 stride = GDF_LIST_GetStride(list);
    void* new = __list_create(
        (LIST_RESIZE_FACTOR * GDF_LIST_GetCapacity(list)),
        stride);
    GDF_MemCopy(new, list, length * stride);

    GDF_LIST_SetLength(new, length);
    __list_destroy(list);
    return new;
}

void* __list_push(void* list, const void* value_ptr) {
    u64 length = GDF_LIST_GetLength(list);
    u64 stride = GDF_LIST_GetStride(list);
    if (length >= GDF_LIST_GetCapacity(list)) {
        list = __list_resize(list);
    }

    u64 addr = (u64)list;
    addr += (length * stride);
    GDF_MemCopy((void*)addr, value_ptr, stride);
    GDF_LIST_SetLength(list, length + 1);
    return list;
}

void __list_pop(void* list, void* dest) {
    u64 length = GDF_LIST_GetLength(list);
    u64 stride = GDF_LIST_GetStride(list);
    // because arithmetic on void pointers is
    // quite diabolical
    u64 addr = (u64)list;
    addr += ((length - 1) * stride);
    GDF_MemCopy(dest, (void*)addr, stride);
    GDF_LIST_SetLength(list, length - 1);
}

void* __list_remove_at(void* list, u64 index, void* dest) {
    u64 length = GDF_LIST_GetLength(list);
    u64 stride = GDF_LIST_GetStride(list);
    if (index >= length) {
        LOG_ERR("Invalid index. List len: %i, index: %i", length, index);
        return list;
    }

    u64 addr = (u64)list;
    if (dest != NULL)
        GDF_MemCopy(dest, (void*)(addr + (index * stride)), stride);

    // If not on the last element, snip out the entry and copy the rest inward.
    if (index != length - 1) {
        GDF_MemCopy(
            (void*)(addr + (index * stride)),
            (void*)(addr + ((index + 1) * stride)),
            stride * (length - index));
    }

    GDF_LIST_SetLength(list, length - 1);
    return list;
}

void* __list_insert_at(void* list, u64 index, void* value_ptr) {
    u64 length = GDF_LIST_GetLength(list);
    u64 stride = GDF_LIST_GetStride(list);
    if (index >= length) {
        LOG_ERR("Invalid index. List len: %i, index: %index", length, index);
        return list;
    }
    if (length >= GDF_LIST_GetCapacity(list)) {
        list = __list_resize(list);
    }

    u64 addr = (u64)list;

    // If not on the last element, copy the rest outward.
    if (index != length - 1) {
        GDF_MemCopy(
            (void*)(addr + ((index + 1) * stride)),
            (void*)(addr + (index * stride)),
            stride * (length - index));
    }

    // Set the value at the index
    GDF_MemCopy((void*)(addr + (index * stride)), value_ptr, stride);

    GDF_LIST_SetLength(list, length + 1);
    return list;
}