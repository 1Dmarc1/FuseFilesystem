#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "Bitmap.h"
#include "FileSystem.h"
#include "Directory.h"
#include "Util.h"


char delim = '/';

i_node_number_t find_free_i_node(filesystem_t *filesystem) {
    bitmap_t *bitmap = filesystem->bitmap;
    unsigned int index = 0;

    while (bitmap[index] == 255) {
        index++;
    }

    index *= 8;
    index += get_first_free_bit(&bitmap[index]);

    return index;
}

i_node_t *get_i_node_from_path(filesystem_t *filesystem, const char *absolute_path) {
    char *root_s = "/";
    if (strcmp(absolute_path, root_s) == 0 || strlen(absolute_path) == 0) {
        return filesystem->root;
    }

    //Get the positions of the delimiters within the path
    int delim_occurrences = count_occurrences(absolute_path, delim);
    int token_pos[delim_occurrences];
    tokenize(absolute_path, delim, token_pos);

    i_node_t *current = filesystem->root;
    int depth = 0;

    while (depth != delim_occurrences) {
        char absolute_copy[strlen(absolute_path) + 1]; // +1 for null terminator
        strcpy(absolute_copy, absolute_path);

        absolute_copy[token_pos[depth + 1 < delim_occurrences ? depth + 1 : depth]] = '\0';
        int directory_name_length = strlen(absolute_copy + token_pos[depth] + 1) + 1;
        char directory_name[directory_name_length];
        strcpy(directory_name, (absolute_copy) + 1 + token_pos[depth]);
        absolute_copy[token_pos[depth + 1 < delim_occurrences ? 1 : 0]] = delim;

        directory_entry_t *entries = current->data;
        int entries_count = current->data_size / sizeof(directory_entry_t);

        //Loop through all directory entries of the current i_node
        bool found = false;
        for (int i = 0; i < entries_count; i++) {
            if (entries[i].i_node_number != -1 && strcmp(entries[i].name, directory_name) == 0) {
                current = get_i_node(filesystem, entries[i].i_node_number);
                found = true;
                break;
            }
        }

        if (!found) {
            return NULL;
        }

        depth++;

    }
    return current;
}

i_node_t *get_i_node(filesystem_t *filesystem, i_node_number_t i_node_number) {
    if (i_node_number >= 0 && i_node_number < I_NODES && test_bit(filesystem->bitmap, i_node_number)) {
        return &filesystem->nodes[i_node_number];
    } else {
        return NULL;
    }
}


void
add_entry_to_directory(filesystem_t *filesystem, i_node_number_t directory, char *name, i_node_number_t i_node_number) {
    i_node_t *directory_node = get_i_node(filesystem, directory);

    //Increase the data size for the directory
    directory_node->data_size += sizeof(directory_entry_t);
    directory_node->data = realloc(directory_node->data, directory_node->data_size);

    //Add a new entry to the directory
    directory_entry_t *entries = directory_node->data;
    directory_entry_t *new_entry = &entries[(directory_node->data_size / sizeof(directory_entry_t)) - 1];
    new_entry->i_node_number = i_node_number;
    strcpy(new_entry->name, name);
}

void remove_entry_from_directory() {

}

/**
 * Retrieves the i_node_number of the parent directory for a given path.
 * @param filesystem filesystem
 * @param absolute_filepath The absolute filepath to the i_node
 * @return parent directory i_node_number
 */
i_node_number_t get_parent_directory(filesystem_t *filesystem, const char *absolute_filepath) {
    //Get the positions of the delimiter
    int delim_occurrences = count_occurrences(absolute_filepath, delim);
    int token_pos[delim_occurrences];
    tokenize(absolute_filepath, delim, token_pos);

    //Retrieve directory
    char directory_path[strlen(absolute_filepath) + 1];
    strcpy(directory_path, absolute_filepath);
    directory_path[token_pos[delim_occurrences - 1]] = '\0';
    i_node_t *directory_node = get_i_node_from_path(filesystem, directory_path);

    return directory_node->i_node_number;
}

//Deletion of i_nodes

int delete_i_node(filesystem_t *filesystem, const char *absolute_filepath) {
    i_node_t *i_node = get_i_node_from_path(filesystem, absolute_filepath);

    if (i_node == NULL) {
        return -ENOENT;
    }

    filesystem->i_node_count--;
    free(i_node->data);
    clear_bit(filesystem->bitmap, i_node->i_node_number);

    i_node_t *parent_directory = get_i_node(filesystem, get_parent_directory(filesystem, absolute_filepath));

    directory_entry_t *entries = parent_directory->data;
    int entry_count = parent_directory->data_size / sizeof(directory_entry_t);

    int delim_occurrences = count_occurrences(absolute_filepath, delim);
    int delim_pos[delim_occurrences];
    tokenize(absolute_filepath, delim, delim_pos);

    for (int i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].name, absolute_filepath + delim_pos[delim_occurrences - 1] + 1) == 0) {
            entries[i].i_node_number = -1;
            break;
        }
    }
    return 0;

}

int delete_directory(filesystem_t *filesystem, const char *absolute_filepath) {
    i_node_t *i_node = get_i_node_from_path(filesystem, absolute_filepath);

    if (i_node == NULL) {
        return -ENOENT;
    }

    if (i_node->type != TYPE_DIRECTORY) {
        return -ENOTDIR;
    }

    return delete_i_node(filesystem, absolute_filepath);
}


//Creation of i_nodes

int create_i_node(filesystem_t *filesystem, i_node_type type, mode_t mode, const char *absolute_filepath) {

    //Todo: Check if i_node already exists

    i_node_number_t i_node_number = find_free_i_node(filesystem);

    //Set the bit in the bitmap to mark the i_node as used
    set_bit(filesystem->bitmap, i_node_number);

    //Increment the number of i_nodes stored in the filesystem
    filesystem->i_node_count++;

    //Tokenize the path
    int delim_occurrences = count_occurrences(absolute_filepath, delim);
    int token_pos[delim_occurrences];
    tokenize(absolute_filepath, delim, token_pos);

    //Retrieve directory in which i_node is created
    i_node_t *directory_node = get_i_node(filesystem, get_parent_directory(filesystem, absolute_filepath));


    //Increase the data size for the parent directory
    directory_node->data_size += sizeof(directory_entry_t);
    directory_node->data = realloc(directory_node->data, directory_node->data_size);

    //Add a new entry to the directory
    directory_entry_t *entries = directory_node->data;
    directory_entry_t *new_entry = &entries[(directory_node->data_size / sizeof(directory_entry_t)) - 1];
    new_entry->i_node_number = i_node_number;
    strcpy(new_entry->name, absolute_filepath + token_pos[delim_occurrences - 1] + 1);

    //Initialize the new i_node
    i_node_t *i_node = get_i_node(filesystem, i_node_number);

    i_node->type = type;
    i_node->mode = mode;
    i_node->i_node_number = i_node_number;

    i_node->data_size = 0;
    i_node->data = NULL;

    return 0;
}

int create_directory(filesystem_t *filesystem, const char *absolute_path, mode_t mode) {
    if (get_i_node_from_path(filesystem, absolute_path) == NULL) {
        return create_i_node(filesystem, TYPE_DIRECTORY, mode, absolute_path);
    } else {
        return -EEXIST;
    }
}

int create_file(filesystem_t *filesystem, const char *absolute_path, mode_t mode) {
    if (get_i_node_from_path(filesystem, absolute_path) == NULL) {
        return create_i_node(filesystem, TYPE_FILE, mode, absolute_path);
    } else {
        return -1;
    }
}