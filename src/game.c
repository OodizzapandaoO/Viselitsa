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

int str_vec_push(string_vec_t *vec, char *value) { //добавляем слово
  if (value[0] == '\0')
    return 0;

  if (vec->size >= vec->capacity) { //проверяем не заполнен ли контейнер

    unsigned new_capacity = vec->capacity * 2; //если заполнен удваеваем место
    char **new_content = realloc(
        vec->content,
        new_capacity * sizeof(char **)); //перевыделяет новое пространство и
                                         //копирует туда старые элементы

    if (!new_content) { //проверяем не заполнен ли новый контейнер
      perror("Vector push reallocation failed"); //если есть ошибка
      return 1;
    }

    vec->capacity = new_capacity; //если нет ошибки
    vec->content = new_content;
  }

  int value_len = strlen(value) + 1; //определяем размер слова
  vec->content[vec->size] =
      malloc(value_len * sizeof(char)); //выделяем память для слова
  if (!vec->content[vec->size]) { //проверяем получилось ли добавить слово
    perror("Vector push allocation failed"); //если не получилось
    return 1;
  }

  strncpy(vec->content[vec->size], value,
          value_len); //копируем слово по данному указателю
  ++vec->size; //увеличиваем размер size

  return 0;
}

void str_vec_free(string_vec_t *vec) { //освобождение
  if (vec->content == NULL) //проверяет нужно ли вообще очищать content
    return;

  for (int n = 0; n < vec->size;
       ++n) { //если нужно, то проходим по каждому элементу и очищаем
    free(vec->content[n]);
  }
  free(vec->content); //освобождаем саму таблицу
}
