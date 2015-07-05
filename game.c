/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

*/

#include "game.h"
#include "mmu.h"
#include "tss.h"
#include "screen.h"
#include "i386.h"

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

int game_id(int index){
  int aux = (index & 0xfff8) >> 3; //cereo atributos
  return aux - 15;  //le resto 15 porque los inicialice desde el indice 15
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
  pirata->pos_x = 1; //el tablero va de 0 a 79 y de 0 a 54
  pirata->pos_y = 2;
  pirata->jugador = j; 

  //tss *t = (tss*)(gdt[index].base_0_15 + ((gdt[index].base_23_16) << 16) + ((gdt[index].base_31_24) << 24)); //saco la direccion base del descriptor de tss en la GDT, que es donde deberia estar la tss.

  uint cr3 = (uint) mmu_inicializar_dir_pirata();
  uint pila0 = (uint) dame_pagina_libre();
  completar_tss(id, cr3, pila0);
}

void game_tick(uint id_pirata)
{
  screen_actualizar_reloj_global();
}
//devuelve la posicion fisica en la cual se va a parar el pirata
uint dame_siguiente_pos_fisica(uint actual, direccion dir){
  uint fisica;
  if (dir == IZQ){
    fisica = actual - 4096; //(le resto 4kb)
  }
  if (dir == DER){
    fisica = actual + 4096; //(le sumo 4kb)
  }
  if (dir == ABA){
    fisica = actual + MAPA_ANCHO * 4096; 
  }
  if (dir == ARR){
    fisica = actual - MAPA_ANCHO * 4096; 
  }
  return fisica;
}

void prueba_lanzar_pirata(){
  // jugadorA.piratas[0]
  // mmu_mapear_pagina();
  copiar_codigo_tarea((int*)0x400000, (int*)0x10000);
  //print("HARE", 50, 3, 16);
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
  uint index = p->index_gdt;
  tss *t = (tss*)(gdt[index].base_0_15 + ((gdt[index].base_23_16) << 16) + ((gdt[index].base_31_24) << 24));
  uint cr3 = t->cr3;
  // LEVANTA BIEN EL CR3 Y SU ID TODO PIO WACHO
  int x = 0; 
  int y = 0;
  //uint fisica1;
  //uint fisica2;

  print("E", p->pos_x, p->pos_y, 2);
  game_dir2xy(dir,&x,&y); //convierte la pos actual y una direc en la nueva pos
  p->pos_x = p->pos_x + x;
  p->pos_y = p->pos_y + y;
  print("E",p->pos_x, p->pos_y,2);
  if (game_posicion_valida(p->pos_x,p->pos_y)){ //pregunto si ese movimiento me deja en una pos valida del mapa
  //  if (p->tipo == explorador){
      uint actual = mmu_pos_fisica(cr3,0x400000);
      uint fisica_a_moverse = dame_siguiente_pos_fisica(actual, dir);
      if (dir == IZQ){
        //fisica1 = actual - MAPA_ANCHO * 4096 - 4096; // Arriba de la cual te moves
        //fisica2 = actual + MAPA_ANCHO * 4096 - 4096; // Abajo de la cual te moves
        screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x);
        screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x);
      }
      if (dir == DER){
        //fisica1 = actual - MAPA_ANCHO * 4096 + 4096; // Arriba de la cual te moves
        //fisica2 = actual + MAPA_ANCHO * 4096 + 4096; // Abajo de la cual te moves
        screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x);
        screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x);
      }
      if (dir == ABA){
        //fisica1 = actual + MAPA_ANCHO * 4096 - 4096; // Izquierda
        //fisica2 = actual + MAPA_ANCHO * 4096 + 4096; // Derecha
        screen_pintar(32,C_BG_GREEN, p->pos_y,p->pos_x-1);
        screen_pintar(32,C_BG_GREEN, p->pos_y,p->pos_x+1);
      }
      if (dir == ARR){
        //fisica1 = actual - MAPA_ANCHO * 4096 - 4096; // Izquierda 
        //fisica2 = actual - MAPA_ANCHO * 4096 + 4096; // Derecha 
        screen_pintar(32,C_BG_GREEN, p->pos_y,p->pos_x-1);
        screen_pintar(32,C_BG_GREEN, p->pos_y,p->pos_x+1);
      }
  //    breakpoint();
      uint* virtual = dame_pagina_unica();

      mmu_mapear_pagina(virtual, &cr3, actual); //primero mapeo y dsp copio codigo no????
      mmu_mapear_pagina(0x400000, &cr3, fisica_a_moverse); //primero mapeo y dsp copio codigo no????
      copiar_codigo_tarea((int*)0x400000, virtual);
      //print_hex(actual,15, 2, 2,15);
      //print_hex(fisica_a_moverse,15, 4, 4,15);
      //print_hex(fisica1,15, 6, 6,15);
      //print_hex(fisica2,15, 8, 8,15);

//      breakpoint();

    }
//  } 
    return 0;
}

      //print_hex(actual,15, 2, 2,15);
      //print_hex(fisica,15, 4, 4,15);

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
