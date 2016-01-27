#include <stdio.h>
#include <ncurses.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ad.h"
extern WINDOW *list[2];// два рабочих окна
WINDOW *sel; //селектор
DIR *dir;
struct dirent *my_dir;

char cur_space[2][256]; //полный адрес текущей директории для двух окон

/*Функция инициализации главного окна и фона для рабочих окон*/
int init_scr (void) {
    initscr();
    cbreak();
    noecho();
    idlok(stdscr,TRUE);
    clear();
    keypad(stdscr,TRUE);
    curs_set(0);
    /*Инициализация цветовых пар*/
    start_color();
    init_pair(1,COLOR_YELLOW,COLOR_RED);
    init_pair(2,COLOR_GREEN,COLOR_BLUE);
    init_pair(3,COLOR_CYAN,COLOR_BLACK);
    printw(" MANAGER |Move:arrow|Open:enter|/..:F3|Exit:F10|");
    /*Создание фона двух окон */
    list[0]=newwin(getmaxy(stdscr)-2,getmaxx(stdscr)/2,2,0);
    list[1]=newwin(getmaxy(stdscr)-2,getmaxx(stdscr)/2,2,getmaxx(stdscr)/2);
    /*Установка цвета фона для окон*/
    wbkgd(list[0],COLOR_PAIR(1));
    wbkgd(list[1],COLOR_PAIR(2));
    /*Отрисовка рамки вокруг окон*/
    box(list[1],0,0);
    box(list[0],0,0);
    refresh();
    wrefresh(list[0]);
    wrefresh(list[1]);
    keypad(list[0],TRUE);
    keypad(list[1],TRUE);
    return 0;
}




/*Заполнение окна списком элементов директории. Входные параметры:активное окно,адрес текущей директории,
 имя директории, список которой нужно отобразить*/
int new_list (int cur_win,char *cur_dir, char *path) {
        int y_p,i;
        struct stat st;
        /*Переходим в текущую директорию*/
        i=chdir(cur_dir);
        if(i==-1)
        {
            perror("dir not changed");
            return 0;
        }
        /*отрываем текущую директорию*/
        dir = opendir(path);
        if (!dir) {
            perror("diropen");
            return 1;
        };
        /*Переходим в открытую директорию*/
        i=chdir(path);
        if(i==-1)
        {
            perror("dir not changed");
            return 0;
        }
        /*Очищаем окно*/
        wclear(list[cur_win]);
        /*Рисуем заново рамку*/
        box(list[cur_win],0,0);
        wrefresh(list[cur_win]);
        /*Делаем шапку*/
        wattron(list[cur_win],A_BOLD|A_UNDERLINE);
        wmove(list[cur_win],1,2);
        wprintw(list[cur_win],"Name");
        wmove(list[cur_win],1,(getmaxx(stdscr)/2-10));
        wprintw(list[cur_win],"Byte");
        wattroff(list[cur_win],A_BOLD|A_UNDERLINE);
        y_p=2;//поля начинаются c 3 строки


        while ( (my_dir = readdir(dir)) != NULL) {
            i= stat(my_dir->d_name,&st);
            if(i==-1)
            {
                perror("stat error\n");

                return 0;
            }
            /*Устанавливаем курсор под шапку*/
            wmove(list[cur_win],y_p,2);
            wattron(list[cur_win],A_BOLD);
            /*Выводим имя и размер*/
            wprintw(list[cur_win],"%s",my_dir->d_name);
            wmove(list[cur_win],y_p,(getmaxx(stdscr)/2-10));
            wprintw(list[cur_win],"%d",st.st_size);
            y_p++;
            /*Ограничиваем колво записей высотой рабочего окна*/
            if (y_p==((getmaxy(list[cur_win])-1))){
                break;
            }

        }

        wrefresh(list[cur_win]);
        closedir(dir);
        /*Сохраняем полный адрес текущей директории*/
        getcwd(cur_space[cur_win],255);

        /*Возвращаем количество строк в списке*/
        return y_p;
}






