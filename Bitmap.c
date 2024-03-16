#include "Bitmap.h"

//Get the first set bit in the bitmap
int8_t get_first_set_bit(bitmap_t *bitmap) {
    for (int8_t pos = 0; pos < 8; pos++) {
        if (test_bit(bitmap, pos)) {
            return pos;
        }
    }
    return -1;
}

//Get the first free bit in the bitmap
int8_t get_first_free_bit(bitmap_t *bitmap) {
    for (int8_t pos = 0; pos < 8; pos++) {
        if (!test_bit(bitmap, pos)) {
            return pos;
        }
    }
    return -1;
}

// Set a bit in the bitmap
void set_bit(bitmap_t *bitmap, unsigned int pos) {
    bitmap[pos / 8] |= (1 << (pos % 8));
}

// Clear a bit in the bitmap
void clear_bit(bitmap_t *bitmap, unsigned int pos) {
    bitmap[pos / 8] &= ~(1 << (pos % 8));
}

// Test if a bit is set in the bitmap
int8_t test_bit(bitmap_t *bitmap, unsigned int pos) {
    return (bitmap[pos / 8] >> (pos % 8)) & 1;
}