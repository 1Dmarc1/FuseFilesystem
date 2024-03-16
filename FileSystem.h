#ifndef FUSEFILESYSTEM_FILESYSTEM_H
#define FUSEFILESYSTEM_FILESYSTEM_H

#include <stdbool.h>

#include "INode.h"
#include "Bitmap.h"


#define I_NODES 2048

typedef struct {
    unsigned int i_node_count;

    i_node_t* root;

    i_node_t nodes[I_NODES];
    bitmap_t bitmap[I_NODES];
    unsigned int min_index;

} filesystem_t;

i_node_number_t find_free_i_node(filesystem_t *filesystem);

bool remove_i_node(i_node_number_t i_node_number);

i_node_t *get_i_node(filesystem_t *filesystem, i_node_number_t i_node_number);

i_node_t *get_i_node_from_path(filesystem_t *filesystem, const char *absolute_path);

int create_directory(filesystem_t *filesystem, const char *absolute_path, mode_t mode);

i_node_number_t create_i_node(filesystem_t *filesystem, i_node_type type, mode_t mode, const char *absolute_filepath);

int create_file(filesystem_t *filesystem, const char *absolute_path, mode_t mode);

i_node_number_t get_parent_directory(filesystem_t *filesystem, const char *absolute_filepath);

int delete_directory(filesystem_t *filesystem, const char *absolute_filepath);


#endif //FUSEFILESYSTEM_FILESYSTEM_H
