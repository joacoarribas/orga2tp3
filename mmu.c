/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
/* Atributos paginas */
/* -------------------------------------------------------------------------- */
void create_page_directory() {
  int i = 0;
  uint *t = (uint*)(0x27000);
  t[i] = 0x28003;
  for (i=1 ; i < 1024 ; i++) {
    t[i] = 0x00000000;
  }
}
void create_page_table() {
  int i = 0;
  int direction = 0x00000003;
  uint *t = (uint*)(0x28000);
  for(i=0; i < 1024 ; i++) {
    t[i] =   direction;
    direction += 0x00001000;
  }
}


/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
