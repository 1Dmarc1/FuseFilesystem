#include <stdio.h>
#include <errno.h>

#include "FileSystem.h"
#include "Directory.h"

#define FUSE_USE_VERSION 30

#include <fuse.h>

static filesystem_t filesystem;


int debug(const char *s, const char *arg) {
    FILE *file;
    char *filename = "/home/daniel/CLionProjects/FuseFileSystem_2/log.txt";

    // Open file in write mode ("w")
    file = fopen(filename, "a");

    // Check if file was opened successfully
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    // Write data to the file using fprintf
    fprintf(file, "%s %s\n", s, arg);

    // Close the file
    fclose(file);

    return 0;
}

void setup_filesystem() {
    filesystem.i_node_count = 0;
    filesystem.min_index = 0;

    filesystem.root = &filesystem.nodes[0];
    i_node_t *root = &filesystem.nodes[0];
    root->data = NULL;
    root->data_size = 0;
    root->type = TYPE_DIRECTORY;
    root->i_node_number = 0;

    for (int i = 0; i < I_NODES; i++) {
        filesystem.bitmap[i] = 0;
    }

    set_bit(filesystem.bitmap, 0);
}

static int perform_mkdir(const char *path, mode_t mode) {
    return create_directory(&filesystem, path, mode);
}

static int perform_getattr(const char *path, struct stat *stateBuff) {
    i_node_t *node = get_i_node_from_path(&filesystem, path);

    if (node == NULL) {
        return -ENOENT;
    }

    if (node->type == TYPE_DIRECTORY) {
        stateBuff->st_mode = S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO;
    } else if (node->type == TYPE_FILE) {
        stateBuff->st_mode = S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO;
    } else if (node->type == TYPE_SYMLINK) {
        stateBuff->st_mode = S_IFLNK | 0777;
    }
    stateBuff->st_size = node->data_size;
    stateBuff->st_gid = node->group_id;
    stateBuff->st_uid = node->owner_id;

    stateBuff->st_mtim = node->mtime;
    stateBuff->st_ctim = node->ctime;
    stateBuff->st_atim = node->atime;

    return 0;
}

static int
perform_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    i_node_t *node = get_i_node(&filesystem, fi->fh);

    if (node == NULL) {
        return -ENOENT;
    }

    if (node->type != TYPE_DIRECTORY) {
        return -ENOTDIR;
    }

    int entry_count = node->data_size / sizeof(directory_entry_t);
    directory_entry_t *entries = node->data;

    for (int i = 0; i < entry_count; i++) {
        if (entries[i].i_node_number >= 0) {
            filler(buf, entries[i].name, NULL, 0);
        }

    }
    return 0;
}

static int perform_mknod(const char *path, mode_t mode, dev_t rdev) {
    return create_file(&filesystem, path, mode);
}

static int perform_rmdir(const char *path) {
    return delete_directory(&filesystem, path);
}

static int perform_open(const char *path, struct fuse_file_info *fi) {

    i_node_t *i_node = get_i_node_from_path(&filesystem, path);

    if (i_node == NULL) {
        return -ENOENT;
    }

    if (i_node->type != TYPE_FILE) {
        return -EPERM;
    }

    fi->fh = i_node->i_node_number;
    return 0;
}

static int perform_opendir(const char *path, struct fuse_file_info *fi) {
    i_node_t *node = get_i_node_from_path(&filesystem, path);

    if (node == NULL) {
        return -ENOENT;
    }

    if (node->type != TYPE_DIRECTORY) {
        return -ENOTDIR;
    }

    fi->fh = node->i_node_number;
    return 0;
}

static struct fuse_operations operations = {
        .mkdir = perform_mkdir,
        .getattr = perform_getattr,
        .readdir = perform_readdir,
        .mknod = perform_mknod,
        .rmdir = perform_rmdir,
        .open = perform_open,
        .opendir = perform_opendir,
};

int main(int argc, char *argv[]) {
    setup_filesystem();

    return fuse_main(argc, argv, &operations, NULL);
}
