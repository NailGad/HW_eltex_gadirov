#include <stdio.h>
#include <dlfcn.h>

typedef int (*math_func)(int, int);

int main() {
    void *handle;
    math_func funcs[4];  // Простой массив указателей
    char *func_names[] = {"add", "sub", "mul", "div"};
    
    // Загружаем библиотеку
    handle = dlopen("./libmath.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Ошибка загрузки: %s\n", dlerror());
        return 1;
    }
    
    // Получаем все функции
    for (int i = 0; i < 4; i++) {
        dlerror();
        funcs[i] = (math_func)dlsym(handle, func_names[i]);
        if (dlerror()) {
            fprintf(stderr, "Ошибка получения %s\n", func_names[i]);
            dlclose(handle);
            return 1;
        }
    }
    
    while(1) {
        int menu, a, b;
        
        printf("\nVibor:\n1) Add\n2) Sub\n3) Mul\n4) Div\n5) Exit\n");
        printf("Choice: ");
        scanf("%d", &menu);
        
        if (menu == 5) {
            dlclose(handle);
            printf("Bye!\n");
            return 0;
        }
        
        if (menu < 1 || menu > 4) {
            printf("error\n");
            continue;
        }
        
        printf("a = ");
        scanf("%d", &a);
        printf("b = ");
        scanf("%d", &b);
        
        printf("Result = %d\n", funcs[menu-1](a, b)); 
    }
}