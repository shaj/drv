/* 
 *  print_string.c - отправляет вывод на tty терминала, независимо от того 
 *  X11 это, или telnet, или что-то еще. Делается это путем вывода строки на tty,
 *  ассоциированный с текущим процессом.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>   /* определение current */
#include <linux/tty.h>     /* определение tty */
#include <linux/version.h> /* макрос LINUX_VERSION_CODE */
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Peter Jay Salzman");
 
static void print_string(char *str)
{
  struct tty_struct *my_tty;
 
  /* 
   * Начиная с версии 2.6.6, структура tty перекочевала в структуру signal 
   */
#if ( LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,5) )
  /* 
   * tty текущего процесса
   */
  my_tty = current->tty;
#else
  /* 
   * tty текущего процесса, для ядер 2.6.6 и выше
   */
  my_tty = current->signal->tty;
#endif
 
  /* 
   * если my_tty == NULL, то текущий процесс не имеет tty на который можно 
   * было бы что нибудь вывести (например, демон).  
   * В этом случае нам ничего не нужно делать.
   */
  if (my_tty != NULL) {
 
    /* 
     * my_tty->driver -- структура, которая хранит указатели на функции-обработчики,
     * одна из которых (write) используется для вывода строк на tty. 
     *
     * Первый параметр функции -- tty, на который осуществляется вывод,
     * поскольку эта функция обычно используется для вывода на все 
     * tty одного и того же типа. 
     * Второй параметр -- флаг расположения строки
     * если строка находится в пространстве ядра, флаг равен false (0)
     * если в пространстве пользователя, то true (не ноль).  
     * Третий параметр -- указатель на строку.
     * Четвертый параметр -- длина строки.
     */
    ((my_tty->driver)->write) (my_tty,  /* Собственно tty  */
             0,                         /* Строка в пространстве ядра */
             str,                       /* Сама строка */
             strlen(str));              /* Длина строки */
 
    /* 
     * tty изначально был аппаратным устройством, который (обычно) 
     * ограничивался стандартом ASCII, в котором перевод строки
     * включал в себя два символа -- "возврат каретки" и "перевод строки".
     * В Unix, символ ASCII -- "перевод строки" заменял оба этих символа,
     * поэтому нам придется использовать для перевода строки
     * оба символа.
     *
     * Это одна из причин различий между текстовыми файлами Unix и
     * MS Windows. CP/M и ее "наследницы", например MS-DOS и
     * MS Windows, строго придерживались стандарта ASCII.
     */
    ((my_tty->driver)->write) (my_tty, 0, "\015\012", 2);
  }
}
 
static int __init print_string_init(void)
{
  print_string("The module has been inserted.  Hello world!");
  return 0;
}
 
static void __exit print_string_exit(void)
{
  print_string("The module has been removed.  Farewell world!");
}
 
module_init(print_string_init);
module_exit(print_string_exit);
 