#include <ncurses.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "ad.h"
extern WINDOW *list[2];
extern WINDOW *sel;
extern DIR *dir;
extern struct dirent *my_dir;
extern char cur_space[2][256];
extern int max[2];

/*Управление селектором. Входные параметры: активное окно, текущие положение
селектора, режим*/
int selector (int cur_win, int ptr, int key) {
    /*удаляем окно селектора,если существовало до*/
    werase(sel);
    delwin(sel);
    wrefresh(list[cur_win]);
    /*Режим 0,перемещение селектора вниз*/
    if (key==0){
        ptr++;
    }
    /*Режим 1,перемещение селектора вверх*/
     else if (key==1){
        ptr--;
    }
    /*Режим 2,установка селектора в начало*/
    else if (key==2){
        ptr=2;
        wrefresh(list[1]);
        wrefresh(list[0]);
    }
    /*Режим 3,обновление окна селектора*/
    else if (key==3){
        wrefresh(list[1]);
        wrefresh(list[0]);
    }
    /*Создаём окно селектора и вставляем в него символ*/
    sel=derwin(list[cur_win],1,1,ptr,1);
    wattron(sel,A_BOLD);
    wprintw(sel,">");
    wrefresh(sel);
    wrefresh(list[cur_win]);
    /*Возвращаем новое положение селектора*/
    return ptr;
}

/*Открытие папки по нажатию энтер. Входные параметры:активное окно,текущие положение селектора*/
 int my_open (int cur_win,int ptr) {
     int i,p;
     char* name;
     pid_t pid;
     int status;
     /*Переходим в текущую директорию*/
     p=chdir(cur_space[cur_win]);
     if(p==-1)
     {
         perror("dir not changed");
         return ptr;
     }
     /*Отркываем текущию директорию*/
     dir=opendir(".");
     if (!dir) {
          perror("diropen");
          return ptr;
     }
     /*Переходим к выбранной записи*/
     for(i=1;i<=ptr-1;i++)
     {
         my_dir=readdir(dir);
     }
     name=my_dir->d_name;
     /*Если выбранный объект-директория,вызываем функцию отрисовки нового списка*/
     if(my_dir->d_type==4){

         max[cur_win]=new_list(cur_win,cur_space[cur_win],name);
         return 2;
     }
     /*Если не директория,проверяем файл на "регулярность"*/
     if (my_dir->d_type==8){
         i=access(name,X_OK); //проверяем БИТ исполняемости.

         /*Если файл имеет разрешение на исполнение,запускаем*/
         if ((access(name,X_OK)==0)){
             pid=fork();
             if (pid==0){
                 execl(name,name,NULL);
                 exit(1);
             }
             /*Родительским процессом ожидаем завершения дочернего.Если дочерний возвращет 1- выводим сообщение о ошибке*/
             else {
                 wait(&status);
                 if (status==1){
                     perror("fork");
                     my_refresh();
                     return ptr;
                 }
                 /*Востанавливаем параметры экрана*/
                 my_refresh();
                 return ptr;
             }
         }
         /*Если файл не исполняемый, открываем его своим текстовым редактором. Редактору передаём имя файла и путь к нему*/
         else {

                pid=fork();
                if (pid==0){
                execl("/home/igor/my_editor","/home/igor/my_editor",my_dir->d_name,cur_space[cur_win],NULL);
                exit(1);
                }
                else{
                wait(&status);
                if (status==1){
                    perror("txt");
                }
                my_refresh();

                }
            }

         }
     return ptr ;
     }
