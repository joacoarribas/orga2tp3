/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler

*/

#include "sched.h"
//#include "game.h"
#include "screen.h"
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

  while ((index < 8) & (scheduler[indice_actual].piratas[index].estaVivo == 0)) { //mientras el pirata este muerto, avanzar al siguiente
    index++;
  }

  if (index < 8) {
    uint hola = scheduler[indice_actual].piratas[index].index_gdt; //devuelve indice en la GDT
    print_hex((uint)hola, 5, 11, 11, 3);
    print_hex((uint)index, 5, 12, 12, 3);
    return hola; 
  }

  if (indice_actual == 0)
    indice_actual++;
  if (indice_actual == 1)
    indice_actual--;

  while ((index < 8) & (scheduler[indice_actual].piratas[index].estaVivo == 0)) { //mientras el pirata este muerto, avanzar al siguiente
    index++;
  }

  if (index < 8) {
    uint hola = scheduler[indice_actual].piratas[index].index_gdt; //devuelve indice en la GDT
    print_hex((uint)hola, 5, 14, 14, 3);
    return hola; 
  }

  return 0x70;
}

void sched_generar_pirata_jugadorA(){
  game_jugador_lanzar_pirata(&(scheduler[0]), 0, POS_INIT_A_X, POS_INIT_A_Y);
}

void sched_generar_pirata_jugadorB(){
  game_jugador_lanzar_pirata(&(scheduler[1]), 0, POS_INIT_B_X, POS_INIT_A_Y);
}

void sched_ejecutar_tarea(int index_gdt){
  int id_pirata = game_id(index_gdt);
  pirata_t *p = id_pirata2pirata(id_pirata);
  prueba_lanzar_pirata(p);

}

int sched_tick() {
  int index = scheduler[indice_actual].index;
  int id = scheduler[indice_actual].piratas[index].id;
  print_hex((uint)id, 5, 5, 5, 3);

  game_tick(id);
  uint hola =  sched_proxima_a_ejecutar();
  print_hex((uint)hola, 5, 7, 7, 3);


  return hola;
}

