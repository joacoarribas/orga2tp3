/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
#include "screen.h"
#include <stdbool.h>
/* Atributos paginas */
/* -------------------------------------------------------------------------- */

#define CACA (uint*)0xFEDEFA50DEAD
uint* dame_pagina_libre();
void copiar_codigo_tarea();
void cerear_pagina(uint* dir);

int libres;
uint* base = (uint*) 0x100000;


void mmu_inicializar(){
  libres = 0;
}

void mmu_inicializar_dir_pirata(pirata_t *p, int x, int y){
  // 0x003FFFFF
  //  0000 0000 00   00 0000 0000   0000 0000 0000
  // (0000 0000 00) (11 1111 1111) (1111 1111 1111)0x0400000
  jugador_t *j = p->jugador; 
  uint cr3 = p->cr3;
  uint kernel_cr3 = 0x27000;

  if (j == &(jugadorA)) {
    p->fisica_actual = PAG_INICIAL_A;

    if (p->tipo == 0) {
      //breakpoint();
      mmu_mapear_pagina(0x400000, &cr3, PAG_INICIAL_A); //Mapeo a la dirección virtual 0x0400000 de la tarea, una página de lectura-escritura (para su código y su pila) que apunta a la memoria física de su ubicación en el mapa.
      mmu_mapear_pagina(0x3FF000, &kernel_cr3, PAG_INICIAL_A); //primero mapeo y dsp copio codigo no????
      copiar_codigo_tarea((int*)0x3FF000, (int*)0x10000);
      mmu_mapear_pagina(0x3FF000, &kernel_cr3, 0x3FF000); //primero mapeo y dsp copio codigo no????
    } else {
      mmu_mapear_pagina(0x400000, &cr3, PAG_INICIAL_A); //Mapeo a la dirección virtual 0x0400000 de la tarea, una página de lectura-escritura (para su código y su pila) que apunta a la memoria física de su ubicación en el mapa.
      mmu_mapear_pagina(0x3FF000, &kernel_cr3, PAG_INICIAL_A); //primero mapeo y dsp copio codigo no????
      copiar_codigo_tarea((int*)0x3FF000, (int*)0x11000);
      int *pstack =  (int*) (0x3FF000 + 0x1000 -12);
      *pstack = 0x0;
      pstack++;
      *pstack = x;
      pstack++;
      *pstack = y;
      mmu_mapear_pagina(0x3FF000, &kernel_cr3, 0x3FF000); //primero mapeo y dsp copio codigo no????
    }
  } else {
    p->fisica_actual = PAG_INICIAL_B;

    if (p->tipo == 0) {
      mmu_mapear_pagina(0x400000, &cr3, PAG_INICIAL_B); //Mapeo a la dirección virtual 0x0400000 de la tarea, una página de lectura-escritura (para su código y su pila) que apunta a la memoria física de su ubicación en el mapa.
      mmu_mapear_pagina(0x3FF000, &kernel_cr3, PAG_INICIAL_B); //primero mapeo y dsp copio codigo no????
      copiar_codigo_tarea((int*)0x3FF000, (int*)0x12000);
      mmu_mapear_pagina(0x3FF000, &kernel_cr3, 0x3FF000); //primero mapeo y dsp copio codigo no????
    } else {
      mmu_mapear_pagina(0x400000, &cr3, PAG_INICIAL_B); //Mapeo a la dirección virtual 0x0400000 de la tarea, una página de lectura-escritura (para su código y su pila) que apunta a la memoria física de su ubicación en el mapa.
      mmu_mapear_pagina(0x3FF000, &kernel_cr3, PAG_INICIAL_B); //primero mapeo y dsp copio codigo no????
      copiar_codigo_tarea((int*)0x3FF000, (int*)0x13000);
      int *pstack =  (int*) (0x3FF000 + 0x1000 -12);
      *pstack = 0x0;
      pstack++;
      *pstack = x;
      pstack++;
      *pstack = y;
      mmu_mapear_pagina(0x3FF000, &kernel_cr3, 0x3FF000); //primero mapeo y dsp copio codigo no????
    }
  }

}

uint mmu_pos_fisica(uint* cr3, uint virtual){
  uint fisica;
  unsigned int directory_index = virtual >> 22; 
  uint* PDE = (uint*)cr3[directory_index]; //cr3 fue inicializado como entero sin signo, luego (*pcr3) recorre el array con offset de 4 bytes. (dir = base + tamaño del tipo*subinidice)
  uint* table;

  table = (uint*)(((unsigned int)PDE & 0xFFFFF000)); // Limpio los bits de atributos. Queda la dirección física sola.

  unsigned int table_index = ((virtual & 0x003FF000) >> 12);
  uint* PTE = (uint*)table[table_index];
 
  fisica = (((uint)PTE & 0xFFFFF000) + ((unsigned int)virtual & 0x00000FFF)); 

  return fisica;
}

