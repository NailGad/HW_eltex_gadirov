#include "fm_lib.h"

// Определение типа файла
const char *get_file_type(mode_t mode) {
    if (S_ISREG(mode)) return "file";
    if (S_ISDIR(mode)) return "direct";
    if (S_ISLNK(mode)) return "link";
    if (S_ISCHR(mode)) return "symval";
    if (S_ISBLK(mode)) return "block";
    if (S_ISFIFO(mode)) return "fifo";
    if (S_ISSOCK(mode)) return "socket";
    return "?";
}

// Вспомогательные функции
void swap(File *f1, File *f2) {
    File temp = *f1;
    *f1 = *f2;
    *f2 = temp;
}

// Чтение директории
File *find_files(Panel *panel) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char full_path[1024];
    char time_buf[64];
    char pwd[256];
    int i = 0;
    File *array;
    
    array = panel->files;
    panel->count = 0;
    
    if (array != NULL) {
        free(array);
        array = NULL;
    }
    
    getcwd(pwd, 256);
    
    dir = opendir(pwd);
    if (dir == NULL) {
        perror("Ошибка открытия директории");
        return NULL;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        File *temp = realloc(array, sizeof(File) * ((size_t)i + 1));
        if (temp == NULL) {
            free(array);
            closedir(dir);
            return NULL;
        }
        array = temp;
        snprintf(full_path, sizeof(full_path), "%s/%s", pwd, entry->d_name);
        
        if (lstat(full_path, &file_stat) == -1) {
            perror("Ошибка stat");
            continue;
        }
        
        if (strcmp(entry->d_name, ".") == 0) {
            continue;
        }
        
        strcpy(array[i].path, pwd);
        strcpy(array[i].type, get_file_type(file_stat.st_mode));
        strcpy(array[i].name, entry->d_name);
        
        array[i].links = (int)file_stat.st_nlink;
        array[i].size = (float)file_stat.st_size;
        
        strftime(array[i].time, sizeof(time_buf), "%d %b %Y %H:%M",
                 localtime(&file_stat.st_mtime));
        
        if (strcmp(array[i].name, "..") == 0 && i != 0) {
            swap(&array[i], &array[0]);
        }
        i++;
    }
    
    panel->count = i;
    closedir(dir);
    return array;
}

// Перемещение в директорию
void move_direct(Panel *panel) {
    if (strcmp(panel->files[panel->index].type, "direct") == 0) {
        char pwd[1024];
        snprintf(pwd, sizeof(pwd), "%s/%s", panel->files[panel->index].path,
                 panel->files[panel->index].name);
        if (access(pwd, R_OK | X_OK) != 0) {
            return;
        }
        panel->start = 0;
        
        if (chdir(pwd) != 0) {
            perror("error");
        } else {
            panel->index = 0;
            panel->start = 0;
        }
    }
}

// Сортировка по имени
void sort(Panel *panel) {
    int i, j;
    for (i = 0; i < panel->count - 1; i++) {
        for (j = i + 1; j < panel->count; j++) {
            if (strcasecmp(panel->files[i].name, panel->files[j].name) > 0) {
                swap(&panel->files[i], &panel->files[j]);
            }
        }
    }
}

// Отрисовка панели
void draw(Panel *panel) {
    int max_y, max_x;
    int lines, coun;
    int i;
    char pwd[1024]; 
    
    if (panel->files == NULL || panel->count == 0)
        return;
    
    werase(panel->win);
    
    switch (panel->status) {
    case ACTIVE:
        wbkgd(panel->win, COLOR_PAIR(ACTIVE));
        wattron(panel->win, COLOR_PAIR(OBJ_A));
        wattron(panel->win, A_BOLD);
        mvwprintw(panel->win, 1, 1, "%s", panel->files->path);
        mvwprintw(panel->win, 2, 1, "%-28s %s %28s", "name", "time", "size");
        wattroff(panel->win, A_BOLD);
        wattroff(panel->win, COLOR_PAIR(OBJ_A));
        break;
    case INACTIVE:
        wbkgd(panel->win, COLOR_PAIR(INACTIVE));
        wattron(panel->win, A_BOLD);
        mvwprintw(panel->win, 1, 1, "%s", panel->files->path);
        mvwprintw(panel->win, 2, 1, "%-28s %s %28s", "name", "time", "size");
        wattroff(panel->win, A_BOLD);
        break;
    }
    
    box(panel->win, '|', '-');
    
    getmaxyx(panel->win, max_y, max_x);
    
    lines = max_y - HEADER_HEIGHT;
    coun = (panel->count < lines) ? panel->count : lines;
    
    for (i = 0; i < coun; i++) {
        int ix = panel->start + i;
        if (ix >= panel->count)
            break;
            
        if (panel->status == ACTIVE && strcmp(panel->files[ix].type, "direct") == 0) {
            wattron(panel->win, COLOR_PAIR(DIRECT));
        }
        
        snprintf(pwd, sizeof(pwd), "%s/%s", panel->files[ix].path,
                 panel->files[ix].name);
                 
        if (panel->status == ACTIVE && access(pwd, R_OK | X_OK) != 0 &&
            strcmp(panel->files[ix].type, "direct") == 0) {
            wattron(panel->win, COLOR_PAIR(ADMN));
        }
        
        if (panel->status == ACTIVE && panel->index == ix) {
            wattron(panel->win, COLOR_PAIR(CURSOR));
            mvwchgat(panel->win, i + 3, 1, max_x - 2, A_NORMAL, CURSOR, NULL);
        }
        
        if (strlen(panel->files[ix].name) > 20) {
            if (strcmp(panel->files[ix].type, "direct") == 0) {
                mvwprintw(panel->win, i + 3, 1, "%.20s%-8s %-28s %-4s",
                          panel->files[ix].name, "...", panel->files[ix].time, "DIR");
            } else {
                mvwprintw(panel->win, i + 3, 1, "%.20s%-8s %-22s %9.1f kb",
                          panel->files[ix].name, "...", panel->files[ix].time,
                          ((double)panel->files[ix].size / 1024.0));
            }
        } else {
            if (strcmp(panel->files[ix].type, "direct") == 0) {
                mvwprintw(panel->win, i + 3, 1, "%-28s %-28s %-4s",
                          panel->files[ix].name, panel->files[ix].time, "DIR");
            } else {
                mvwprintw(panel->win, i + 3, 1, "%-28s %-24s %7.1f kb",
                          panel->files[ix].name, panel->files[ix].time,
                          ((double) panel->files[ix].size / 1024.0));
            }
        }
        
        if (panel->status == ACTIVE && strcmp(panel->files[ix].type, "direct") == 0) {
            wattroff(panel->win, COLOR_PAIR(DIRECT));
        }
        if (panel->status == ACTIVE && access(pwd, R_OK | X_OK) != 0 &&
            strcmp(panel->files[ix].type, "direct") == 0) {
            wattroff(panel->win, COLOR_PAIR(ADMN));
        }
        if (panel->status == ACTIVE && panel->index == ix) {
            wattroff(panel->win, COLOR_PAIR(CURSOR));
        }
    }
    wrefresh(panel->win);
}