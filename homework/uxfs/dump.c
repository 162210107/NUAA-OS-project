#include "utils/std.h"
#include "utils/typer.h"
#include "disk.h"
#include "fs.h"
#include "inode.h" 
#include "dir.h"
#include "dump.h"

void dump_regular(inode_t *inode)
{
    /* TODO */
    char* memory;
    memory = (char *)malloc(inode->size + 1);
    typer_dump("reg num: %d\n", inode->size);
    inode_read(inode,0,memory,inode->size);
    memory[inode->size] = '\0';
    typer_dump("reg %s\n", memory);
    free(memory);

}

extern void dump_dir_entry(dir_entry_t *dir_entry);

void dump_dir(inode_t *dir)
{
    typer_dump("dir\n");
    typer_push();

    dir_entry_t *dir_entry;
    dir_cursor_t dir_cursor;
    dir_cursor_init(&dir_cursor, dir);

    /* TODO */
    while (dir_entry = dir_cursor_next(&dir_cursor)) {
        if (dir_entry->ino == FAKE_INO)
            continue;
        dump_dir_entry(dir_entry);
    }

    dir_cursor_destroy(&dir_cursor);
    typer_pop();
}

static int is_dot(char *name)
{
    return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

void dump_dir_entry(dir_entry_t *dir_entry)
{
    char *name = dir_entry->name;
    int ino = dir_entry->ino;
    if (ino == FAKE_INO)
        return;

    typer_dump("entry\n");
    typer_push();

    typer_dump("name %s\n", name);
    typer_dump("ino %d\n", ino);
    // !!!
    if(is_dot(name)) {
        typer_pop();
        return ;
    }
    /* TODO */
    inode_t *dir_inode=fs_seek_inode(dir_entry->ino);
    if(inode_is_dir(dir_inode)) dump_dir(dir_inode);
    if(inode_is_regular(dir_inode)) dump_regular(dir_inode);

    typer_pop();
}

void dump_uxfs(int argc, char *argv[])
{
    if (argc != 2) {
        puts("Usage: dump.uxfs path");
        return;
    }
    char *path = argv[1];

    fs = disk_open(path);
    fs_dump();

    inode_t *root = fs_seek_inode(ROOT_INO);
    dump_dir(root);

    disk_close();
}
