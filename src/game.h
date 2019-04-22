#ifndef GAME_H
#define GAME_H

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
} string_vec_t; //контейнер

int str_vec_init(string_vec_t *vec,
                 unsigned capacity); //подготовка структуры к работе, выделение
                                     //изначальной памяти
int str_vec_push(string_vec_t *vec, char *value); //если хотим, что-то добавить
void str_vec_free(string_vec_t *vec); //освобождает вектор
char *str_vec_random(string_vec_t *vec); //выбирает рандомное слово
int str_vec_load_from_file(string_vec_t *vec, char *filedir,
                           char *filename); //выгружаем слова из файла

typedef struct {
    Pixmap bitmap; //картинка
    unsigned int bitmap_width, bitmap_height; //параметры картинки
    int x, y;
} pixmap_attr_t;

int load_pixmap(x_window_param_t *window, pixmap_attr_t *pixmap, //загрузка картинки из файла
char *filedir, char *filename);