#ifndef FUSEFILESYSTEM_INODE_H
#define FUSEFILESYSTEM_INODE_H

#include <stdlib.h>

typedef int i_node_number_t;

typedef enum {
    TYPE_FILE,
    TYPE_DIRECTORY,
    TYPE_SYMLINK,
} i_node_type;

typedef struct {
    i_node_type type;
    mode_t mode;
    i_node_number_t i_node_number;
    int data_size;
    int owner_id;
    int group_id;

    struct timespec atime;
    struct timespec mtime;
    struct timespec ctime;

    void *data;
} i_node_t;

#endif //FUSEFILESYSTEM_INODE_H
