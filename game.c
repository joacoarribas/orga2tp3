/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

*/

#include "game.h"
#include "sched.h"
#include "mmu.h"
#include "tss.h"
#include "screen.h"
#include "i386.h"

#include <stdarg.h>

#include "gdt.h"

//#define POS_INIT_A_X                      1
//#define POS_INIT_A_Y                      1
//#define POS_INIT_B_X         MAPA_ANCHO - 2
//#define POS_INIT_B_Y          MAPA_ALTO - 2

#define CANT_POSICIONES_VISTAS            9
#define MAX_SIN_CAMBIOS                 999


uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
                                        {30,  3, 50}, {30, 38, 50}, {15, 21, 100}, {45, 21, 100} ,
                                        {49,  3, 50}, {49, 40, 50}, {72, 34, 100}, {34, 21, 100}
                                    };
  
jugador_t jugadorA;
jugador_t jugadorB;

void pintar_botines() {
  print("b", 30, 3, 15);
  print("b", 49, 3, 15);
  print("b", 30, 38, 15);
  print("b", 49, 40, 15);
  print("b", 15, 21, 15);
  print("b", 72, 34, 15);
  print("b", 45, 21, 15);
  print("b", 34, 21, 15);
}

uint descubrio_botin(jugador_t *j, int x, int y)
{
  uint res = 0;
  int i = 0;
  while (i < BOTINES_CANTIDAD){
    if ((botines[i][0] == x) & (botines[i][1] == y) & (j->botines_vistos[i] == 0)){
      res = 1;
      j->botines_vistos[i] = 1;
    }
    i++;
  }
  return res;
}


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

int game_id_from_selector(int index){
  int aux = (index & 0xfff8) >> 3; //cereo atributos
  return aux - 15;  //le resto 15 porque los inicialice desde el indice 15
}

int game_id_from_index(int index){
  index -= 0x78;
  index /= 8;
  return index;  //le resto 15 porque los inicialice desde el indice 15
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
  pintar_botines();
}

void game_jugador_inicializar_mapa(jugador_t *jug)
{
  int fisicas_iniciales[9];
  if (jug == &jugadorA) {
    fisicas_iniciales[0] = PAG_INICIAL_A;
    fisicas_iniciales[1] = PAG_INICIAL_A + 0x1000; // Derecha de inicial
    fisicas_iniciales[2] = PAG_INICIAL_A - 0x1000; // Izquierda de inicial
    fisicas_iniciales[3] = PAG_INICIAL_A + MAPA_ANCHO * 0x1000; // Abajo de inicial
    fisicas_iniciales[4] = PAG_INICIAL_A + MAPA_ANCHO * 0x1000 + 0x1000; // Abajo-Derecha de inicial
    fisicas_iniciales[5] = PAG_INICIAL_A + MAPA_ANCHO * 0x1000 - 0x1000; // Abajo-Izquierda de inicial
    fisicas_iniciales[6] = PAG_INICIAL_A - MAPA_ANCHO * 0x1000; // Arriba de inicial
    fisicas_iniciales[7] = PAG_INICIAL_A - MAPA_ANCHO * 0x1000 + 0x1000; // Arriba-Derecha de inicial
    fisicas_iniciales[8] = PAG_INICIAL_A - MAPA_ANCHO * 0x1000 - 0x1000; // Arriba-Izquierda de inicial
  } else {
    fisicas_iniciales[0] = PAG_INICIAL_B;
    fisicas_iniciales[1] = PAG_INICIAL_B + 0x1000; // Derecha de inicial
    fisicas_iniciales[2] = PAG_INICIAL_B - 0x1000; // Izquierda de inicial
    fisicas_iniciales[3] = PAG_INICIAL_B + MAPA_ANCHO * 0x1000; // Abajo de inicial
    fisicas_iniciales[4] = PAG_INICIAL_B + MAPA_ANCHO * 0x1000 + 0x1000; // Abajo-Derecha de inicial
    fisicas_iniciales[5] = PAG_INICIAL_B + MAPA_ANCHO * 0x1000 - 0x1000; // Abajo-Izquierda de inicial
    fisicas_iniciales[6] = PAG_INICIAL_B - MAPA_ANCHO * 0x1000; // Arriba de inicial
    fisicas_iniciales[7] = PAG_INICIAL_B - MAPA_ANCHO * 0x1000 + 0x1000; // Arriba-Derecha de inicial
    fisicas_iniciales[8] = PAG_INICIAL_B - MAPA_ANCHO * 0x1000 - 0x1000; // Arriba-Izquierda de inicial
  }

  int i = 0;
  int j = 0;
  uint cr3;
  for (i=0 ; i<8 ; i++) {
    pirata_t p = jug->piratas[i];
    cr3 = p.cr3;
    for (j=0 ; j<9 ; j++) {
      uint fisica_a_mapear = fisicas_iniciales[j];
      uint logica = fisica_a_mapear + 0x300000;
      mmu_mapear_pagina(logica, &cr3, fisica_a_mapear);
    }
  }
}


