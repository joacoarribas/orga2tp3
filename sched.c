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
  int i = 0;
  for (i=0; i<8; i++){
    jugadorA.piratas[i].index_gdt = 0x78+(i*8);
    jugadorA.piratas[i].jugador = &jugadorA; 
    jugadorA.piratas[i].id = i; 
  }
  for (i=8; i<16; i++) {
    jugadorB.piratas[i].index_gdt = 0x78+(i*8);
    jugadorB.piratas[i].jugador = &jugadorB; 
    jugadorB.piratas[i].id = i; 
  }
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

  while (scheduler[indice_actual].piratas[index].estaVivo == 0 ) {
    index++;
  }
//  return index;
  return scheduler[indice_actual].piratas[index].index_gdt;
}

int sched_tick() {
  int index = scheduler[indice_actual].index;
  int id = scheduler[indice_actual].piratas[index].id;

  game_tick(id);

  return sched_proxima_a_ejecutar();
}
