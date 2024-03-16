#ifndef FUSEFILESYSTEM_BITMAP_H
#define FUSEFILESYSTEM_BITMAP_H

#include <stdint.h>

typedef uint8_t bitmap_t;

int8_t get_first_set_bit(bitmap_t *bitmap);

int8_t get_first_free_bit(bitmap_t *bitmap);

void set_bit(bitmap_t *bitmap, unsigned int pos);

void clear_bit(bitmap_t *bitmap, unsigned int pos);

int8_t test_bit(bitmap_t *bitmap, unsigned int pos);

#endif //FUSEFILESYSTEM_BITMAP_H
