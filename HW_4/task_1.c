#include <stdio.h>
#include <locale.h>
#define N 100
#define P 10
//структура абонента 
struct abonent
{
    char name[P];
    char second_name[P];
    char tel[P];
};
typedef struct abonent abonent; // чтобы не писать все время struct abonent

//функция для добавления нового абонента 
void add(abonent *abonents, char *count)
{
    //проверка на переполнение массива
    if (*count >= N)
    {
        printf("Нет места!\n");
        return;
    }

    //ввод имени
    printf("Введите имя: ");
    scanf("%9s", abonents[*count].name);
    while (getchar() != '\n') //записываем enter чтобы не мешалсся
        ;
    //ввод фамилии
    printf("Введите фамилию: ");
    scanf("%9s", abonents[*count].second_name);
    while (getchar() != '\n')
        ;
    //ввод телефона
    printf("Введите телефон: ");
    scanf("%9s", abonents[*count].tel);
    while (getchar() != '\n')
        ;
    //счетчик абонента
    (*count)++;
    printf("Абонент добавлен!\n");
}

//функция вывода
void vivod(abonent *abonents, char *count)
{
    //если пусто
    if (*count == 0)
    {
        printf("Справочник пуст!\n");
        return;
    }

    printf("\nАБОНЕНТЫ\n");
    printf("Всего: %d\n\n", *count);
    //вывод справочника
    for (int i = 0; i < *count; i++)
    {
        printf("Абонент №%d:\n", i + 1);
        printf("  Имя:        %s\n", abonents[i].name);
        printf("  Фамилия:    %s\n", abonents[i].second_name);
        printf("  Телефон:    %s\n\n", abonents[i].tel);
    }
}

//функция для смещения абонентов которые идут после удаляемого абонента
void smeshenie(abonent *abonents, char *count, int idx)
{
    //смещаем
    for (int i = idx; i < *count - 1; i++)
    {
        abonents[i] = abonents[i + 1];
    }
    //последний элемент зануляем, это и есть наш удаленный абонент
    for (int i = 0; i < P; i++)
    {
        abonents[*count - 1].name[i] = 0;
        abonents[*count - 1].second_name[i] = 0;
        abonents[*count - 1].tel[i] = 0;
    }
}

//фукция для удаления 
void del(abonent *abonents, char *count)
{
    if (*count == 0)
    {
        printf("Справочник пуст!\n");
        return;
    }

    printf("\nУдаление абонента\n");
    printf("\n   name       sec_name   tel_number\n");
    //вывод всех абонентов
    for (int i = 0; i < *count; i++)
    {
        printf("%d. %-10s %-10s %-10s\n", i + 1, abonents[i].name, abonents[i].second_name, abonents[i].tel);
    }
    //удаление по порядковому номеру
    int num;
    printf("\nВведите номер для удаления (1-%d): ", *count);
    scanf("%d", &num);
    while (getchar() != '\n')
        ;
    //если в пределах
    if (num < 1 || num > *count)
    {
        printf("Неверный номер!\n");
        return;
    }
    //вычесление индекса удаляемого абонента
    int idx = num - 1;
    for (int i = 0; i < P; i++)
    {
        abonents[idx].name[i] = 0;
        abonents[idx].second_name[i] = 0;
        abonents[idx].tel[i] = 0;
    }
    //смещаем 
    smeshenie(abonents, count, idx);
    //вычитаес из счетчика
    (*count)--;
    printf("Абонент удален! Осталось: %d\n", *count);
}

//функция сравнения двух строк
int sravnenie(char stroka[], char pod[])
{

    int i=0;
    char *ptr = NULL;
    int flag = 1; //считаем что нашли
    while(stroka[i] != '\0' || pod[i] != '\0')//до конца одной из строк
    {
        //если не совпало то не то
        if(stroka[i] != pod[i])
        {
            flag = 0;
            return flag;
        }
        i++;
    }

    return flag;
}

//фнкция сравнения
void search(abonent *abonents, char *count, char name[])
{
    if (*count == 0)
    {
        printf("Справочник пуст!\n");
        return;
    }

    for (int i = 0; i < *count; i++)
    {
        //если они одиннаковы
        if (sravnenie(abonents[i].name, name))
        {
            //выводим
            printf("Абонент №%d:\n", i + 1);
            printf("  Имя:        %s\n", abonents[i].name);
            printf("  Фамилия:    %s\n", abonents[i].second_name);
            printf("  Телефон:    %s\n\n", abonents[i].tel);
        }
    }
}

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8");//для вывода кирилицы (не для записи кирилицы)
    //выбор меню и счетчик
    char vibor, count = 0;
    abonent abonents[N] = {0};
    while (1)
    {
        printf("\n1) Добавить подписчика\n");
        printf("2) Удалить подписчика\n");
        printf("3) Выполнить поиск подписчиков по имени\n");
        printf("4) Отобразить все записи\n");
        printf("5) Выйти\n");
        printf("Выберите действие: ");

        vibor = getchar();
        while (getchar() != '\n')
            ;

        switch (vibor)
        {
        case '1':
            add(abonents, &count);
            break;
        case '2':
            del(abonents, &count);
            break;
        case '3':
        //вводд имени для поиска
            printf("введите имя: \n");
            char search_name[P]; //какое имя найти
            scanf("%9s", search_name);
            while (getchar() != '\n')
                ;
            search(abonents, &count, search_name);
            break;
        case '4':
            vivod(abonents, &count);
            break;
        case '5':
            printf("Выход из программы\n");
            return 0;
        default:
            printf("Неверный выбор! Попробуйте снова.\n");
            break;
        }
    }
}