void game_jugadores_inicializar(jugador_t *jA, jugador_t *jB)
{
  int index_gdt;
  int index;
  int i = 0;
  int clock_x = 6;
  for (i=0; i<8; i++){
    index_gdt = 0x78+(i*8);
    game_pirata_inicializar(&(jA->piratas[i]), jA, index_gdt, i, clock_x);
    clock_x += 2;
  }
  clock_x = 60;
  for (i=8; i<16; i++) {
    index_gdt = 0x78+(i*8);
    index = i-8;
    game_pirata_inicializar(&(jB->piratas[index]), jB, index_gdt, i, clock_x);
    clock_x += 2;
  }
  jA->puntaje = 0;
  jA->index = 0;
  jB->puntaje = 0;
  jB->index = 0;
  jA->pos_puntaje_x = 32;
  jA->pos_puntaje_y = 46;
  jB->pos_puntaje_x = 41;
  jB->pos_puntaje_y = 46;
  for (i=0 ; i<8 ; i++) {
    jA->botines_vistos[i] = 0;
    jB->botines_vistos[i] = 0;
  }

  //breakpoint();
  game_jugador_inicializar_mapa(jA);
  game_jugador_inicializar_mapa(jB);
  //breakpoint();
}

void game_pirata_inicializar(pirata_t *pirata, jugador_t *j, uint index, uint id, int clock_x)
{
  pirata->id = id;
  pirata->index_gdt = index;
  pirata->estaVivo = 0 ;
  pirata->jugador = j; 
  pirata->pos_clock_x = clock_x;
  pirata->pos_clock_y = 47;
  pirata->ya_corrio = 0;
  pirata->exploto = 0;
  uint* cr3 = dame_pagina_libre();
  uint* pt = dame_pagina_libre();
   
  inicializar_ident_mapping(cr3,pt);
  pirata->cr3 = (uint) cr3;
  uint pila0 = (uint) dame_pagina_libre();
  pirata->pila0 = pila0; 
  completar_tss(id, (uint)cr3, pila0);

}
void print_cr3(jugador_t *j) {
  int i = 0;
  for (i=0 ; i <8 ; i++) {
    breakpoint();
    print_hex(j->piratas[i].cr3, 15, 15 , 15 , 15);
  }

}
void game_pirata_tick(uchar sel_segmento)
{
  if (sel_segmento != 0x70) {
    int id_pirata = game_id_from_selector(sel_segmento);
    pirata_t* p = id_pirata2pirata(id_pirata);
    screen_actualizar_reloj_pirata(p);
  }
}

void game_tick()
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

// void game_pirata_relanzar(pirata_t* pPirata, jugador_t* pJ, uint tipo)
// {
//   reiniciar_tss_tarea(pPirata);
// }
//
void dame_tipo(int index_gdt)
{
  int id = game_id_from_selector(index_gdt);
  pirata_t *p = id_pirata2pirata(id); 
  breakpoint();
  print_hex((uint)index_gdt, 15, 17, 17, 15);
  print_hex((uint)p->tipo, 15, 15, 15, 15);
  print_hex((uint)id, 15, 13, 13, 15);
}

int pirata_target_Y(int index_gdt)
{
  int id = game_id_from_index(index_gdt);
  pirata_t *p = id_pirata2pirata(id); 
  
	return p->target_y;
}

int pirata_target_X(int index_gdt)
{
  int id = game_id_from_index(index_gdt);
  pirata_t *p = id_pirata2pirata(id); 
  
	return p->target_x;
}

pirata_t* game_jugador_erigir_pirata(jugador_t *j, uint tipo)
{
    // ~ completar ~

	return NULL;
}

