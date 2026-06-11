#ifndef FUN_H
#define FUN_H

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

typedef struct {
  int id;
  int size;
  pthread_mutex_t mutex;
} Shops;

typedef struct {
  int id;
  int need;
} Buyers;


extern int game;
extern Shops shops[SHOPS];
extern pthread_mutex_t print;


void safe_print(const char *format, ...);
void *buy(void *arg);
void *load(void *arg);
#endif