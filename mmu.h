/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "defines.h"
#include "game.h"
#include <stdbool.h>


#define CODIGO_BASE       0X400000

#define MAPA_BASE_FISICA  0x500000
#define MAPA_BASE_VIRTUAL 0x800000
#define PAG_INICIAL 0x5A1000 //Todo: poner bien la página inicial



// void kernel_create_page_directory();
// void kernel_create_page_table();

void mmu_inicializar();
uint* mmu_inicializar_dir_pirata();
void mmu_mapear_pagina();
void mmu_desmapear_pagina();
uint* dame_pagina_libre();
uint* dame_pagina_unica();
uint mmu_pos_fisica();
void copiar_codigo_tarea();

#endif	/* !__MMU_H__ */
