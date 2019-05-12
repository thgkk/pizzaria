#include "pizzeria.h"
#include "queue.h"
#include "helper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

queue_t* smartDeck;

sem_t semMesasDisponiveis, semGarconsDisponiveis, semPizzaiolosDisponiveis, semFornoDisponivel;

pthread_mutex_t mtxPegarMesa, mtxPa, mtxPegador;

pthread_t *pizzailos, *garcons;

int pizzariaAberta;

void pizzeria_init(int tam_forno, int n_pizzaiolos, int n_mesas, int n_garcons, int tam_deck, int n_grupos) {



  pizzariaAberta = 1;

  pthread_mutex_init(&mtxPegarMesa,NULL);

  pthread_mutex_init(&mtxPa, NULL);

  pthread_mutex_init(&mtxPegador, NULL);

  //mesasDisponiveis = n_mesas;

  sem_init(&semFornoDisponivel, 0, tam_forno);

  sem_init(&semMesasDisponiveis, 0, n_mesas);

  sem_init(&semPizzaiolosDisponiveis, 0, n_pizzaiolos);

  sem_init(&semGarconsDisponiveis, 0, n_garcons);

  queue_init(smartDeck,tam_deck);


}

void pizzeria_close() {
  pizzariaAberta = 0;
}

void pizzeria_destroy() {
  pthread_mutex_destroy(&mtxPa);
  pthread_mutex_destroy(&mtxPegador);
  pthread_mutex_destroy(&mtxPegarMesa);

  sem_destroy(&semFornoDisponivel);
  sem_destroy(&semMesasDisponiveis);
  sem_destroy(&semPizzaiolosDisponiveis);
  sem_destroy(&semGarconsDisponiveis);
}

void pizza_assada(pizza_t* pizza) {
  pthread_mutex_lock(&mtxPa);
  pizzaiolo_retirar_forno(pizza);
  pthread_mutex_unlock(&mtxPa);
  sem_post(&semFornoDisponivel);
}

int pegar_mesas(int tam_grupo) {
  /*pthread_mutex_lock(&mtxPegarMesa);

  if (tam_grupo % 4 == 0 && mesasDisponiveis>0){
    mesasDisponiveis -= tam_grupo/4;

  }
  if(tam_grupo % 4 != 0 && mesasDisponiveis>0) {
    mesasDisponiveis -= tam_grupo/4 + 1;
  }
  pthread_mutex_unlock(&mtxPegarMesa);
  return 0; //erro: não fui implementado (ainda)!
}*/
  if (!pizzariaAberta)
    return -1;

  if(tam_grupo % 4 == 0) {
    for(int i = 0; i<tam_grupo/4 ;i++){
      sem_wait(&semMesasDisponiveis);
    }
    return 0;
  }
  else{
    for(int i=0; i<tam_grupo/4 + 1; i++){
      sem_wait(&semMesasDisponiveis);
    }
    return 0;
  }

}

void garcom_tchau(int tam_grupo) {
  if(tam_grupo % 4 == 0) {
    for(int i = 0; i<tam_grupo/4 ;i++)
      sem_post(&semMesasDisponiveis);
  }
  else{
    for(int i=0; i<tam_grupo/4 + 1; i++)
      sem_post(&semMesasDisponiveis);
  }
}

void garcom_chamar() {
  sem_wait(&semGarconsDisponiveis);
  sem_post(&semGarconsDisponiveis);

}

void* preparar_pizza(void* a){
  void* pedidot = queue_wait(smartDeck);
  pizza_t* pizza = pizzaiolo_montar_pizza((pedido_t*)pedidot);

  sem_wait(&semFornoDisponivel);
  pthread_mutex_lock(&mtxPa);
  pizzaiolo_colocar_forno(pizza);
  pthread_mutex_unlock(&mtxPa);
  pthread_exit(NULL);
}

void fazer_pedido(pedido_t* pedido) {
  queue_push_back(smartDeck, pedido);
  /*
  sem_wait(&semPizzaiolosDisponiveis);
  pizza_t* pizza = pizzaiolo_montar_pizza(pedido);
  sem_post(&semPizzaiolosDisponiveis);

  if(semFornoDisponivel > 0){
    sem_wait(&semPizzaiolosDisponiveis);
    pizzaiolo_colocar_forno(pizza);
    semFornoDisponivel--;
    sem_post(&semPizzaiolosDisponiveis);
  }*/
}

int pizza_pegar_fatia(pizza_t* pizza) {
  pthread_mutex_lock(&mtxPegador);
  if(pizza->fatias > 0){
    pizza->fatias--;
    pthread_mutex_unlock(&mtxPegador);
    return 0;
  } else {
    pthread_mutex_unlock(&mtxPegador);
    return -1;
  }
}
