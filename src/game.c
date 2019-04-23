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

char *str_vec_random(string_vec_t *vec) { //выбирает рандомное слово
  srand(time(NULL)); //задает зерно для вычисления рандомного числа
  int random_variable = rand(); //возвращает случайное число
  return vec->content[random_variable % vec->size]; //возвращает элемент таблицы
                                                    //в которой находится слово
}

static char *merge_str(char *first,
                       char *second) { //объединяет два слова в одну строку и
                                       //возвращает указатель на нее
  int size = strlen(
      first); //высчитываем необходимый для выделения размер памяти(создаем
              //переменную и присваиваем ей значение первого аргумента функции
  size += strlen(second); //прибавляем к этому аргументу значение второй строки
  ++size;                 //увеличиваем на единицу

  char *result = malloc(
      size * sizeof(char)); //создаем переменную и выделяем под нее память
  strncpy(result, first, strlen(first)); //копируем в эту память слово
  strncpy(result + strlen(first), second,
          strlen(second)); //копируем второе слово оставив место под первое
  result[size] = '\0';     //присваиваем конец строки
  return result;           //возвращаем указатель
}

int str_vec_load_from_file(string_vec_t *vec, char *filedir,
                           char *filename) { //загружаем слова из файла
  str_vec_free(vec);     //освобождает память
  str_vec_init(vec, 10); //подготовка вектора к работе

  char *fullpath =
      merge_str(filedir, filename); //объединяем две переменные в одну строку и
                                    //передаем указатель на новую строку

  FILE *words_file = fopen(
      fullpath,
      "r"); //создаем указатель на файл со словами, открываем файл для чтения
  if (!words_file) { //обработка ошибок
    perror("Open words file failed");
    return 1;
  }

  free(fullpath); //если удалось открыть файл удаляем уже ненужную переменную

  char current_word[100]; //массив в котором хранится слово
  int err = 0; //создаем переменную в которой хранится значение ошибки

  while (!feof(words_file)) { //цикл, который будет работать до тех пор пока
                              //файл не закончился
    fscanf(words_file, "%s", current_word); //принимает значение из файла

    err = str_vec_push(vec, current_word); //добавляем это слово в наш вектор
    if (err != 0) { //обработка ошибок
      fprintf(stderr, "Vector push failed.\n");
      return 1;
    }

    current_word[0] = '\0';
  }

  return 0;
}

game_lang_t lang_select() {
  printf("Language selection:\n  1)ENG\n  2)RUS\n  Selection:");
  int itog;
  game_lang_t result = LANG_ENG;
  scanf("%i", &itog);
  switch (itog) {
  case 1:
    result = LANG_ENG;
    break;
  case 2:
    result = LANG_RUS;
    break;
  }
  return result;
}

int game_res_init(x_window_param_t *window, game_res_t *res, char *path,
                  game_lang_t lang) { //загрузка ресурсов
  memset(&res->words, 0, sizeof(string_vec_t)); //зануляем память в базе слов
  res->language = lang;
  int err = 0;
  if (res->language == LANG_RUS) {
    err = str_vec_load_from_file(&res->words, path, "words.txt");
  } else {
    err = str_vec_load_from_file(&res->words, path, "words_eng.txt");
  }

  if (err != 0)           //обработка ошибок
    goto error_handler_1; //если не получилось загрузить слова то перекидывает
                          //на  error_handler_1

  int count = 0; //счетчик для загрузки картинок

  for (; count < 7; count++) {

    char image_name[11]; //массив для названия файла картинки
    sprintf(image_name, "pos_%i.xbm",
            count); //задаем правильное название файла используя счетчик

    err = load_pixmap(window, &res->step_to_death[count], path,
                      image_name); //если не получилось загрузить картинку то
                                   //перекидывает на error_handler_2
    if (err != 0)
      goto error_handler_2;
  }

  return 0;

error_handler_2:
  do { //очищает файлы в обратном порядке относительно счетчика
    count--;
    XFreePixmap(window->display, res->step_to_death[count].bitmap);
  } while (count > 0);

  str_vec_free(&res->words); //очищает вектор

error_handler_1:
  fprintf(stderr,
          "Game resources loading failed.\n"); //выводит сообщение об ошибке
  return 1;
}

void game_res_free(x_window_param_t *window,
                   game_res_t *res) { //освобождает ресурсы
  str_vec_free(&res->words);

  for (int i = 0; i < 6; i++)
    XFreePixmap(window->display, res->step_to_death[i].bitmap);
}
