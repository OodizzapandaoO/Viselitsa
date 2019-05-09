#ifndef GAME_H
#define GAME_H

#include <X11/Xlib.h>

typedef struct {    //отрисовка окна
  Display *display; //сервер
  Window window;    //окно
  XEvent event; //переменная для обработки событий
  int screen_number; //номер экрана
  GC gc;             //карандаш
} x_window_param_t;

int window_init(x_window_param_t *param,
                char *title, //делает предварительные работы
                int pos_x, int pos_y, int Width,
                int Height); //высота, ширина и т.д.

void window_free(x_window_param_t *param); //освобождает ресурсы

typedef struct {     //контейнер для значений
  char **content;    //указатель на указатель
  unsigned capacity; //общий размер выделеной памяти
  unsigned size; //сколько объектов в нем сейчас находится
} string_vec_t; //"контейнер

int str_vec_init(string_vec_t *vec,
                 unsigned capacity); //подготовка структуры к работе, выделение
                                     //изначальной памяти
int str_vec_push(string_vec_t *vec, char *value); //если хотим, что-то добавить
void str_vec_free(string_vec_t *vec); //освобождает вектор
char *str_vec_random(string_vec_t *vec); //выбирает рандомное слово
int str_vec_load_from_file(string_vec_t *vec, char *filedir,
                           char *filename); //выгружаем слова из файла

typedef struct {
  Pixmap bitmap;                            //картинка
  unsigned int bitmap_width, bitmap_height; //параметры картинки
  int x, y;
} pixmap_attr_t;

int load_pixmap(x_window_param_t *window,
                pixmap_attr_t *pixmap, //загружает из файла картинку
                char *filedir, char *filename);

typedef enum { //вспомогательные константы
  LANG_RUS,
  LANG_ENG
} game_lang_t;

game_lang_t lang_select();

typedef struct {
  game_lang_t language; //язык
  string_vec_t words;
  pixmap_attr_t step_to_death[7]; // 7 картинок
} game_res_t;                     //ресурсы для игры

int game_res_init(x_window_param_t *window, game_res_t *res, char *path,
                  game_lang_t lang); //загрузка ресурсов
void game_res_free(x_window_param_t *window,
                   game_res_t *res); //освобождение ресурсов

typedef enum { //вспомогательные константы
  GAME_PROGRESS,
  GAME_OVER
} game_status_t;
