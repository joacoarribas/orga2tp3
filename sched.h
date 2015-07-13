/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler

*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "game.h"
#include "mmu.h"

void inicializar_sched();
int sched_proxima_a_ejecutar(); 
int sched_tick();
void sched_generar_pirata_jugadorA();
void sched_generar_pirata_jugadorB();
void sched_activar_debbuger();
void sched_desactivar_debbuger(); //no se si es necesaria
uint sched_estado_debbuger();
void sched_setear_debbuger();
uint sched_d_seteado();
void arreglar_tecla();
void reanudar_juego();
int sched_todos_vivos();
#endif	/* !__SCHED_H__ */
