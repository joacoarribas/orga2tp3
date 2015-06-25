/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "game.h"

void inicializar_sched();
int sched_proxima_a_ejecutar(); 
int sched_tick();


#endif	/* !__SCHED_H__ */
