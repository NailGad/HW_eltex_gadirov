#ifndef LIST_H
#define LIST_H

#define P 10  // Длина строк для имени, фамилии и телефона

// структура абонента 
typedef struct abonent
{
    char name[P];
    char second_name[P];
    char tel[P];
    struct abonent* prev;
    struct abonent* next;
} abonent;

typedef struct list
{
    abonent* head;
    abonent* tail;
    int size;    
} list;

// функций
list* new_list();
void add(list *lis);
void vivod(list *lis);
void del(list* lis);
int sravnenie(abonent* cur, char pod[]);
void search(list* lis, char name[]);
void free_list(list* lis);

#endif