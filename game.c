/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

*/

#include "game.h"
#include "mmu.h"
#include "tss.h"
#include "screen.h"

#include <stdarg.h>

#include "gdt.h"

#define POS_INIT_A_X                      1
#define POS_INIT_A_Y                      1
#define POS_INIT_B_X         MAPA_ANCHO - 2
#define POS_INIT_B_Y          MAPA_ALTO - 2

#define CANT_POSICIONES_VISTAS            9
#define MAX_SIN_CAMBIOS                 999

#define BOTINES_CANTIDAD 8


uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
                                        {30,  3, 50}, {30, 38, 50}, {15, 21, 100}, {45, 21, 100} ,
                                        {49,  3, 50}, {49, 38, 50}, {64, 21, 100}, {34, 21, 100}
                                    };
  
jugador_t jugadorA;
jugador_t jugadorB;


void* error()
{
	__asm__ ("int3");
	return 0;
}

uint game_xy2lineal (uint x, uint y) {
	return (y * MAPA_ANCHO + x);
}

uint game_posicion_valida(int x, int y) {
	return (x >= 0 && y >= 0 && x < MAPA_ANCHO && y < MAPA_ALTO);
}

pirata_t* id_pirata2pirata(uint id_pirata)
{
	pirata_t *p;
  if (id_pirata < 8){
  p = &(jugadorA.piratas[id_pirata]);
  } else {
  int i = id_pirata % 8;
  p = &(jugadorB.piratas[i]);
  }
  return p;
}

uint game_dir2xy(direccion dir, int *x, int *y)
{
	switch (dir)
	{
		case IZQ: *x = -1; *y =  0; break;
		case DER: *x =  1; *y =  0; break;
		case ABA: *x =  0; *y =  1; break;
		case ARR: *x =  0; *y = -1; break;
    	default: return -1;
	}

	return 0;
}

uint game_valor_tesoro(uint x, uint y)
{
	int i;
	for (i = 0; i < BOTINES_CANTIDAD; i++)
	{
		if (botines[i][0] == x && botines[i][1] == y)
			return botines[i][2];
	}
	return 0;
}

// dada una posicion (x,y) guarda las posiciones de alrededor en dos arreglos, uno para las x y otro para las y
void game_calcular_posiciones_vistas(int *vistas_x, int *vistas_y, int x, int y)
{
	int next = 0;
	int i, j;
	for (i = -1; i <= 1; i++)
	{
		for (j = -1; j <= 1; j++)
		{
			vistas_x[next] = x + j;
			vistas_y[next] = y + i;
			next++;
		}
	}
}


void game_inicializar()
{
  game_jugadores_inicializar(&jugadorA, &jugadorB);
}

void game_jugador_inicializar_mapa(jugador_t *jug)
{
}

void game_jugadores_inicializar(jugador_t *jA, jugador_t *jB)
{
//	static int index = 0;
//
//	j->index = index++;
    // ~ completar ~
  int index_gdt;
  int i = 0;
  for (i=0; i<8; i++){
    index_gdt = 15+i;
    game_pirata_inicializar(&(jA->piratas[i]), jA, index_gdt, i);
  }
  for (i=8; i<16; i++) {
    index_gdt = 15+i;
    game_pirata_inicializar(&(jB->piratas[i]), jB, index_gdt, i);
  }
  jA->puntaje = 0;
  jB->puntaje = 0;
}

void game_pirata_inicializar(pirata_t *pirata, jugador_t *j, uint index, uint id)
{
  pirata->id = id;
  pirata->index_gdt = index;
  pirata->estaVivo = 0 ;
  pirata->pos_x = 78; //el tablero va de 0 a 79 y de 0 a 54
  pirata->pos_y = 1;
  pirata->jugador = j; 

  tss *t = (tss*)(gdt[index].base_0_15 + ((gdt[index].base_23_16) << 16) + ((gdt[index].base_31_24) << 24)); //saco la direccion base del descriptor de tss en la GDT, que es donde deberia estar la tss.

   uint cr3 = (uint) mmu_inicializar_dir_pirata();
   uint pila0 = (uint) dame_pagina_libre();
   completar_tss(t, cr3, pila0);

}

