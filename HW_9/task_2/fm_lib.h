#ifndef FM_LIB_H
#define FM_LIB_H

#include <dirent.h>
#include <grp.h>
#include <ncurses.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// Константы интерфейса
#define WIN1_H 20
#define WIN1_W 70
#define WIN1_Y 2
#define WIN1_X 2

#define WIN2_H 20
#define WIN2_W 70
#define WIN2_Y 2
#define WIN2_X 75

#define HEADER_HEIGHT 4

// Цветовые схемы
#define ACTIVE 1
#define INACTIVE 2
#define CURSOR 3
#define DIRECT 4
#define OBJ_A 5
#define ADMN 6
#define SORTED 7

// Структуры данных
typedef struct {
    char path[512];   
    char name[256];    
    float size;
    char time[64];     
    int links;
    char type[20];     
} File;

typedef struct {
    WINDOW *win;
    File *files;
    int count;
    int index;
    int status;
    int start;
} Panel;

// Прототипы функций 
const char *get_file_type(mode_t mode);
void swap(File *f1, File *f2);
File *find_files(Panel *panel);
void move_direct(Panel *panel);
void sort(Panel *panel);
void draw(Panel *panel);

#endif