void game_jugador_lanzar_pirata(jugador_t *j, uint tipo, int x, int y)
{
  pirata_t *p;
  int es_minero_pendiente = 0;
  if (tipo == 1){
    int i = 0;
    while ((i < 8) & (j->piratas[i].estaVivo == 1)){
      i++;  
    }
    //salgo de este while con la pos donde meter el pirata  
    if (i < 8){
      p = &(j->piratas[i]);
    //invariante: id, index_gdt, jugador de p son correctos
    } else {
    //lo que hago si no habia lugar
      es_minero_pendiente = 1;
      i = 0;
      while (j->minerosPendientes[i].estaVivo == 1){ //termina porque como mucho son 8
        i++;  
      }
      p = &(j->minerosPendientes[i]);
    //invariante: id, index_gdt, jugador de p son correctos
    }
    if (j == &jugadorA) {
      p->pos_x = POS_INIT_A_X;
      p->pos_y = POS_INIT_A_Y;
    }
    if (j == &jugadorB) {
      p->pos_x = POS_INIT_B_X;
      p->pos_y = POS_INIT_B_Y;
    }
    p->target_x = x;
    p->target_y = y;
  }
  if (tipo == 0){
    int i = 0;
    while ((i<8) & (j->piratas[i].estaVivo == 1)){  
      i++;
    }
    if(i < 8)
      p = &(j->piratas[i]);
      //invariante: id, index_gdt, jugador de p son correctos
    p->pos_x = x;
    p->pos_y = y;
    p->target_x = 0;
    p->target_y = 0;
    }

  p->estaVivo = 1;

  p->tipo = tipo;
  p->ya_corrio = 0;

  if (es_minero_pendiente == 0) {
    mmu_inicializar_dir_pirata(p, x, y);

    if (j == &jugadorA) {
      screen_pintar(32,C_BG_GREEN, p->pos_y, p->pos_x-1);
      screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x-1);
      screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x-1);
      screen_pintar(32,C_BG_GREEN, p->pos_y,p->pos_x+1);
      screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x+1);
      screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x+1);
      screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x-1);
      screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x);
      screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x+1);
      screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x-1);
      screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x);
      screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x+1);
    } else {
      screen_pintar(32,C_MAKE_BG(9), p->pos_y, p->pos_x-1);
      screen_pintar(32,C_MAKE_BG(9), p->pos_y-1,p->pos_x-1);
      screen_pintar(32,C_MAKE_BG(9), p->pos_y+1,p->pos_x-1);
      screen_pintar(32,C_MAKE_BG(9), p->pos_y,p->pos_x+1);
      screen_pintar(32,C_MAKE_BG(9), p->pos_y-1,p->pos_x+1);
      screen_pintar(32,C_MAKE_BG(9), p->pos_y+1,p->pos_x+1);
      screen_pintar(32,C_MAKE_BG(9), p->pos_y+1,p->pos_x-1);
      screen_pintar(32,C_MAKE_BG(9), p->pos_y+1,p->pos_x);
      screen_pintar(32,C_MAKE_BG(9), p->pos_y+1,p->pos_x+1);
      screen_pintar(32,C_MAKE_BG(9), p->pos_y-1,p->pos_x-1);
      screen_pintar(32,C_MAKE_BG(9), p->pos_y-1,p->pos_x);
      screen_pintar(32,C_MAKE_BG(9), p->pos_y-1,p->pos_x+1);
    }
  }
}

void game_pirata_habilitar_posicion(jugador_t *j, pirata_t *pirata, int x, int y)
{
}

void mapearle_pila_tarea(){
  uint kernel_cr3 = 0x27000;
  mmu_mapear_pagina(0x400000, &kernel_cr3, 0x400000);
}

void desmapearle_pila_tarea(){
  uint kernel_cr3 = 0x27000;
  mmu_desmapear_pagina(0x400000, &kernel_cr3);
}


void game_explorar_posicion(jugador_t *jugador, int c, int f)
{
//uint game_dir2xy(direccion dir, int *x, int *y)
//le paso las direc que me tire esta funcion
}

