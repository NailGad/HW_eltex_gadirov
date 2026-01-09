#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int IsPassOk(void);
int main(void)
{
    int PwStatus;
    puts("Enter password:");
    PwStatus = IsPassOk();
    if (PwStatus == 0)
    {
        printf("Bad password!\n");
        exit(1);
    }
    else
    {
        printf("Access granted!\n"); // Строка для которой нужно выяснить адрес
    }
    return 0;
}

int IsPassOk(void)
{
    char Pass[12];
    gets(Pass); 	
    return 0 == strcmp(Pass, "test");
}

// gdb) disas main
// Dump of assembler code for function main:
//    0x00005555555551c9 <+0>:	endbr64
//    0x00005555555551cd <+4>:	push   %rbp
//    0x00005555555551ce <+5>:	mov    %rsp,%rbp
//    0x00005555555551d1 <+8>:	sub    $0x10,%rsp
//    0x00005555555551d5 <+12>:	lea    0xe28(%rip),%rax        # 0x555555556004
//    0x00005555555551dc <+19>:	mov    %rax,%rdi
//    0x00005555555551df <+22>:	call   0x555555555090 <puts@plt>
//    0x00005555555551e4 <+27>:	call   0x555555555221 <IsPassOk>
//    0x00005555555551e9 <+32>:	mov    %eax,-0x4(%rbp)
//    0x00005555555551ec <+35>:	cmpl   $0x0,-0x4(%rbp)
//    0x00005555555551f0 <+39>:	jne    0x55555555520b <main+66>
//    0x00005555555551f2 <+41>:	lea    0xe1b(%rip),%rax        # 0x555555556014
//    0x00005555555551f9 <+48>:	mov    %rax,%rdi
//    0x00005555555551fc <+51>:	call   0x555555555090 <puts@plt>
//    0x0000555555555201 <+56>:	mov    $0x1,%edi
//    0x0000555555555206 <+61>:	call   0x5555555550d0 <exit@plt>
// => 0x000055555555520b <+66>:	lea    0xe10(%rip),%rax        # 0x555555556022
//    0x0000555555555212 <+73>:	mov    %rax,%rdi
//    0x0000555555555215 <+76>:	call   0x555555555090 <puts@plt>
//    0x000055555555521a <+81>:	mov    $0x0,%eax
//    0x000055555555521f <+86>:	leave
//    0x0000555555555220 <+87>:	ret
// End of assembler dump.
// (gdb) x/s 0x555555556022
// 0x555555556022:	"Access granted!"
// 
// (gdb) find &main, +999999, "Access granted!"
//0x555555556022