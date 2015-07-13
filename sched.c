/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler

*/

#include "sched.h"
#include "screen.h"
#include "i386.h"

jugador_t* scheduler[2];
int indice_actual;
int debug_activo; //si entro a modo debug 1
int debug_seteado; //si quiero que entre a modo debug cuando se produzca una excepcion 1

void inicializar_sched() {
  scheduler[0] = &jugadorA;
  scheduler[1] = &jugadorB;

  indice_actual = 0;
  debug_activo = 0; //0 no esta activo, 1 esta activo
  debug_seteado = 0;
}

void sched_activar_debbuger(){
  debug_activo = 1;
  }

//esta no se si es necesaria

void sched_desactivar_debbuger(){
  debug_activo = 0;
  }

void sched_setear_debbuger(){
  debug_seteado = 1;
  }  

void sched_unsetear_debbuger(){
  debug_seteado = 0;
  }

uint sched_estado_debbuger(){ //si es cero, esta desactivado, si es uno, esta activado
  return debug_activo;
  }

void reanudar_juego() {
  debug_activo = 0;
}

uint sched_d_seteado(){
  return debug_seteado;
  }

int sched_dame_proximo_pirata(jugador_t *j) {
  int res = 0x70;
  int encontre = 0;
  int todosMuertos = 0;
  int index = 0;

  // Me fijo si estan todos muertos
  while ((index < 8) & (j->piratas[index].estaVivo == 0)) {
    index++;
  }

  if (index == 8)
    todosMuertos = 1;

    // Pre : hay por lo menos una tarea para correr 
  if (todosMuertos == 0) {
    index = 0;
    while ((index < 8) & (encontre == 0)) {
      // Si el pirata esta muerto -> avanzo
      if (j->piratas[index].estaVivo == 0) {
        index++;
      } else {
        // Si el pirata esta vivo, pero ya corrio -> avanzo
        if (j->piratas[index].ya_corrio == 1) {
          index++;
        // Si el pirata esta vivo y no corrio -> No aumento el indice y sigo 
        } else {
          encontre = 1;
        }
      }
    }


    if (index == 8) { // Si ya corrieron todas las tareas que estaban vivas
      int i = 0;
      // Seteo el ya_corrio de todas a cero
      for (i=0 ; i<8 ; i++) {
        //if (j->piratas[i].estaVivo == 1)
          j->piratas[i].ya_corrio = 0; 
      }

      // Busco la primera que este viva y la devuelvo
      i = 0;
      while (j->piratas[i].estaVivo == 0)
        i++;

      res = j->piratas[i].index_gdt;
      j->piratas[i].ya_corrio = 1;
    
    } else {
      res = j->piratas[index].index_gdt;
      j->piratas[index].ya_corrio = 1;
    }
  }

  return res;
}

void hay_minero_pendiente(jugador_t *jug) {
  int i = 0;
  int todosVivos = 1;
  int index_minero_pendiente = 0;
  int target_x;
  int target_y;
  while ((index_minero_pendiente<8) & (jug->minerosPendientes[index_minero_pendiente].estaVivo == 0)) {
    index_minero_pendiente++;
  }

  if (index_minero_pendiente < 8) {

    i = 0;
    while ((i < 8) & (todosVivos == 1)) {
      if (jug->piratas[i].estaVivo == 0) {
        todosVivos = 0;
      }
      i++;
    } 
    if (todosVivos == 0) {
      i = 0;
      while ((i < 8) & (jug->piratas[i].estaVivo == 1)) 
        i++;

      jug->minerosPendientes[index_minero_pendiente].estaVivo = 0;
      jug->piratas[i].estaVivo = 1;
      jug->piratas[i].ya_corrio = 0;
      jug->piratas[i].exploto = 1;
      jug->piratas[i].tipo = 1;
      target_x = jug->minerosPendientes[index_minero_pendiente].target_x;
      target_y = jug->minerosPendientes[index_minero_pendiente].target_y;
      jug->piratas[i].target_x = jug->minerosPendientes[index_minero_pendiente].target_x;
      jug->piratas[i].target_y = jug->minerosPendientes[index_minero_pendiente].target_y;
      jug->piratas[i].pos_x = jug->minerosPendientes[index_minero_pendiente].pos_x;
      jug->piratas[i].pos_y = jug->minerosPendientes[index_minero_pendiente].pos_y;

      mmu_inicializar_dir_pirata(&(jug->piratas[i]), target_x, target_y);
    }
  
  }

}

int sched_proxima_a_ejecutar() {
  int res = 0x70;


  if (indice_actual == 0){
    indice_actual++;
  } else {
    indice_actual--;
  }

  hay_minero_pendiente(scheduler[indice_actual]);
  res = sched_dame_proximo_pirata(scheduler[indice_actual]);

  if (res == 0x70){

    if (indice_actual == 0){
      indice_actual++;
    } else {
      indice_actual--;
    }

    hay_minero_pendiente(scheduler[indice_actual]);
    res = sched_dame_proximo_pirata(scheduler[indice_actual]);
  }

  return res;
}

void sched_generar_pirata_jugadorA(){
  int i = 0;
  int todosVivos = 1;
  jugador_t *jug = scheduler[0];
  while ((i < 8) & (todosVivos == 1)) {
    if (jug->piratas[i].estaVivo == 0) {
      todosVivos = 0;
    }
    i++;
  } 
  if (todosVivos == 0)
    game_jugador_lanzar_pirata(jug, 0, POS_INIT_A_X, POS_INIT_A_Y);
}
int sched_todos_vivos(){
  int i = 0;
  int todosVivos = 1;
  jugador_t *jug = scheduler[0];
  while ((i < 8) & (todosVivos == 1)) {
    if (jug->piratas[i].estaVivo == 0) {
      todosVivos = 0;
    }
    i++;
  } 
  jug = scheduler[1];
  while ((i < 8) & (todosVivos == 1)) {
    if (jug->piratas[i].estaVivo == 0) {
      todosVivos = 0;
    }
    i++;
  } 

  return todosVivos;
}

void sched_generar_pirata_jugadorB(){
  int i = 0;
  int todosVivos = 1;
  jugador_t *jug = scheduler[1];
  while ((i < 8) & (todosVivos == 1)) {
    if (jug->piratas[i].estaVivo == 0) {
      todosVivos = 0;
    }
    i++;
  } 
  if (todosVivos == 0)
    game_jugador_lanzar_pirata(jug, 0, POS_INIT_B_X, POS_INIT_B_Y);
}

int sched_tick() {
  game_tick();

  return sched_proxima_a_ejecutar();
}



