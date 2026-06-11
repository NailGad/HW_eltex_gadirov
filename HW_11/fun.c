#include "fun.h"

int game = 1;
Shops shops[SHOPS];
pthread_mutex_t print = PTHREAD_MUTEX_INITIALIZER;

void safe_print(const char *format, ...) {
  va_list args;
  va_start(args, format);
  pthread_mutex_lock(&print);
  vprintf(format, args);
  fflush(stdout);
  pthread_mutex_unlock(&print);
  va_end(args);
}

void *buy(void *arg) {
  Buyers *data = (Buyers *)arg;
  int id = data->id;
  int need = data->need;

  safe_print("[Покупатель %d] Проснулся, потребность = %d\n", id, need);

  while (need > 0) {
    int in = 0;
    for (int i = 0; i < SHOPS; i++) {

      if (pthread_mutex_trylock(&shops[i].mutex) == 0) {
        if (shops[i].size > 0) {
          safe_print("[Покупатель %d] Зашел в ларек %d, товаров = %d\n",
                     id, shops[i].id, shops[i].size);

          while (need > 0 && shops[i].size > 0) {
            int min = (shops[i].size > need) ? need : shops[i].size;
            shops[i].size -= min;
            need -= min;
          }
          safe_print("[Покупатель %d] У покупателя стало %d потребности\n", id,
                     need);
          safe_print(
              "[Покупатель %d] Вышел из ларька %d, осталось товаров = %d\n", id,
              shops[i].id, shops[i].size);
          in = 1;
        }
        pthread_mutex_unlock(&shops[i].mutex);
        if (need <= 0)
          break;
        if (in)
          break;
      }
    }
    if (need > 0) {
      if (!in) {
        safe_print("[покупатель %d] не нашел свободный ларек с товаром\n", id);
      }
      safe_print("[покупатель %d] уснул на 2 секунды\n", id);
      sleep(2);
      safe_print("[покупатель %d] проснулся, текущая потребности = %d\n", id,
                 need);
    }
  }

  safe_print("[Покупатель %d] ЗАВЕРШИЛ\n", id);
  free(data);
  return NULL;
}

void *load(void *arg) {
  safe_print("[ПОГРУЗЧИК] Проснулся и начал работу\n");
  while (game) {
    int ii = rand() % 5;
    pthread_mutex_lock(&shops[ii].mutex);
    shops[ii].size += LOADER_SIZE;

    safe_print(
        "[ПОГРУЗЧИК] Зашел в ларек %d, пополнил на %d, теперь товаров = %d\n",
        shops[ii].id, LOADER_SIZE,shops[ii].size);
    pthread_mutex_unlock(&shops[ii].mutex);
    safe_print("[ПОГРУЗЧИК] Уснул на 1 секунду\n");
    sleep(1);
    safe_print("[ПОГРУЗЧИК] Проснулся\n");
  }
  return NULL;
}
