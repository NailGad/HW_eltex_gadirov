#include "fun.h"

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