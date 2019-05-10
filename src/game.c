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

int load_pixmap(x_window_param_t *window,
                pixmap_attr_t *pixmap, //структура, хранящая картинку
                char *filedir, char *filename) {
  char *fullpath = merge_str(
      filedir, filename); //объединяет дерикторию и имя файла в одну строку
  int rc = XReadBitmapFile(
      window->display,
      window->window, //возвращает результат, прочитана ли картинка
      fullpath,       //путь
      &pixmap->bitmap_width, &pixmap->bitmap_height, //параметры
      &pixmap->bitmap, //место куда загрузится картинка
      &pixmap->x, &pixmap->y); //координаты
  if (rc != BitmapSuccess) {   //обработка ошибок
    fprintf(stderr, "Read bitmap failed: %s\n", fullpath);
    return 1;
  }
  free(fullpath); //освобождение памяти
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

int game_init(game_stat_t *game,
              game_res_t *game_res) { //подготовка структуры к работе

  game->words_base = &game_res->words; //присваиваем значение для базы слов
  game->word_progress = NULL; //прогресс слова
  if (game_reset(game)) {     //сброс текущего прогреса
    fprintf(stderr, "Game reset failed.\n");
    return 1;
  }
  return 0;
}

void game_letter_push(game_stat_t *game,
                      char *letter) { //принимаем букву из русского яз
  int hitting = 0;                    //количество попаданий

  for (int i = 0; i < strlen(game->current_word);
       ++i) { //проходим по всему слову и проверяем подходит ли буква, которую
              //мы написали
    if ((letter[0] == game->current_word[i]) && //проверяем по 2 чара из-за
                                                //особенностей символов русского
                                                //алфавита (занимают 2 чара)
        (letter[1] == game->current_word[i + 1])) {
      game->word_progress[i] = game->current_word[i];
      game->word_progress[i + 1] = game->current_word[i + 1];
      i++;
      hitting++;
    }
  }

  if (hitting == 0) //если у нас 0 попаданий
    game->step_to_death++; //счетчик "шагов к проигрышу" увеличивается

  if (game->step_to_death ==
      6) //если счетчик "шагов к проигрышу" достиг 6 - проигрыш
    game->status = GAME_OVER;
}

void game_letter_push_eng(game_stat_t *game,
                          char letter) { //тот же цикл для английской раскладки
  int hitting = 0;

  for (int i = 0; i < strlen(game->current_word); ++i) {
    if (letter == game->current_word[i]) {
      game->word_progress[i] = game->current_word[i];
      hitting++;
    }
  }

  if (hitting == 0)
    game->step_to_death++;

  if (game->step_to_death == 6)
    game->status = GAME_OVER;
}

static void game_word_progress_free(
    game_stat_t
        *game) { //освобождает память, которая выделена для word_progress
  if (game->word_progress != NULL)
    free(game->word_progress);
}

int game_reset(game_stat_t *game) { //сбрасывает текущий прогресс
  game->current_word =
      str_vec_random(game->words_base); //берем новое рандомное слово

  game_word_progress_free(game); //удаляет старое слово
  int word_len = strlen(game->current_word) + 1; //узнаем длину нового слова

  game->word_progress =
      malloc(word_len * sizeof(char)); //выделяем память под новое слово
  if (!game->word_progress) { //оброботка ошибок
    perror("Word_progress allocation failed");
    return 1;
  }

  memset(game->word_progress, '_',
         word_len * sizeof(char)); //если ошибки нет записываем везде _
  game->word_progress[word_len - 1] =
      '\0'; //последниму значению присваиваем значение конца строки

  game->step_to_death =
      0; //счетчик "шагов до проигрыша" в начале игры равен нулю
  game->status = GAME_PROGRESS;

  return 0;
}

void game_free(game_stat_t *game) { //освобождение памяти структуры логики игры
  game_word_progress_free(
      game); //освобождаем память, которая выделилась для game_word_progress
}

char *
game_return_progress(game_stat_t *game) { //возвращает слово (для русского яз)
  int size = strlen(game->current_word) + 1; //узнаем размер слова

  static char *result; //создаем указатель на новую строку

  if (result != NULL) //проверяем равен ли он null
    free(result);     //если не равен - то удаляем

  result = malloc(size * sizeof(char)); //выделяем новую память

  memset(result, 0, size * sizeof(char)); //зануляем

  for (int i = 0, j = 0; j < size; i++,
           j++) { //в цикле мы используем 2 переменные счетчика для
                  //отслеживания позиции в текущемм слове и слове, которое мы
                  //получим в результате. Это необходимо из-за того, что в
                  // Linux русская раскладка требует 2 байта и кол-во
                  //подчеркиваний в текущем слове удвоено. При нахождении
                  //подчеркиваний счетчик позиции текущего слова перепрыгивает
                  //через 1 байт
    result[i] = game->word_progress[j];
    if (game->word_progress[j] == '_')
      j++;
  }

  return result;
}

char *
game_return_progress_eng(game_stat_t *game) { //возвращаем слово (для англ яз)
  return game->word_progress;
}

int game_win_check(
    game_stat_t *game) { //проверяем совпадает ли исходное слово с прогрессом
  return !strcmp(game->current_word, game->word_progress)
             ? 1
             : 0; //если совпадает 1, если нет 0
}
int return_letter_by_keycode(unsigned int keycode, char *output) {

  if (keycode == 0x18) {
    strncpy(output, "й", 2 * sizeof(char));
  } else if (keycode == 0x19) {
    strncpy(output, "ц", 2 * sizeof(char));
  } else if (keycode == 0x1a) {
    strncpy(output, "у", 2 * sizeof(char));
  } else if (keycode == 0x1b) {
    strncpy(output, "к", 2 * sizeof(char));
  } else if (keycode == 0x1c) {
    strncpy(output, "е", 2 * sizeof(char));
  } else if (keycode == 0x1d) {
    strncpy(output, "н", 2 * sizeof(char));
  } else if (keycode == 0x1e) {
    strncpy(output, "г", 2 * sizeof(char));
  } else if (keycode == 0x1f) {
    strncpy(output, "ш", 2 * sizeof(char));
  } else if (keycode == 0x20) {
    strncpy(output, "щ", 2 * sizeof(char));
  } else if (keycode == 0x21) {
    strncpy(output, "з", 2 * sizeof(char));
  } else if (keycode == 0x22) {
    strncpy(output, "х", 2 * sizeof(char));
  } else if (keycode == 0x23) {
    strncpy(output, "ъ", 2 * sizeof(char));

  } else if (keycode == 0x26) {
    strncpy(output, "ф", 2 * sizeof(char));
  } else if (keycode == 0x27) {
    strncpy(output, "ы", 2 * sizeof(char));
  } else if (keycode == 0x28) {
    strncpy(output, "в", 2 * sizeof(char));
  } else if (keycode == 0x29) {
    strncpy(output, "а", 2 * sizeof(char));
  } else if (keycode == 0x2a) {
    strncpy(output, "п", 2 * sizeof(char));
  } else if (keycode == 0x2b) {
    strncpy(output, "р", 2 * sizeof(char));
  } else if (keycode == 0x2c) {
    strncpy(output, "о", 2 * sizeof(char));
  } else if (keycode == 0x2d) {
    strncpy(output, "л", 2 * sizeof(char));
  } else if (keycode == 0x2e) {
    strncpy(output, "д", 2 * sizeof(char));
  } else if (keycode == 0x2f) {
    strncpy(output, "ж", 2 * sizeof(char));
  } else if (keycode == 0x30) {
    strncpy(output, "э", 2 * sizeof(char));

  } else if (keycode == 0x34) {
    strncpy(output, "я", 2 * sizeof(char));
  } else if (keycode == 0x35) {
    strncpy(output, "ч", 2 * sizeof(char));
  } else if (keycode == 0x36) {
    strncpy(output, "с", 2 * sizeof(char));
  } else if (keycode == 0x37) {
    strncpy(output, "м", 2 * sizeof(char));
  } else if (keycode == 0x38) {
    strncpy(output, "и", 2 * sizeof(char));
  } else if (keycode == 0x39) {
    strncpy(output, "т", 2 * sizeof(char));
  } else if (keycode == 0x3a) {
    strncpy(output, "ь", 2 * sizeof(char));
  } else if (keycode == 0x3b) {
    strncpy(output, "б", 2 * sizeof(char));
  } else if (keycode == 0x3c) {
    strncpy(output, "ю", 2 * sizeof(char));
  } else
    return 1;
  return 0;
}

int return_letter_by_keycode_eng(unsigned int keycode, char *output) {

  if (keycode == 0x18) {
    strncpy(output, "q", sizeof(char));
  } else if (keycode == 0x19) {
    strncpy(output, "w", sizeof(char));
  } else if (keycode == 0x1a) {
    strncpy(output, "e", sizeof(char));
  } else if (keycode == 0x1b) {
    strncpy(output, "r", sizeof(char));
  } else if (keycode == 0x1c) {
    strncpy(output, "t", sizeof(char));
  } else if (keycode == 0x1d) {
    strncpy(output, "y", sizeof(char));
  } else if (keycode == 0x1e) {
    strncpy(output, "u", sizeof(char));
  } else if (keycode == 0x1f) {
    strncpy(output, "i", sizeof(char));
  } else if (keycode == 0x20) {
    strncpy(output, "o", sizeof(char));
  } else if (keycode == 0x21) {
    strncpy(output, "p", sizeof(char));

  } else if (keycode == 0x26) {
    strncpy(output, "a", sizeof(char));
  } else if (keycode == 0x27) {
    strncpy(output, "s", sizeof(char));
  } else if (keycode == 0x28) {
    strncpy(output, "d", sizeof(char));
  } else if (keycode == 0x29) {
    strncpy(output, "f", sizeof(char));
  } else if (keycode == 0x2a) {
    strncpy(output, "g", sizeof(char));
  } else if (keycode == 0x2b) {
    strncpy(output, "h", sizeof(char));
  } else if (keycode == 0x2c) {
    strncpy(output, "j", sizeof(char));
  } else if (keycode == 0x2d) {
    strncpy(output, "k", sizeof(char));
  } else if (keycode == 0x2e) {
    strncpy(output, "l", sizeof(char));

  } else if (keycode == 0x34) {
    strncpy(output, "z", sizeof(char));
  } else if (keycode == 0x35) {
    strncpy(output, "x", sizeof(char));
  } else if (keycode == 0x36) {
    strncpy(output, "c", sizeof(char));
  } else if (keycode == 0x37) {
    strncpy(output, "v", sizeof(char));
  } else if (keycode == 0x38) {
    strncpy(output, "b", sizeof(char));
  } else if (keycode == 0x39) {
    strncpy(output, "n", sizeof(char));
  } else if (keycode == 0x3a) {
    strncpy(output, "m", sizeof(char));
  } else
    return 1;
  return 0;
}


int game_lose_check(
    game_stat_t *game) { //проверяем равно ли количество "шагов до проигрыша" 6
  return game->step_to_death == 6 ? 1 : 0; //если совпадает 1, если нет 0
}
