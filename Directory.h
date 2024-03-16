#ifndef FUSEFILESYSTEM_DIRECTORY_H
#define FUSEFILESYSTEM_DIRECTORY_H

#include "INode.h"

typedef struct {
    char name[255];
    i_node_number_t i_node_number;
} directory_entry_t;

#endif //FUSEFILESYSTEM_DIRECTORY_H
