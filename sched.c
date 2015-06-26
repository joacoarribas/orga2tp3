/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler

*/

#include "sched.h"
#include "i386.h"

jugador_t scheduler[2];
int indice_actual;

void inicializar_sched() {
  scheduler[0] = jugadorA;
  scheduler[1] = jugadorB;

  indice_actual = 0;
}

int sched_proxima_a_ejecutar() {
  if (indice_actual == 0)
    indice_actual++;
  if (indice_actual == 1)
    indice_actual--;
  
  int index = scheduler[indice_actual].index;

  while (scheduler[indice_actual].piratas[index].estaVivo == 0 ) { //mientras el pirata este muerto, avanzar al siguiente
    index++;
  }
//  return index;
  return scheduler[indice_actual].piratas[index].index_gdt; //devuelve indice en la GDT
}

int sched_tick() {
  int index = scheduler[indice_actual].index;
  int id = scheduler[indice_actual].piratas[index].id;

  game_tick(id);

  return sched_proxima_a_ejecutar();
}