void print_pirata_mapa(pirata_t *p, direccion dir, int *vistasX, int *vistasY) {

  jugador_t *j = p->jugador;
  int x = 0; 
  int y = 0;

  if (j == &jugadorA) {
    if (p->tipo == 0) { //explorador
      print("E", p->pos_x, p->pos_y, 2);
      game_dir2xy(dir,&x,&y); //para donde te tenes que mover
      p->pos_x = p->pos_x + x;
      p->pos_y = p->pos_y + y;
      screen_pintar(32,C_FG_RED, p->pos_y ,p->pos_x);
      print("E",p->pos_x, p->pos_y,4);
    } else {
      print("M", p->pos_x, p->pos_y, 2);
      game_dir2xy(dir,&x,&y); //para donde te tenes que mover
      p->pos_x = p->pos_x + x;
      p->pos_y = p->pos_y + y;
      print("M",p->pos_x, p->pos_y,4);
    }
    if (dir == IZQ){
      screen_pintar(32,C_BG_GREEN, p->pos_y, p->pos_x-1);
      screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x-1);
      screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x-1);
      vistasX[0] = p->pos_x-1;
      vistasX[1] = p->pos_x-1;
      vistasX[2] = p->pos_x-1;
      vistasY[0] = p->pos_y;
      vistasY[1] = p->pos_y-1;
      vistasY[2] = p->pos_y+1;
    }

    if (dir == DER){
      screen_pintar(32,C_BG_GREEN, p->pos_y,p->pos_x+1);
      screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x+1);
      screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x+1);
      vistasX[0] = p->pos_x+1;
      vistasX[1] = p->pos_x+1;
      vistasX[2] = p->pos_x+1;
      vistasY[0] = p->pos_y;
      vistasY[1] = p->pos_y-1;
      vistasY[2] = p->pos_y+1;
    }
    if (dir == ABA){
      screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x-1);
      screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x);
      screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x+1);
      vistasX[0] = p->pos_x-1;
      vistasX[1] = p->pos_x;
      vistasX[2] = p->pos_x+1;
      vistasY[0] = p->pos_y+1;
      vistasY[1] = p->pos_y+1;
      vistasY[2] = p->pos_y+1;
    }
    if (dir == ARR){
      screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x-1);
      screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x);
      screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x+1);
      vistasX[0] = p->pos_x-1;
      vistasX[1] = p->pos_x;
      vistasX[2] = p->pos_x+1;
      vistasY[0] = p->pos_y-1;
      vistasY[1] = p->pos_y-1;
      vistasY[2] = p->pos_y-1;
    }
  } else {
    if (p->tipo == 0) { //explorador
      screen_pintar(32,C_FG_BLUE, p->pos_y,p->pos_x);
      print("E", p->pos_x, p->pos_y, 9);
      game_dir2xy(dir,&x,&y); //para donde te tenes que mover
      p->pos_x = p->pos_x + x;
      p->pos_y = p->pos_y + y;
      screen_pintar(32,C_FG_BLUE, p->pos_y,p->pos_x);
      print("E",p->pos_x, p->pos_y,15);
    } else {
      screen_pintar(32,C_FG_BLUE, p->pos_y,p->pos_x);
      print("M", p->pos_x, p->pos_y, C_FG_BLUE);
      game_dir2xy(dir,&x,&y); //para donde te tenes que mover
      p->pos_x = p->pos_x + x;
      p->pos_y = p->pos_y + y;
      screen_pintar(32,C_FG_BLUE, p->pos_y,p->pos_x);
      print("M",p->pos_x, p->pos_y,15);
    }

    if (dir == IZQ){
      screen_pintar(32,C_BG_BLUE, p->pos_y, p->pos_x-1);
      screen_pintar(32,C_BG_BLUE, p->pos_y-1,p->pos_x-1);
      //screen_pintar(32,C_BG_BLUE, p->pos_y+1,p->pos_x-1);
      vistasX[0] = p->pos_x-1;
      vistasX[1] = p->pos_x-1;
      vistasX[2] = p->pos_x-1;
      vistasY[0] = p->pos_y;
      vistasY[1] = p->pos_y-1;
      vistasY[2] = p->pos_y+1;
    }
    if (dir == DER){
      screen_pintar(32,C_BG_BLUE, p->pos_y,p->pos_x+1);
      screen_pintar(32,C_BG_BLUE, p->pos_y-1,p->pos_x+1);
      //screen_pintar(32,C_BG_BLUE, p->pos_y+1,p->pos_x+1);
      vistasX[0] = p->pos_x+1;
      vistasX[1] = p->pos_x+1;
      vistasX[2] = p->pos_x+1;
      vistasY[0] = p->pos_y;
      vistasY[1] = p->pos_y-1;
      vistasY[2] = p->pos_y+1;
    }
    if (dir == ABA){
      screen_pintar(32,C_BG_BLUE, p->pos_y+1,p->pos_x-1);
      screen_pintar(32,C_BG_BLUE, p->pos_y+1,p->pos_x);
      //screen_pintar(32,C_BG_BLUE, p->pos_y+1,p->pos_x+1);
      vistasX[0] = p->pos_x-1;
      vistasX[1] = p->pos_x;
      vistasX[2] = p->pos_x+1;
      vistasY[0] = p->pos_y+1;
      vistasY[1] = p->pos_y+1;
      vistasY[2] = p->pos_y+1;
    }
    if (dir == ARR){
      screen_pintar(32,C_BG_BLUE, p->pos_y-1,p->pos_x-1);
      screen_pintar(32,C_BG_BLUE, p->pos_y-1,p->pos_x);
      //screen_pintar(32,C_FG_BLUE, p->pos_y-1,p->pos_x+1);
      vistasX[0] = p->pos_x-1;
      vistasX[1] = p->pos_x;
      vistasX[2] = p->pos_x+1;
      vistasY[0] = p->pos_y-1;
      vistasY[1] = p->pos_y-1;
      vistasY[2] = p->pos_y-1;
    }
  }
}

