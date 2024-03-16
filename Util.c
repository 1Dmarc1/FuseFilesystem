#include <string.h>

#include "Util.h"

int count_occurrences(const char *string, char c) {
    int index = 0;
    int occurrences = 0;
    while (string[index] != '\0') {
        if (string[index] == c) {
            occurrences++;
        }
        index++;
    }
    return occurrences;
}

void tokenize(const char *string, char delimiter, int res[]) {
    int index = 0;
    int res_index = 0;
    while (string[index] != '\0') {
        if (string[index] == delimiter) {
            res[res_index] = index;
            res_index++;
        }
        index++;
    }
}