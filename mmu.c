/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
#include <stdbool.h>
/* Atributos paginas */
/* -------------------------------------------------------------------------- */

#define PAG_INICIAL 0x500000 //Todo: poner bien la página inicial
#define CACA (uint*)0xFEDEFA50DEAD
uint* dame_pagina_libre();
void inicializar_ident_mapping(uint* cr3, uint* pt);


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

  inicializar_ident_mapping(cr3,pt);

  mmu_mapear_pagina(0x0400000, &cr3, PAG_INICIAL);
  return CACA;  
}

void mmu_mapear_pagina(uint* virtual, uint** pcr3, uint* fisica){
  unsigned int directory_index = (unsigned int) (((unsigned int)virtual & 0xFFC00000) / (0x2^0x16)); // No sé si el casteo me hace la conversión a decimal
  uint* PDE = (uint*)(*pcr3)[directory_index]; //cr3 fue inicializado como entero sin signo, luego (*pcr3) recorre el array con offset de 4 bytes. (dir = base + tamaño del tipo*subinidice)
  bool present = (int)PDE & 0x1;
  uint* table;

  if (!present){
    table = dame_pagina_libre();
    PDE = (uint*)((unsigned int)PDE | (unsigned int)(table + 0x3)); // Guardo en la entrada del directorio de páginas la dirección base de la página que contiene la tabla de páginas y seteo los bits R/W y P en 1.
    (*pcr3)[directory_index] = (unsigned int)PDE; //Escribo en el directorio, porque PDE era una copia Agustín, prestá atención.
  }
  else {
    table = (uint*)(((unsigned int)PDE & 0x11111000)); // Limpio los bits de atributos. Queda la dirección física sola.
  }

// ¿Sabía Ud. que:
// a[i] is semantically equivalent to *(a+i), which in turn is equivalent to *(i+a), the expression can also be written as i[a] (although this form is rarely used).
// Como en assembly!
// KE

  unsigned int table_index = (unsigned int) (((unsigned int)virtual & 0x003FF000) / (0x2^0xC));
  uint* PTE = (uint*)table[table_index];
  present = (int)PTE & 0x1;
  uint* pagina;

  if (!present){
    pagina = dame_pagina_libre();
    PTE = (uint*)((unsigned int)PTE | ((unsigned int)pagina + (unsigned int)0x3)); // Guardo en la entrada de la tabla de páginas la dirección base de la página de 4K y seteo los bits R/W y P en 1. Bits 9, 10 y 11 [deberían, y] siempre van a ser cero porque los punteros a páginas son múltiplo de 4K así que siempre cierra todo bonito y contento.
    table[table_index] = (unsigned int)PTE; // Yo calculo que acá estoy escribiendo la tabla, y no la copia. Pero uno nunca sabe...
  }
  else {
    pagina = (uint*)((unsigned int)PDE & 0x11111000); // Limpio los bits de atributos. Queda la dirección física sola.
  }

  // La variable página no me estaría importando porque el offset dentro de la página
  // no lo estaría usando todavía. Pero, nuevamente, uno nunca sabe estas cosas.

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

void task_create_page_directory(){
  int i = 0;
  uint *t = (uint*)(0x27000);
  t[i] = 0x28003;
  for (i=1 ; i < 1024 ; i++) {
    t[i] = 0x00000000;
  }
}

void task_create_page_table(){
  int i = 0;
  int direction = 0x00000003;
  uint *t = (uint*)(0x28000);
  for(i=0; i < 1024 ; i++) {
    t[i] =   direction;
    direction += 0x00001000;
  }  
}


void inicializar_ident_mapping(uint* cr3, uint* pt){

}


/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
