#define FUSE_USE_VERSION 26
#define FILE_OFFSET_BITS 64
 
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
 
typedef struct Node* Link; // тип "ссылка на узел"
typedef struct Node {      //  тип "узел"
    char* name;            // имя узла иерархии
    char* content;         // содержимое узла
    Link parent;           //ссылка на "родителя"
    Link* childs;          // массив ссылок на "потомков"
    int childCount;        // число потомков
} Node;
static Link tree;          // корень файловой системы в RAM

static char* tempFile = ""; /* узел с которым осуществляется текущая работа 
- использование глобальной переменной не корректно, так как 
невозможным оказывается, например, работа одновременно с двумя разными файлами. */
static char* tempContent = "";  /* содержимое используемого файла, 
с которым осуществляется взаимодействие - 
опять же не корректно(см. комментарии к объявлению 
предыдущей переменной)
*/

Link createNode(const char* name, const char* content);
char** split(const char* path);
Link skNode(Link tree, const char* path);
Link seekNode(Link tree, const char* path);
char* memcpu(const char* source, const char* buf);
void addNode(Link parent, Link node);
void deleteNode(Link node);

/*следующая функция ищет узел иерархии по заданному пути используя  skNode +
проверку того, что обнаружденный последней функцию элемент действительно 
является конечным элементом пути
возвращает укаатель на найденный узел*/
Link seekNode(Link tree, const char* path) {  // поиск имени в иерархии
    char** splited = split(path); // разбиваем путь на составляющие
    int count = 0;
    int i = 0;
    while(splited[i++] != 0) count++; // узнаем число узлов
    Link node = skNode(tree,path);  
    if (strcmp(node->name,splited[count-1]) != 0) return 0;  // проверка
    return node;
}
 
/* получаем атрибуты файла*/
static int my_getattr(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));
    Link node = seekNode(tree, path);
    if (node == 0) return -ENOENT;  
    if (node->content == 0)
    {
        stbuf->st_mode = S_IFDIR | 0666;
        stbuf->st_nlink = 2;
    } else
    {
        stbuf->st_mode = S_IFREG | 0666;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(node->content);
    }
    return 0; 
}
 
/*Далее получаем сдержимое папки*/
static int my_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi) {
         
    Link node = seekNode(tree, path);   
    int i = 0;
    for(; i < node->childCount; i++)
    {
        filler(buf, node->childs[i]->name, NULL, 0);
    }
    return 0;   
}
 
/*определяем опции открытия файла . здесь -  по умолчанию*/
static int my_open(const char *path, struct fuse_file_info *fi)
{
     
    return 0;
}
 
/*читаем данные из открытого файла*/
static int my_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi)
{       
    Link node = seekNode(tree, path);
    int len = strlen(node->content);
    if (offset < len)
    {
        if (offset + size > len) size = len - offset;
        memcpy(buf, node->content + offset, size);
    } else  size = 0;
    return size;
}
 
/*Предоставляет возможность записать в открытый файл - tempFile
ДАННАЯ РЕАЛИЗАЦИЯ НЕ ДАЁТ ВОЗМОЖНОСТИ ИЗМЕНЯТЬ ФАЙЛ ПОСЛЕ СОЗДАНИЯ -
ТО ЕСТЬ ОНА ОШИБОЧНА - ФАКТИЧЕСКИ НИКАКОЙ ЗАПИСИ НЕ ПРОИСХОДИТ*/
static int my_write(const char *path, const char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi)
{   
    tempFile = (char*)path; /*это единственный момент в данном примере,  
   когда определяется значение tempFile  -при этом 
в строке ниже НЕ ПРОИСХОДИТ ОПРЕДЕЛЕНИЯ ТЕКУЩЕГО КОНТЕНТА ФАЙЛА - 
ЭТО ВЫЗЫВАЕТ РЯД ОШИБОК - НАПРИМЕР ПОСЛЕ КОПИРОВАНИЯ ФАЙЛА -
в  tempContent  останется именно содержимое последнего записанного файла -
а уже не нового*/
    tempContent = memcpu(tempContent, buf + offset); 
    return strlen(buf+offset);
}

 
/*создаём папку*/
static int my_mkdir(const char* path, mode_t mode)
{   
    char** array = split(path);
    int ct = 0; 
    while(array[ct] != 0) ct++;
    Link node = createNode(array[ct - 1], 0); /* создаём узел  - так как эта папка  - то собственных
 данных она не содержит - только потомки.*/
    Link parent = skNode(tree, path);
    addNode(parent, node);
    return 0;       
}
/*создаём узел*/
static int my_mknod(const char* path, mode_t mode, dev_t dev)
{
    char** array = split(path);
    int ct = 0; 
    while(array[ct] != 0) ct++;
    Link node = createNode(array[ct - 1], ""); //создаём узел - в качестве содержимого  -пустая строка.
    Link parent = skNode(tree, path); // ищём родительский узел в иерархии
    addNode(parent, node); // "пристёгиваем" созданный узел к родительскому.
 
    return 0;
}
 
