#include <stdio.h>
//буферизированный ввод/вывод удобен тем что подходит для малого объема данных 
int main() {
    FILE *file;
    char str[] = "String from file";
    file = fopen("test_1.txt", "w+"); // открываем запись/чтение
    if(file == NULL)
    {
   		perror("error open");
    	return 1;
    }
    if(fputs(str,file) == EOF)
    {
   		perror("error open");
    	return 1;
    }
    
    long pos = ftell(file)-1; // смещаемся с конца
    while(pos !=-1)
    {
    	fseek(file, pos, SEEK_SET); // смещаемся с начала каждый раз на 1 шаг меньше предыдущего
   		char sim = fgetc(file);
    	printf("%c", sim);
     	pos -= 1;
      	
    }
    fclose(file);
    return 0;
}