
#ifndef FUSEFILESYSTEM_UTIL_H
#define FUSEFILESYSTEM_UTIL_H

int count_occurrences(const char *string, char c);

struct tokenize_res {
    int res_length;
    char **res;
};

void tokenize(const char *string, char delimiter, int res[]);

#endif //FUSEFILESYSTEM_UTIL_H
