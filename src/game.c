#include "game.h"
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void set_title(char *title_ptr, Display *display,
                      Window window) { //дает имя окна
  XTextProperty windowname;
  XStringListToTextProperty(&title_ptr, 1, &windowname);

  XSetWMProperties(display, window, &windowname, NULL, NULL, NULL, NULL, NULL,
                   NULL); //устанавливает свойства окну
}

int window_init(x_window_param_t *param, char *title, int pos_x, int pos_y,
                int Width, int Height) {
  param->display = XOpenDisplay(NULL);
  if (param->display == NULL) {
    perror("Open display failed");
    return 1;
  }

  param->screen_number = DefaultScreen(param->display);

  param->window = XCreateSimpleWindow( //создаем окно
      param->display, RootWindow(param->display, param->screen_number), pos_x,
      pos_y, Width, Height, 1,
      BlackPixel(param->display, param->screen_number), //возвращает код цвета
      WhitePixel(param->display, param->screen_number));

  set_title(title, param->display, param->window); //имя окошка

  XSelectInput(param->display, param->window,
               KeyPressMask |
                   KeyReleaseMask); //выбирает устройство обработки событий
  XMapWindow(param->display, param->window); //отображает окно

  param->gc = XCreateGC(
      param->display, param->window, 0,
      NULL); //создает "руку которая берет нужный карандашь для рисования"

  return 0;
}
void window_free(x_window_param_t *param) { //освобождение памяти
  XFreeGC(param->display, param->gc); //освобождаем графический контекст
  XCloseDisplay(param->display); //закрываем сервер display
}

int str_vec_init(string_vec_t *vec, unsigned capacity) { //подготовка структуры
  vec->content = malloc(capacity * sizeof(char **)); //выделяем память

  if (!vec->content) { //обработка ошибок
    perror("Vector init allocation failed");
    return 1;
  }

  vec->capacity = capacity; //переменной capacity присваиваем значение размера
                            //выделеной памяти
  vec->size = 0; //изначальный размер равен нулю

  return 0;
}
