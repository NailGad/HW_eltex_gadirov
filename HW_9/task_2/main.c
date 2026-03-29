#include "fm_lib.h"
#include <unistd.h>

int main(void) {
    // Все объявления переменных в начале функции
    int ch;
    int my;
    int visible_h;
    Panel *left;
    Panel *right;
    
    // Изменяем размер терминала
    printf("\033[8;24;150t");
    fflush(stdout);
    usleep(10000);
    
    // Инициализация ncurses
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    start_color();
    keypad(stdscr, true);
    
    // Инициализация цветов
    init_pair(ACTIVE, COLOR_GREEN, COLOR_BLUE);
    init_pair(INACTIVE, COLOR_CYAN, COLOR_BLACK);
    init_pair(CURSOR, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(DIRECT, COLOR_WHITE, COLOR_BLUE);
    init_pair(OBJ_A, COLOR_YELLOW, COLOR_BLUE);
    init_pair(ADMN, COLOR_RED, COLOR_BLUE);
    init_pair(SORTED, COLOR_BLACK, COLOR_CYAN);
    
    clear();
    refresh();
    
    // Заголовки
    wattron(stdscr, COLOR_PAIR(SORTED));
    mvwprintw(stdscr, 1, 50, "%s", " WITHOUT SORTED ");
    mvwprintw(stdscr, 1, 120, "%s", " SORTED by NAME ");
    wattroff(stdscr, COLOR_PAIR(SORTED));
    refresh();
    
    // Панели
    left = malloc(sizeof(Panel));
    right = malloc(sizeof(Panel));
    
    left->win = newwin(WIN1_H, WIN1_W, WIN1_Y, WIN1_X);
    right->win = newwin(WIN2_H, WIN2_W, WIN2_Y, WIN2_X);
    
    left->count = 0;
    left->index = 0;
    left->status = ACTIVE;
    left->files = NULL;
    left->start = 0;
    
    right->count = 0;
    right->index = 0;
    right->status = INACTIVE;
    right->files = NULL;
    right->start = 0;
    
    // Загрузка файлов
    left->files = find_files(left);
    right->files = find_files(right);
    
    wrefresh(stdscr);
    sort(right);
    draw(left);
    draw(right);
    
    // Главный цикл
    while ((ch = getch()) != 'q') {
        Panel *curr = (left->status == ACTIVE) ? left : right;
        
        switch (ch) {
        case '\t':
            left->status = (left->status == ACTIVE) ? INACTIVE : ACTIVE;
            right->status = (right->status == ACTIVE) ? INACTIVE : ACTIVE;
            break;
        case 10:
            if (curr->count > 0) {
                move_direct(curr);
                curr->files = find_files(curr);
                sort(right);
            }
            break;
        case KEY_UP:
            if (curr->index > 0) {
                curr->index--;
                if (curr->index < curr->start) {
                    curr->start = curr->index;
                }
            }
            break;
        case KEY_DOWN:
            if (curr->index < curr->count - 1) {
                curr->index++;
                my = getmaxy(curr->win);
                visible_h = my - HEADER_HEIGHT;
                if (curr->index >= curr->start + visible_h) {
                    curr->start++;
                }
            }
            break;
        }
        
        draw(left);
        draw(right);
    }
    
    // Очистка
    free(left->files);
    free(right->files);
    free(left);
    free(right);
    endwin();
    return 0;
}