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

#define PAG_INICIAL


int libres;
uint* base = (uint*) 0x100000;


void mmu_inicializar(){
  libres = 0;
}


uint* mmu_inicializar_dir_pirata(bool jugador, uint* pirata){
  uint* cr3 = dame_pagina_libre();
  uint* pt = dame_pagina_libre();
  // 0x003FFFFF
  //  0000 0000 00   00 0000 0000   0000 0000 0000
  // (0000 0000 00) (11 1111 1111) (1111 1111 1111)

  inicializar_idmap(cr3,pt);
  mmu_mapear_pagina(0x0400000, cr3, PAG_INICIAL);
  
}

//mmu_mapear_pagina(direccion virtual, puntero al pd, direccion fisica)
void mmu_mapear_pagina(uint* virtual, uint* cr3, uint* fisica){
  unsigned int directory_index = (unsigned int) ((virtual & FFC00000) / (2^21));
  uint* PDE = cr3[directory_index];
  bool seg_present = PDE & 1;

  if (seg_present = 0){
    uint* nueva_pd = dame_pagina_libre();
  }
  else {
    
  }

}

void mmu_desmapear_pagina(unsigned int virtual, unsigned int cr3){


}


uint* dame_pagina_libre(){
  uint* proxPagina = base + libres*0x1000;
  libres++;
  return proxPagina;
}

void kernel_create_page_directory() {
  int i = 0;
  uint *t = (uint*)(0x27000);
  t[i] = 0x28003;
  for (i=1 ; i < 1024 ; i++) {
    t[i] = 0x00000000;
  }
}
void kernel_create_page_table() {
  int i = 0;
  int direction = 0x00000003;
  uint *t = (uint*)(0x28000);
  for(i=0; i < 1024 ; i++) {
    t[i] =   direction;
    direction += 0x00001000;
  }
}

void task_create_page_table(){
  
}

void task_create_page_directory(){

}

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
