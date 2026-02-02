#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "list.h"

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8");  // Установка русской локали
 
    // Создание телефонного справочника
    list* lis = new_list();
    if (lis == NULL) {
        printf("Ошибка создания телефонного справочника!\n");
        return 1;
    }
    
    char vibor;  // Переменная для выбора действия
    while(1)     // Бесконечный цикл меню
    {
        // Вывод меню
        printf("1. Добавить абонента\n");
        printf("2. Удалить абонента\n");
        printf("3. Поиск по имени\n");
        printf("4. Показать всех абонентов\n");
        printf("5. Выйти\n");
        printf("Выберите действие: ");

        vibor = getchar();               // Чтение выбора пользователя
        while (getchar() != '\n')        // Очистка буфера ввода
            ;

        // Обработка выбора пользователя
        switch (vibor)
        {
        case '1':
            add(lis);                    // Добавление абонента
            break;
        case '2':
            del(lis);                    // Удаление абонента
            break;
        case '3':
            // Поиск по имени
            printf("Введите имя для поиска: ");
            char search_name[P];         // Буфер для имени поиска
            scanf("%9s", search_name);   // Чтение имени
            while (getchar() != '\n')    // Очистка буфера
                ;
            search(lis, search_name);    // Вызов функции поиска
            break;
        case '4':
            vivod(lis);                  // Вывод всех абонентов
            break;
        case '5':
            // Выход из программы
            printf("Выход из программы...\n");
            free_list(lis);              // Освобождение памяти
            return 0;
        default:
            printf("Неверный выбор! Попробуйте снова.\n");
            break;
        }
    }
}