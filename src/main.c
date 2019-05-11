#include "game.h"
#include <stdio.h>

int main() {
  x_window_param_t window_param; //создаем окно

  game_res_t res;   //ресурсы игргы
  game_stat_t game; //логика игры

  int err; //переменная для обработки ошибок
  game_lang_t language = lang_select();

  err = window_init(&window_param, "Gallows game", 100, 100, 400,
                    250); //подготовка окна к работе
  if (err != 0) {
    fprintf(stderr, "window_init failed.\n"); //выводит поток ошибок
    return 1;
  }

  err = game_res_init(&window_param, &res, "./res/",
                      language); //подготовка ресурсов к игре
  if (err != 0) {
    fprintf(stderr, "game_res_init failed.\n"); //оброботка ошибок и вывод
    return 1;
  }

  err = game_init(&game, &res); //подготовка логики игры
  if (err != 0) {
    fprintf(stderr, "game_init failed.\n"); //обработка ошибок и вывод
    return 1;
  }

  err = pre_game_settings(&window_param); //предигровая настройка
  if (err != 0)
    return 1;

  game_loop(&window_param, &res, &game); //игровой цикл

  game_free(&game); //освобождение всех ресурсов
  game_res_free(&window_param, &res);
  window_free(&window_param);
}