uint game_syscall_pirata_mover(uint id, direccion dir)
{
  pirata_t *p = id_pirata2pirata(id); 
  jugador_t *j = p->jugador;
  uint cr3Actual = p->cr3;
  uint fisica_a_moverse;
  uint actual;
  int vistasX[3];
  int vistasY[3];

  print_pirata_mapa(p, dir, vistasX, vistasY);

  if (game_posicion_valida(p->pos_x,p->pos_y)){ //pregunto si ese movimiento me deja en una pos valida del mapa
    //FALTA SI ES MINERO
    //if (p->tipo == 0){
    actual = p->fisica_actual; 
    fisica_a_moverse = dame_siguiente_pos_fisica(actual, dir);
    p->fisica_actual = fisica_a_moverse;
      
      //mapeo las posiciones exploradas
    uint logica = fisica_a_moverse + 0x300000;
    //if (p->tipo == 0) {
    //  mmu_mapear_pagina(0x400000, &cr3Actual, fisica_a_moverse);
    //} else {
    //  breakpoint();
      mmu_mapear_pagina(0x400000, &cr3Actual, fisica_a_moverse);
    //}
    
    if (p->tipo == 0) {
      uint logica_aux_1;
      uint logica_aux_2;
      uint logica_aux_3;
      uint fisica_aux_1;
      uint fisica_aux_2;
      uint fisica_aux_3;

      if (dir == IZQ){
        //abajo a la izquierda
        logica_aux_1 = logica + MAPA_ANCHO * 0x1000 - 0x1000;
        fisica_aux_1 = fisica_a_moverse + MAPA_ANCHO * 0x1000 - 0x1000;

        //arriba a la izquierda
        logica_aux_2 = logica - MAPA_ANCHO * 0x1000 - 0x1000;
        fisica_aux_2 = fisica_a_moverse - MAPA_ANCHO * 0x1000 - 0x1000;

        //a la izquierda
        logica_aux_3 = logica - 0x1000;
        fisica_aux_3 = fisica_a_moverse - 0x1000;
      }
      if (dir == DER){
        //abajo a la derecha
        logica_aux_1 = logica + MAPA_ANCHO * 0x1000 + 0x1000;
        fisica_aux_1 = fisica_a_moverse + MAPA_ANCHO * 0x1000 + 0x1000;

        //arriba a la derecha
        logica_aux_2 = logica - MAPA_ANCHO * 0x1000 + 0x1000;
        fisica_aux_2 = fisica_a_moverse - MAPA_ANCHO * 0x1000 + 0x1000;

        //a la derecha
        logica_aux_3 = logica + 0x1000;
        fisica_aux_3 = fisica_a_moverse + 0x1000;
      }
      if (dir == ABA){
        //abajo
        logica_aux_1 = logica + MAPA_ANCHO * 0x1000;
        fisica_aux_1 = fisica_a_moverse + MAPA_ANCHO * 0x1000;
      
        //abajo a la izquierda
        logica_aux_2 = logica + MAPA_ANCHO * 0x1000 - 0x1000;
        fisica_aux_2 = fisica_a_moverse+ MAPA_ANCHO * 0x1000 - 0x1000;
        
        //abajo a la derecha
        logica_aux_3 = logica + MAPA_ANCHO * 0x1000 + 0x1000;
        fisica_aux_3 = fisica_a_moverse + MAPA_ANCHO * 0x1000 + 0x1000;
      }
      if (dir == ARR){
        //arriba
        logica_aux_1 = logica - MAPA_ANCHO * 0x1000;
        fisica_aux_1 = fisica_a_moverse - MAPA_ANCHO * 0x1000;
      
        //arriba a la derecha
        logica_aux_2 = logica - MAPA_ANCHO * 0x1000 + 0x1000;
        fisica_aux_2 = fisica_a_moverse - MAPA_ANCHO * 0x1000 + 0x1000;
      
        //arriba a la izquierda
        logica_aux_3 = logica - MAPA_ANCHO * 0x1000 - 0x1000;
        fisica_aux_3 = fisica_a_moverse - MAPA_ANCHO * 0x1000 - 0x1000;
      }
      int i = 0;
      for (i=0; i<8; i++) {
        uint cr3 = j->piratas[i].cr3;
        mmu_mapear_pagina(logica_aux_1, &cr3, fisica_aux_1);
        mmu_mapear_pagina(logica_aux_2, &cr3, fisica_aux_2);
        mmu_mapear_pagina(logica_aux_3, &cr3, fisica_aux_3);
      }
      i = 0;
      while (i < 3){
        if (descubrio_botin(j, vistasX[i], vistasY[i])){
          game_jugador_lanzar_pirata(j, 1, vistasX[i], vistasY[i]); 
        }
        i++;
      }
    }
    uint kernel_cr3 = 0x27000;
    mmu_mapear_pagina(0x3FE000, &kernel_cr3, fisica_a_moverse); //primero mapeo y dsp copio codigo no????
    mmu_mapear_pagina(0x3FF000, &kernel_cr3, actual); //primero mapeo y dsp copio codigo no????
    copiar_codigo_tarea((int*)0x3FE000, (int*) 0x3FF000);
    mmu_mapear_pagina(0x3FE000, &kernel_cr3, 0x3FE000); //primero mapeo y dsp copio codigo no????
    mmu_mapear_pagina(0x3FF000, &kernel_cr3, 0x3FF000); //primero mapeo y dsp copio codigo no????
  }

    return 0;
}
    // ESTE CODIGO FUNCIONA
    //uint kernel_cr3 = 0x27000;
    //mmu_mapear_pagina(0x400000, &kernel_cr3, fisica_a_moverse); //primero mapeo y dsp copio codigo no????
    //mmu_mapear_pagina(0x3FF000, &kernel_cr3, actual); //primero mapeo y dsp copio codigo no????
    //copiar_codigo_tarea((int*)0x400000, (int*) 0x3FF000);
    //mmu_mapear_pagina(0x3FF000, &kernel_cr3, 0x3FF000); //primero mapeo y dsp copio codigo no????

