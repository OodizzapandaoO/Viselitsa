#include "./../src/game.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void logic_test(
    char *current_word, char letter,
    char *expected_result); //проверяет логику функции game_letter_push_eng

int main() {
  logic_test("people", 'o', "__o___");
  logic_test("sneak", 'n', "_n___");
  logic_test("ready", 'r', "r____");
  logic_test("array", 'r', "_rr__");
  logic_test("snake", 'o', "__o__");

  return 0;
}

void logic_test(char *current_word, char letter, char *expected_result) {
  game_stat_t game; //проверка состояния игры

  game.current_word = current_word; //устанавливает текущее слово
  int size = strlen(current_word) + 1; //размер слова
  game.word_progress =
      malloc(size * sizeof(char)); //выделяет память для game.word_progress
  memset(game.word_progress, '_', size);
  game.word_progress[size - 1] = '\0';
  game.step_to_death = 0;
  game.status = GAME_PROGRESS;

  game_letter_push_eng(&game, letter); //запускает тестирование
  if (!strcmp(game.word_progress,
              expected_result)) //проверяет соответствие с ожидаемым результатом
    printf("%s Test is done!\n", current_word);
  else
    printf("%s Test is failed!\n", current_word);

  free(game.word_progress);
}
