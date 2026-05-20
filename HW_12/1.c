#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define BUYER 3
#define SHOPS 5
#define LOADER_SIZE 200
#define MAX_SHOP_SIZE 1100
#define MIN_SHOP_SIZE 900
#define MAX_NEED_SIZE 10100
#define MIN_NEED_SIZE 9900

int game = 1;

typedef struct {
  int id;
  int size;
  pthread_mutex_t mutex;
} Shops;

Shops shops[SHOPS];
pthread_mutex_t print = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
  int id;
  int need;
} Buyers;

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

int main() {
  srand(time(NULL));

  for (int i = 0; i < SHOPS; i++) {
    shops[i].id = i + 1;
    shops[i].size =
        rand() % (MAX_SHOP_SIZE - MIN_SHOP_SIZE + 1) + MIN_SHOP_SIZE;
    pthread_mutex_init(&shops[i].mutex, NULL);
    safe_print("[Магазин %d] имеет %d товаров\n", shops[i].id, shops[i].size);
  }

  int total_goods = 0;
  for (int i = 0; i < SHOPS; i++) {
    total_goods += shops[i].size;
  }
  safe_print("\nВсего товара в магазинах: %d\n\n", total_goods);

  pthread_t loader;
  pthread_create(&loader, NULL, load, NULL);

  pthread_t buyers[BUYER];
  for (int i = 0; i < BUYER; i++) {
    Buyers *data = malloc(sizeof(Buyers));
    data->id = i + 1;
    data->need = rand() % (MAX_NEED_SIZE - MIN_NEED_SIZE + 1) + MIN_NEED_SIZE;
    pthread_create(&buyers[i], NULL, buy, data);
  }

  for (int i = 0; i < BUYER; i++) {
    pthread_join(buyers[i], NULL);
  }

  game = 0;
  pthread_join(loader, NULL);

  for (int i = 0; i < SHOPS; i++) {
    pthread_mutex_destroy(&shops[i].mutex);
  }
  printf("\n===ПРОГРАММА ЗАВЕРШИНА===\n");
  return 0;
}