/*переименование*/
static int my_rename(const char* old, const char* new)
{   
    if (strcmp(old,tempFile) == 0)
    {                   
        Link node = seekNode(tree, new);
        node->content = tempContent;
        tempContent = ""; // "обнуляем" текущий контент
        return 0;
    }
    Link node = seekNode(tree, old);
    char** array = split(new);
    int i = 0;
    while(array[i] != 0) i++;
    node->name = array[i - 1];
    return 0;
}
/*удалям папку*/
static int my_rmdir(const char* path) // в данной реализации-  просто удаляем узел
{
    Link node = seekNode(tree, path);
    deleteNode(node);
    return 0;
}
 
/*удаляем файл*/
static int my_unlink(const char* path) //в данной реализации -  просто удаляем узел
{
    Link node = seekNode(tree, path);
    deleteNode(node);
    return 0;
}
 
static struct fuse_operations my_oper =  // структура определённых нами операций
{
    .getattr    = my_getattr,// атрибуты файла
    .readdir    = my_readdir,
    .open       = my_open,
    .read       = my_read,
    .write      = my_write,  //пишем данные в открытый файл.
    .mkdir      = my_mkdir,
    .mknod      = my_mknod,
    .rename     = my_rename,
    .rmdir      = my_rmdir,  // удалить директорию
    .unlink     = my_unlink, // удалить файл
};

 
//------далее вспомогательные функции----------------------------
 
/*это функция делает запись в журнале*/
/*
void logout(char* path)
{
    FILE* f = fopen("/home/OS/5/log.txt","a+");
    int i = 0;
    while(path[i] != 0) fputc((int)path[i++],f);
    fputc((int)'\n',f);
    fclose(f);
}*/
 
/* эта функция создаёт структуру в памяти - 
иерархию папок и файлов - точнее её "голову" */
Link createTree()
{
    Link tree = (Link)malloc(sizeof(Node));
    tree->name = "/"; 
    tree->content = 0;
    tree->parent = 0;
    tree->childs = 0;
    tree->childCount = 0;
    return tree;
}
 
 
/* Добавляем очередной "узел в нашу иерархию  -таким узлом 
может быть файл или папка"*/
void addNode(Link parent, Link node)
{   
    node->parent = parent;   
    Link* newChilds = (Link*)malloc(sizeof(Link*)*(parent->childCount + 1));
    int i;
    for(i = 0; i < parent->childCount; i++)
    {
        newChilds[i] = parent->childs[i];
    }
    newChilds[parent->childCount] = node; 
    parent->childs = newChilds;
    parent->childCount++; 
}
 
