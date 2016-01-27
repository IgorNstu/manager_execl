#include <stdio.h>
#include <ncurses.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ad.h"
WINDOW *list[2];
extern char cur_space[2][256];
int max[2];
int main(void)
{
    int active_button,c,ptr, cur_win;

    init_scr();// инициализация основного экрана
    /*Отркываем в окнах текущии директории.Получаем количество строк в списке*/
    max[0]=new_list(0, ".",".");
    max[1]=new_list(1, ".",".");
    /*Устанавливаем активное окно*/
    cur_win=0;
    /*Устанавливаем переменную положения курсора на первый элемент*/
    ptr=1;
    ptr=selector(cur_win,ptr,0);
    active_button=1;
    /*Работа до прерывания пользователем*/
    while (active_button) {
        c=getch();
        switch (c) {
        /*Отработка нажатия стрелки ВНИЗ*/
        case KEY_DOWN:
            if (ptr<(max[cur_win]-1))
            {
            ptr=selector(cur_win,ptr,0);
            }
        break;

        /*Отработка нажатия клавиги ВВЕРХ*/
        case KEY_UP:
        if (ptr>2){
        ptr=selector(cur_win,ptr,1);
        }
        break;

        /*Переключение между окнами*/
        case KEY_LEFT:
        case KEY_RIGHT:
            if (cur_win==0)  {
                cur_win=1;

            }
            else if (cur_win==1){
                 cur_win=0;

            }
        ptr=selector(cur_win,ptr,2);
        break;

        /*Клавиша ENTER*/
        case 10:
            ptr= my_open(cur_win,ptr);
            ptr=selector(cur_win,ptr,3);
        break;

        /*Переход в родительскую папку текущей папки*/
        case KEY_F(3):
            max[cur_win]=new_list(cur_win,cur_space[cur_win],"..");
            ptr=selector(cur_win,ptr,3);
            break;

         /*Выход*/
        case KEY_F(10):
            active_button=0;
        break;

    }

}
    /*Очищаем и удаляем окна,востанавливаем свойства терминала*/
    wclear(list[1]);
    wclear(list[0]);
    delwin(list[1]);
    delwin(list[0]);
    echo();
    curs_set(TRUE);
    return 0;
}