void mmu_mapear_pagina(uint* virtual, uint** pcr3, uint* fisica){

  unsigned int directory_index = (unsigned int)virtual >> 22; // No sé si el casteo me hace la conversión a decimal
  // unsigned int directory_index = (unsigned int) (((unsigned int)virtual & 0xFFC00000) / (0x2^0xD)); // No sé si el casteo me hace la conversión a decimal
  uint* PDE = (uint*)(*pcr3)[directory_index]; //cr3 fue inicializado como entero sin signo, luego (*pcr3) recorre el array con offset de 4 bytes. (dir = base + tamaño del tipo*subinidice)
  int present = (int)PDE & 0x1;
  uint* table;

  if (present==0){
    table = dame_pagina_libre();
    PDE = (uint*)((unsigned int)(table) + 0x7); // Guardo en la entrada del directorio de páginas la dirección base de la página que contiene la tabla de páginas y seteo los bits R/W y P en 1.
    (*pcr3)[directory_index] = (unsigned int)PDE; //Escribo en el directorio, porque PDE era una copia Agustín, prestá atención.
  }
  else {
    table = (uint*)(((unsigned int)PDE & 0xFFFFF000)); // Limpio los bits de atributos. Queda la dirección física sola.
  }

  unsigned int table_index = (unsigned int) (((unsigned int)virtual & 0x003FF000) >> 12);
  uint* PTE = (uint*)table[table_index];

  PTE = (uint*)((unsigned int)(fisica) + (unsigned int)(0x7)); // Guardo en la entrada de la tabla de páginas la dirección base de la página de 4K y seteo los bits R/W y P en 1. Bits 9, 10 y 11 [deberían, y] siempre van a ser cero porque los punteros a páginas son múltiplo de 4K así que siempre cierra todo bonito y contento.
  table[table_index] = (unsigned int)PTE; // Yo calculo que acá estoy escribiendo la tabla, y no la copia. Pero uno nunca sabe...
  tlbflush();
}

void mmu_desmapear_pagina(unsigned int virtual, uint* cr3){
  unsigned int directory_index = (unsigned int)virtual >> 22; // No sé si el casteo me hace la conversión a decimal
  //uint* PDE = (uint*)cr3[directory_index]; //cr3 fue inicializado como entero sin signo, luego (*pcr3) recorre el array con offset de 4 bytes. (dir = base + tamaño del tipo*subinidice)
  //uint* table;

  //table = (uint*)(((unsigned int)PDE & 0xFFFFF000)); // Limpio los bits de atributos. Queda la dirección física sola.

  //cerear_pagina(table);

  //unsigned int table_index = (unsigned int) (((unsigned int)virtual & 0x003FF000) >> 12);
  //table[table_index] = 0x00000000;

  cr3[directory_index] = 0x00000000;
  // El memory leak ya fue vieja, no me importa nada.
}


uint* dame_pagina_libre(){
  uint* proxPagina = base + libres*0x1000;
  libres++;
  cerear_pagina(proxPagina);
  return proxPagina;
}

uint* dame_pagina_unica(){
  uint* proxPagina = base + libres*0x1000;
  cerear_pagina(proxPagina);
  return proxPagina;
}

void idmap_page_directory(uint* dir) {
  int i = 0;
  uint *t = dir;
  t[i] = 0x28003;
  for (i=1 ; i < 1024 ; i++) {
    t[i] = 0x00000000;
  }
}

void idmap_page_table(uint* dir) {
  int i = 0;
  int direction = 0x00000003;
  uint *t = dir;
  for(i=0; i < 1024 ; i++) {
    t[i] =   direction;
    direction += 0x00001000;
  }
}

void inicializar_ident_mapping(uint* cr3, uint* pt){
  idmap_page_directory((uint*)cr3);
  idmap_page_table(pt);
}

void kernel_create_page_directory(){
  idmap_page_directory((uint*)0x27000);
}

void kernel_create_page_table(){
  idmap_page_table((uint*)0x28000);
}

void copiar_codigo_tarea(int* dirDest, int* dirSrc){
  int i;
  for ( i=0x00; i<0x400; i++){
    *(dirDest+i) = *(dirSrc+i);
  }
}

void cerear_pagina(uint* dir){
  int i;
  for(i=0x0; i<0x1000; i++){
    dir[i] = 0x00;
  }
}
/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */

  //if (p->ya_corrio == 0) {
  //  int i = 0;
  //  while (j->fisicas_vistas[i] != 0) {
  //    uint fisica_a_mapear = j->fisicas_vistas[i];
  //    uint logica = fisica_a_mapear + 0x300000;
  //    mmu_mapear_pagina(logica, &cr3, fisica_a_mapear);
  //    i++;
  //  }
  //} else {
  //  p->ya_corrio = 0;
  //}
  // Le mapeo las 9 posiciones iniciales y más
