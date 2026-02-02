#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "list.h"

// Создание новой пустой структуры списка
list* new_list()
{
    list* new = (list*)malloc(sizeof(list));
    if (new != NULL) {
        new->head = NULL;    
        new->tail = NULL;    
        new->size = 0;       
    }
    return new;
}

// Добавление нового абонента в список
void add(list *lis)
{
    // Выделение памяти для нового абонента
    abonent* new = (abonent*)malloc(sizeof(abonent));
    if (new == NULL) {
        printf("Ошибка выделения памяти!\n");
        return;
    }
    
    // Ввод данных абонента
    printf("Введите имя: ");
    scanf("%9s", new->name);        // Ограничение ввода до 9 символов
    while (getchar() != '\n');      // Очистка буфера ввода

    printf("Введите фамилию: ");
    scanf("%9s", new->second_name);
    while (getchar() != '\n');

    printf("Введите телефон: ");
    scanf("%9s", new->tel);
    while (getchar() != '\n');
    
    new->next = NULL;
    new->prev = NULL;
    
    // Добавление элемента в список
    if(lis->head == NULL)           // Если список пуст
    {
        lis->head = new;            // Новый элемент становится головой
        lis->tail = new;          
    }
    else                            // Если список не пуст
    {
        new->prev = lis->tail;    
        lis->tail->next = new;    
        lis->tail = new;            
    }
    
    lis->size += 1;                 // Увеличение счетчика 
    printf("Абонент добавлен! Всего абонентов: %d\n", lis->size);
}

// Вывод всех абонентов списка
void vivod(list *lis)
{
    if (lis->size == 0)             // Проверка на пустой список
    {
        printf("Справочник пуст!\n");
        return;
    }

    printf("Всего абонентов: %d\n\n", lis->size);
    
    abonent* abon = lis->head;
    int i = 0;
    while(abon != NULL)
    {
        printf("Абонент №%d:\n", i + 1);
        printf("  Имя:        %s\n", abon->name);
        printf("  Фамилия:    %s\n", abon->second_name);
        printf("  Телефон:    %s\n\n", abon->tel);
        abon = abon->next;        
        i++;
    }
}

// Удаление абонента из списка
void del(list* lis)
{
    if (lis->size == 0)             // Проверка на пустой список
    {
        printf("Справочник пуст!\n");
        return;
    }

    printf("\nУДАЛЕНИЕ АБОНЕНТА\n");
    printf("Список абонентов:\n");
    printf("   Имя        Фамилия    Телефон\n");
    printf("---------------------------------\n");
    
    abonent* current = lis->head;
    int i = 0;
    while(current != NULL)          // Вывод всех элементов
    {
        printf("%d. %-10s %-10s %-10s\n", i + 1, current->name, current->second_name, current->tel);
        current = current->next;
        i++;
    }
    
    // Ввод номера удаляемого элемента
    int num;
    printf("\nВведите номер для удаления (1-%d): ", lis->size);
    scanf("%d", &num);
    while (getchar() != '\n')       // Очистка буфера ввода
        ;
    
    if (num < 1 || num > lis->size) 
    {
        printf("Неверный номер!\n");
        return;
    }
    
    // Поиск удаляемого элемента
    abonent* temp = lis->head;
    for(int i = 1; i < num; i++)
    {
        temp = temp->next;
    }
    
    // Удаление элемента с учетом его положения в списке
    if (temp == lis->head && temp == lis->tail) // Единственный элемент
    {
        lis->head = NULL;
        lis->tail = NULL;
    }
    else if (temp == lis->head)                 // Первый элемент
    {
        lis->head = temp->next;
        if (lis->head != NULL) {
            lis->head->prev = NULL;             
        }
    }
    else if (temp == lis->tail)                 // Последний элемент
    {
        lis->tail = temp->prev;
        if (lis->tail != NULL) {
            lis->tail->next = NULL;             
        }
    }
    else                                        // Элемент в середине
    {
        temp->prev->next = temp->next;          
        temp->next->prev = temp->prev;          
    }
    
    free(temp);              // Освобождение памяти элемента
    lis->size -= 1;          // Уменьшение счетчика
    printf("Абонент удален! Осталось абонентов: %d\n", lis->size);
}

// Сравнение имени абонента с заданной строкой
int sravnenie(abonent* cur, char pod[])
{
    int i = 0;
    // Посимвольное сравнение строк
    while(cur->name[i] != '\0' && pod[i] != '\0')
    {
        if(cur->name[i] != pod[i])
        {
            return 0;        // Строки не совпадают
        }
        i++;
    }
    // Проверка окончания обеих строк
    return cur->name[i] == pod[i];
}

// Поиск абонентов по имени
void search(list* lis, char name[])
{
    if (lis->size == 0)             // Проверка на пустой список
    {
        printf("Справочник пуст!\n");
        return;
    }

    printf("\nПОИСК ПО ИМЕНИ: %s\n", name);
    printf("Результаты:\n");
    
    // Поиск по всему списку
    abonent* cur = lis->head;
    int found = 0;                  // Флаг найденных совпадений
    int i = 0;
    
    while(cur != NULL)
    {
        if (sravnenie(cur, name))   // Сравнение имен
        {
            printf("Абонент №%d:\n", i + 1);
            printf("  Имя:        %s\n", cur->name);
            printf("  Фамилия:    %s\n", cur->second_name);
            printf("  Телефон:    %s\n\n", cur->tel);
            found = 1;             
        }
        cur = cur->next;
        i++;
    }
    
    if (!found) {
        printf("Абоненты с именем '%s' не найдены.\n", name);
    }
}

// Освобождение памяти всего списка
void free_list(list* lis)
{
    if (lis == NULL) return;
    
    // Последовательное удаление всех элементов
    abonent* current = lis->head;
    while (current != NULL) {
        abonent* next = current->next;  
        free(current);                  
        current = next;                 
    }
    free(lis);                          // Освобождение структуры списка
}