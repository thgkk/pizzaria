#include "pizzeria.h"
#include "queue.h"
#include "helper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

queue_t* fila;

sem_t garconsDisponiveis;

sem_t pizzaiolosDisponiveis;

pthread_mutex_t mtxPegarMesa;

int mesasDisponiveis;

int fornoDisponivel;

void pizzeria_init(int tam_forno, int n_pizzaiolos, int n_mesas, int n_garcons, int tam_deck, int n_grupos) {
  pthread_mutex_init(&mtxPegarMesa,NULL);

  mesasDisponiveis = n_mesas;

  fornoDisponivel = tam_forno;

  sem_init(&pizzaiolosDisponiveis, 0 ,n_pizzaiolos);

  sem_init(&garconsDisponiveis, 0, n_garcons);

  queue_init(fila,tam_deck);


}

void pizzeria_close() {
}

void pizzeria_destroy() {
}

void pizza_assada(pizza_t* pizza) {
  sem_wait(&pizzaiolosDisponiveis);
  pizzaiolo_retirar_forno(pizza);
  fornoDisponivel++;
  sem_post(&pizzaiolosDisponiveis);
}

int pegar_mesas(int tam_grupo) {
  pthread_mutex_lock(&mtxPegarMesa);


  if (tam_grupo % 4 == 0){
    mesasDisponiveis -= tam_grupo/4;

  } else {
    mesasDisponiveis -= tam_grupo/4 + 1;
  }
  pthread_mutex_unlock(&mtxPegarMesa);
  return 0; //erro: não fui implementado (ainda)!
}

void garcom_tchau(int tam_grupo) {
  pthread_mutex_lock(&mtxPegarMesa);

  if (tam_grupo % 4 == 0){
    mesasDisponiveis += tam_grupo/4;

  } else {
    mesasDisponiveis += tam_grupo/4 + 1;
  }
  pthread_mutex_unlock(&mtxPegarMesa);
}

void garcom_chamar() {
  sem_wait(&garconsDisponiveis);
  sem_post(&garconsDisponiveis);

}

void fazer_pedido(pedido_t* pedido) {
  queue_push_back(fila, (void*)pedido);

  sem_wait(&pizzaiolosDisponiveis);
  pizza_t* pizza = pizzaiolo_montar_pizza(pedido);
  sem_post(&pizzaiolosDisponiveis);

  if(fornoDisponivel > 0){
    sem_wait(&pizzaiolosDisponiveis);
    pizzaiolo_colocar_forno(pizza);
    fornoDisponivel--;
    sem_post(&pizzaiolosDisponiveis);
  }
}

int pizza_pegar_fatia(pizza_t* pizza) {
    return -1; // erro: não fui implementado (ainda)!
}