void ver_si_exploto(uchar sel_segmento) {
  int id_pirata = game_id_from_selector(sel_segmento);
  pirata_t *p = id_pirata2pirata(id_pirata); 
  if (p->exploto == 1){
    //breakpoint();
    reiniciar_tss(p->id, p->pila0);
  }
  p->exploto = 0;
}

uint game_syscall_cavar(uint id)
{
  pirata_t *p = id_pirata2pirata(id); 
  jugador_t *j = p->jugador;
  int i = 0;
  int encontre = 0;
  while ((i < BOTINES_CANTIDAD) & (encontre == 0)){
    if ((botines[i][0] == p->pos_x+2) & (botines[i][1] == p->pos_y-2)){
      j->puntaje++;
      p->monedas_recolectadas++;
      botines[i][2]--;
      print_puntaje(j);
      encontre = 1;
    }
    i++;
  }
  if (p->monedas_recolectadas == botines[i][2]) {
    p->estaVivo = 0;
    p->ya_corrio = 0;
    p->exploto = 1;
    p->monedas_recolectadas = 0;
    limpiar_clock(p);
  }
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

void game_pirata_exploto(uchar sel_segmento)
{
    int id_pirata = game_id_from_selector(sel_segmento);
    pirata_t* p = id_pirata2pirata(id_pirata);
    p->estaVivo = 0;
    p->ya_corrio = 0;
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
  if (tecla == KB_shiftA) {
    print_Lshift();
    sched_generar_pirata_jugadorA();
  }
  if (tecla == KB_shiftB) {
    print_Rshift();
    sched_generar_pirata_jugadorB();
  }
  if (tecla == 0x15) {
    print_Y();
    sched_activar_debbuger(); 
  }
  if (tecla == 0xAA || tecla == 0xB6 || tecla == 0x95)
    clear_screen_portion();
}
