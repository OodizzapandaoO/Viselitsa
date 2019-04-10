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