/*создаём узел  - записываем данные в область памяти  - 
возвращаем указатель на эту область*/
Link createNode(const char* name, const char* content)
{
    Link node = (Link)malloc(sizeof(Node));
    node->name = (char*)name;
    node->content = (char*)content;
    node->parent = 0;
    node->childs = 0;
    node->childCount = 0;
    return node;
}
/*  удаляем узел*/
void deleteNode(Link node)
{
    Link parent = node->parent;
    Link* newChilds = (Link*)malloc(sizeof(Link)*(parent->childCount - 1));  
    int i = 0;
    int mod = 0;
    for(; i < parent->childCount; i++)
    {
        if (strcmp(parent->childs[i]->name, node->name) == 0)
        {
            mod = -1;
            continue;
        }
        newChilds[i + mod] = parent->childs[i];
    }
    parent->childCount--;
    parent->childs = newChilds;
//    return 0;
}
/* данная функция "разбивает" переданный путь на составляющие - строит массив, послеждовательно 
хранящий имена директорий (узлов, папок) - которое встречались в данному пути*/
char** split(const char* path)
{
    char** array; // возвращаемое значение. 
 
    if (strlen(path) > 1) // возм. - "если это не корневая папка" - если длинна пути больше единицы
    {   
        int count = 0;  
        int i = 0;  
        while(path[i] != 0)
        {
            if (path[i] == '/') count++; // определяем "глубину" переданного  пути
            i++;
        }
 /* в результате мы посчитали "глубину залегания" последнего элемента - узла, указанного в пути */
 
        array = (char**)malloc(sizeof(char*)*(count + 2)); //  выделяем память для  (count + 2) указателей на массивы символов
        array[count + 1] = 0; // в конец строки - отделяющию null символ
 
        array[0] = "/"; // в начало добавлям разделяющий слэш
        int n = 1; // индекс элемента массива указателей на массивы символов - сейчас поставлен на указатель для первого массива символов.
        i = 1; // текущий индекс символа (парвый=0) в строке, содежащей переданный в функцию путь
 
 
        while(path[i] != 0)// перебираем все символы строки , содержащей путь со второго до предпоследнего включительно
        {
            int c = 1;  // число символом в имени одного из узлов пути (можно сказать - имени папки) - пока только один 
            int j = i;  // вспомогательный  индекс - далее мы считываем имя узла  - пока не встретим слыш или строка не закончится - приравниваем его к i
         
            while((path[j] != '/') && (path[j] != 0)) // определяем длинну имени узла -перемещаемся по фрагмента строки пути до следующего слэша
            {
                c++; // фиксируем то , что нами обнаружен очередной символ имени узла
        j++; // накручиваем счётчик 
        i++; // фиксируем тот факт, что мы считали очередной символ переданной в функцию строки, содержащей путь.
            }
            if (path[i] != 0) i++; /* если мы вышли из предыдущего цикла только по причине того, что встретили слэш - то пропускаем его - 
                              чтобы при следующей итерации цикла while(path[i] != 0) снова начать читать символы */
 
            array[n] = (char*)malloc(sizeof(char)*c);// выделяем память по числу букв "c" в имени текущего узла 
            array[n][c - 1] = 0;  // в конец фрагмента - имени узла - отделяющию null символ
            n++;    
        }
 
 
        n = 1; // номер указателя в массие фрагментов пути -  имён директорий
        i = 1;
        while(path[i] != 0)
        {           
            int j = i;
            int tmp = i;            
            while((path[j] != '/') && (path[j] != 0)) // заполняем массив именами фрагментов пути - именами узлов (директорий)
            {
                array[n][j - tmp] = path[j];
                j++;i++;
            }
            if (path[i] != 0) i++;
            n++;    
        }
    }
 else // если всё-таки длина пути равна единице - то есть это путь к корню относительно корня.
    {
        array = (char**)malloc(sizeof(char*)*2); // выделяем память для массива без значащего содержимого
        array[1] = 0;
        array[0] = "/";
    }
    return array;
}
 
/* ищет в иерархихи узел , путь к которому задан как (* path) -
возвращает ПОЛСЕДНИЙ ОБНАРУЖЕННЫЙ узел пути - самую "младший" во ввложенности обнаруженный файл*/
Link skNode(Link tree, const char* path)
{   
    char** splited = split(path); // "разбиваем" путь на куски
 
    int count = 0;// здесь будет храниться число кусков
    int i = 0;
 
    while(splited[i++] != 0) count++; // определяем число кусков
 
    if (count == 1) return tree;// если найден только один - то значит это корень  -возвращаем ссылку на корень нашей иерархии узлов
 
    Link node = tree;    // иначе - если фрагментов ("кусков" - далее - фрагментов) больше чем 1 то, то получаем ссылку на корень нашей иерархии.
    i = 1; // оставим открывающий путь слэш, который храниться на позиции ноль и начнём с первого массива с символами имени франмента пути  - имени директории  
     
  while(splited[i] != 0) // начинаем поиск - перебираем массив фрагментов - последовательных узов пути.
    {       
        int j;      
        for(j = 0; j < node->childCount; j++) // перебираем всех непосредственных потомков данного текущего элемента  - узла
        {           
            if (strcmp(node->childs[j]->name, splited[i]) == 0) // если имя совпало - то есть искомый узел splited[i] (возм. - только промежуточный) - найден.
            {
                node = node->childs[j]; // переходим на уровень ниже - к более младшим 
                break; // раз мы обнаружили элемент - досрочно выхожим из цикла for(j; j < node->childCount; j++)
            }
        }
        i++;
    }
    return node;
}
 
/*следующая функция работает с памятью - выделяет
дополнительное пространство - протсо прибавляет данные буфера 
к уже имеющейся области памяти  - возврщает указатель на суммурную область*/
char* memcpu(const char* source, const char* buf)
{
    int size = strlen(source) + strlen(buf) + 1;    
    char* result = (char*)malloc(sizeof(char)*size);
    result[size - 1] = 0; // ставим отделяющий символ
    int i = 0;
    for(; i < strlen(source); i++) // переносим существующее значение
    {
        result[i] = source[i];
    }
    for(; i < size; i++) // добавляем значение буфера 
    {
        result[i] = buf[i - strlen(source)];
    }
    return result; // возвращаем указатель на область памяти . содежращую старые данные и данные , переданные в буфере.
}

int main(int argc, char *argv[])
{   
    tree = createTree(); // создаём иерархическую структуру файлов и папок в памяти.
    return fuse_main(argc, argv, &my_oper, NULL); // передаём данные можелю ядра ОС  - FUSE
}
 
