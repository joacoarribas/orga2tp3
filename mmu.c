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

#define PAG_INICIAL 0x500000 //Todo: poner bien la página inicial
#define CACA (uint*)0xFEDEFA50DEAD
uint* dame_pagina_libre();
void inicializar_ident_mapping(uint* cr3, uint* pt);
void copiar_codigo_tarea();
void cerear_pagina(uint* dir);

int libres;
uint* base = (uint*) 0x100000;


void mmu_inicializar(){
  libres = 0;
}

uint* mmu_inicializar_dir_pirata(){
  uint* cr3 = dame_pagina_libre();
  uint* pt = dame_pagina_libre();
  // 0x003FFFFF
  //  0000 0000 00   00 0000 0000   0000 0000 0000
  // (0000 0000 00) (11 1111 1111) (1111 1111 1111)0x0400000
  inicializar_ident_mapping(cr3,pt);
  mmu_mapear_pagina(0x400000, &cr3, PAG_INICIAL); //Mapeo a la dirección virtual 0x0400000 de la tarea, una página de lectura-escritura (para su código y su pila) que apunta a la memoria física de su ubicación en el mapa.
  // copiar_codigo_tarea((int*)PAG_INICIAL);
  return cr3;
}
  //print_hex(*cr3, 15, 10, 10, 15);
  //print_hex((uint)cr3, 16, 7, 7, 15);
  //print_hex(virtual, 15, 13, 13, 15);
  //
  //print("PDE_index", 40, 40, 15);
  //print_hex(PDE_index, 15, 41, 41, 15);
  //
  //print("*PDE", 30, 30, 15);
  //print_hex(*PDE, 15, 31, 31, 15);
  //print("PDE", 33, 33, 15);
  //print_hex((uint)PDE, 15, 34, 34, 15);
  //
  //print("aux", 12, 12, 15);
  //print_hex((uint)aux, 15, 13, 13, 15);
  //print("*aux", 15, 15, 15);
  //print_hex((uint)aux, 15, 16, 16, 15);
  //print("aux", 9, 9, 15);
  //print_hex((uint)aux, 15, 10, 10, 15);
  //print("fisica", 3, 3, 15);
  //print_hex(fisica, 15, 4, 4, 15);
  //
  //print("*PTE", 6, 6, 15);
  //print_hex(*PDE, 15, 7, 7, 15);
  //print("PTE", 9, 9, 15);
  //print_hex((uint)PDE, 15, 10, 10, 15);
  //
  //print("aux", 12, 12, 15);
  //print_hex((uint)aux, 15, 13, 13, 15);
  //print("*aux", 15, 15, 15);
  //print_hex((uint)*aux, 15, 16, 16, 15);

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
  unsigned int directory_index = (unsigned int) (((unsigned int)virtual & 0xFFC00000) / (0x2^0x16)); // No sé si el casteo me hace la conversión a decimal
  cr3[directory_index] = 0x00000000;
  // El memory leak ya fue vieja, no me importa nada.
}


uint* dame_pagina_libre(){
  uint* proxPagina = base + libres*0x1000;
  libres++;
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

void copiar_codigo_tarea(int* dir){
  int i;
  for ( i=0x00; i<0x400; i++){
    *(dir+i) = *((int*)(0x10000+i*4));
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