void game_tick(uint id_pirata)
{
  screen_actualizar_reloj_global();
}
//devuelve la posicion fisica en la cual se va a parar el pirata
uint dame_pos_fisica(pirata_t *p, direccion dir){
  uint fisica;
  uint index = p->index_gdt;
  tss *t = (tss*)(gdt[index].base_0_15 + ((gdt[index].base_23_16) << 16) + ((gdt[index].base_31_24) << 24));
  uint pcr3 = t->cr3;
  if (dir == IZQ){
    uint actual = mmu_pos_fisica(pcr3,0x400000);
    fisica = actual - 4096; //(le resto 4kb)
  }
  if (dir == DER){
    uint actual = mmu_pos_fisica(pcr3,0x400000);
    fisica = actual + 4096; //(le sumo 4kb)
  }
  if (dir == ABA){
    uint actual = mmu_pos_fisica(pcr3,0x400000);
    fisica = actual + MAPA_ANCHO * 4096; 
  }
  if (dir == ARR){
    uint actual = mmu_pos_fisica(pcr3,0x400000);
    fisica = actual - MAPA_ANCHO * 4096; 
  }
  return fisica;  
}

void prueba_lanzar_pirata(){
  // jugadorA.piratas[0]
  // mmu_mapear_pagina();
  copiar_codigo_tarea((int*)0x400000);
  print("HARE", 50, 3, 16);
  // copiar_codigo_tarea((int*)PAG_INICIAL);

}
// void game_pirata_relanzar(pirata_t* pPirata, jugador_t* pJ, uint tipo)
// {
//   reiniciar_tss_tarea(pPirata);
// }

pirata_t* game_jugador_erigir_pirata(jugador_t *j, uint tipo)
{
    // ~ completar ~

	return NULL;
}


void game_jugador_lanzar_pirata(jugador_t *j, uint tipo, int x, int y)
{
}

void game_pirata_habilitar_posicion(jugador_t *j, pirata_t *pirata, int x, int y)
{
}


void game_explorar_posicion(jugador_t *jugador, int c, int f)
{
//uint game_dir2xy(direccion dir, int *x, int *y)
//le paso las direc que me tire esta funcion
}


uint game_syscall_pirata_mover(uint id, direccion dir)
{
  pirata_t *p = id_pirata2pirata(id); //esta funcion hay que hacerla
  // uint index = p->index_gdt;
  // tss *t = (tss*)(gdt[index].base_0_15 + ((gdt[index].base_23_16) << 16) + ((gdt[index].base_31_24) << 24));
  // uint pcr3 = t->cr3;
  int *x = &(p->pos_x);
  int *y = &(p->pos_y);
  game_dir2xy(dir,x,y); //convierte la pos actual y una direc en la nueva pos
  // if (game_posicion_valida(*x,*y)){ //pregunto si ese movimiento me deja en una pos valida del mapa
  //   if (p->tipo == explorador){
  //     uint fisica = dame_pos_fisica(p,dir);
  //     mmu_mapear_pagina((uint*)0x400000,&pcr3,&fisica); //primero mapeo y dsp copio codigo no????
  //     copiar_codigo_tarea((uint*)0x400000); 

  //   }
  // } 
  print("HARE", 50, (int)x,(int) y);
    return 0;
}

uint game_syscall_cavar(uint id)
{
    // ~ completar ~

	return 0;
}

uint game_syscall_pirata_posicion(uint id, int idx) //ver que onda los ids de los piratas de distintos equipos
{
  uint pos;
  pirata_t *p = id_pirata2pirata(id); //ver si esta funcion funca :D
  if (idx == -1){ 
    pos = (p->pos_y << 8) | p->pos_x; 
  } else {
    jugador_t *j = p->jugador;
    pirata_t p1 = j->piratas[idx]; //dudoso esto, no se si a esto se refiere con el idx que me pasan
    pos = (p1.pos_y << 8) | p1.pos_x; 
  }  
  return pos;
}

uint game_syscall_manejar(uint syscall, uint param1)
{
    // ~ completar ~
    return 0;
}

void game_pirata_exploto(uint id)
{
}

pirata_t* game_pirata_en_posicion(uint x, uint y)
{
	return NULL;
}


void game_jugador_anotar_punto(jugador_t *j)
{
  j->puntaje = j->puntaje + 1;
}



void game_terminar_si_es_hora()
{
}


#define KB_w_Aup    0x11 // 0x91
#define KB_s_Ado    0x1f // 0x9f
#define KB_a_Al     0x1e // 0x9e
#define KB_d_Ar     0x20 // 0xa0
#define KB_e_Achg   0x12 // 0x92
#define KB_q_Adir   0x10 // 0x90
#define KB_i_Bup    0x17 // 0x97
#define KB_k_Bdo    0x25 // 0xa5
#define KB_j_Bl     0x24 // 0xa4
#define KB_l_Br     0x26 // 0xa6
#define KB_shiftA   0x2a // 0xaa
#define KB_shiftB   0x36 // 0xb6


void game_atender_teclado(unsigned char tecla)
{
}
