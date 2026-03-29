#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
//буферизированный ввод/вывод удобен тем что подходит для большого объема данных
int main() {  
    int fd = open("test_2.txt", O_RDWR | O_CREAT,0666); //открытие запись/чтение или создание  с правами запись/чтение
    char str[] = "String from file";
    char cym[1];
    if(fd == -1)
    {
   		perror("error print file");
     	return 1;
    }
    
    ssize_t written = write(fd, str, sizeof(str)-1); //запись без учета завершающего нуля
       if (written == -1) {
           printf("Ошибка при записи в файл!\n");
           close(fd);
           return 1;
       }
    long file_size = lseek(fd, 0, SEEK_END); //определение размера файла
    while(file_size != -1)
    {
    	file_size--;
     	lseek(fd, file_size, SEEK_SET);// перемещаемся каждый раз на 1 шаг меньше к началу
    	int read_bites = read(fd,cym, 1); //читаем по 1 символу
    	if(read_bites == -1)
     	{
     		perror("error read");
      		close(fd);
      		return 1;
      	}
     	printf("%c", cym[0]);
    }
    
    
    
    close(fd);
    return 0;
}