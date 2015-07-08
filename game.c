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

#define BOTINES_CANTIDAD 8


uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
                                        {30,  3, 50}, {30, 38, 50}, {15, 21, 100}, {45, 21, 100} ,
                                        {49,  3, 50}, {49, 38, 50}, {64, 21, 100}, {34, 21, 100}
                                    };
  
jugador_t jugadorA;
jugador_t jugadorB;

uint descubrio_botin(int x, int y)
{
  uint res = 0;
  int i = 0;
  while (i < BOTINES_CANTIDAD){
    if ((botines[i][0] == x) & (botines[i][1] == y)){
      res = 1;
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
}

void game_jugador_inicializar_mapa(jugador_t *jug, uint cr3)
{
  //if (jug == &(jugadorA)) {
    uint fisica_a_moverse = 0x500000 + MAPA_ANCHO * 0x1000 + 0x1000;
    uint auxiliar = 0x800000 + MAPA_ANCHO * 0x1000 + 0x1000;
  //} else {
  //  uint fisica_a_moverse = 0x500000 + MAPA_ANCHO * 0x1000 + 0x1000;
  //  uint auxiliar = 0x800000 + MAPA_ANCHO * 0x1000 + 0x1000;
  //}

  mmu_mapear_pagina(auxiliar, &cr3, fisica_a_moverse);
      
  uint aux2;
  uint auxf;

  aux2 = auxiliar - 0x1000;
  auxf = fisica_a_moverse - 0x1000;
  mmu_mapear_pagina((uint*)aux2, &cr3,(uint*)auxf); //izq
  
  aux2 = auxiliar + 0x1000;
  auxf = fisica_a_moverse + 0x1000;
  mmu_mapear_pagina(aux2, &cr3, auxf);//der
  
  aux2 =auxiliar + MAPA_ANCHO * 0x1000;
  auxf =fisica_a_moverse + MAPA_ANCHO * 0x1000;
  mmu_mapear_pagina(aux2, &cr3,auxf);
  
  aux2 =auxiliar + MAPA_ANCHO * 0x1000 - 0x1000;
  auxf =fisica_a_moverse+ MAPA_ANCHO * 0x1000 - 0x1000;
  mmu_mapear_pagina(aux2, &cr3, auxf);
  
  aux2 =auxiliar + MAPA_ANCHO * 0x1000 + 0x1000;
  auxf =fisica_a_moverse + MAPA_ANCHO * 0x1000 + 0x1000;
  mmu_mapear_pagina(aux2, &cr3, auxf);
  
  aux2 =auxiliar - MAPA_ANCHO * 0x1000;
  auxf =fisica_a_moverse - MAPA_ANCHO * 0x1000;
  mmu_mapear_pagina(aux2, &cr3, auxf);
  
  aux2 =auxiliar - MAPA_ANCHO * 0x1000 + 0x1000;
  auxf =fisica_a_moverse - MAPA_ANCHO * 0x1000 + 0x1000;
  mmu_mapear_pagina(aux2, &cr3, auxf);
  
  aux2 =auxiliar - MAPA_ANCHO * 0x1000 - 0x1000;
  auxf =fisica_a_moverse - MAPA_ANCHO * 0x1000 - 0x1000;
  mmu_mapear_pagina(aux2, &cr3, auxf);

}


void game_jugadores_inicializar(jugador_t *jA, jugador_t *jB)
{
  int index_gdt;
  int i = 0;
  for (i=0; i<8; i++){
    index_gdt = 0x78+(i*8);
    game_pirata_inicializar(&(jA->piratas[i]), jA, index_gdt, i);
  }
  for (i=8; i<16; i++) {
    index_gdt = 0x78+(i*8);
    game_pirata_inicializar(&(jB->piratas[i]), jB, index_gdt, i);
  }
  jA->puntaje = 0;
  jA->index = 0;
  jB->puntaje = 0;
  jB->index = 0;
}

void game_pirata_inicializar(pirata_t *pirata, jugador_t *j, uint index, uint id)
{
  pirata->id = id;
  pirata->index_gdt = index;
  pirata->estaVivo = 0 ;
  //pirata->pos_x = POS_INIT_A_X; //el tablero va de 0 a 79 y de 0 a 54
  //pirata->pos_y = POS_INIT_A_Y;
  pirata->jugador = j; 
  //pirata->tipo = explorador;
  print_hex(pirata->index_gdt, 10, 9, 9, 3);
  //breakpoint();

  uint cr3 = (uint) mmu_inicializar_dir_pirata();
  pirata->cr3 = cr3;

  uint pila0 = (uint) dame_pagina_libre();
  completar_tss(id, cr3, pila0);
  game_jugador_inicializar_mapa(j, cr3);
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

void prueba_lanzar_pirata(pirata_t *p){
  jugador_t *j = p->jugador;

  uint cr3 = p->cr3;
  print_hex(cr3, 10, 24, 24, 3);

  //uint fisica_actual = mmu_pos_fisica(&cr3, 0x400000);
  uint fisica_actual = PAG_INICIAL;

  if (fisica_actual == PAG_INICIAL) {
    if (j == &(jugadorA)) {
      if (p->tipo == 0) {
        copiar_codigo_tarea((int*)0x400000, (int*)0x10000);
      } else {
        copiar_codigo_tarea((int*)0x400000, (int*)0x11000);
      }
    } else {
      if (p->tipo == 0) {
        copiar_codigo_tarea((int*)0x400000, (int*)0x12000);
      } else {
        copiar_codigo_tarea((int*)0x400000, (int*)0x13000);
      }
    }
  }
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


//POTENTE COMENTARIO SOBRE QUE CARAJO X E Y
void game_jugador_lanzar_pirata(jugador_t *j, uint tipo, int x, int y)
{
  //breakpoint();
    pirata_t *p;
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
      i = 0;
      while (j->minerosPendientes[i].estaVivo == 1){ //termina porque como mucho son 8
        i++;  
      }
      p = &(j->minerosPendientes[i]);
    //invariante: id, index_gdt, jugador de p son correctos
    }
    p->estaVivo = 1;
    p->tipo = tipo;
    p->pos_x = x;
    p->pos_y = y;
  }
  if (tipo == 0){
    int i = 0;
    while ((i<8) & (j->piratas[i].estaVivo == 1)){  
      i++;
    }
    if(i < 8){
      p = &(j->piratas[i]);
      //invariante: id, index_gdt, jugador de p son correctos
      p->estaVivo = 1;
      p->tipo = tipo;
      p->pos_x = x;
      p->pos_y = y;
      }
    }

  
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
  pirata_t *p = id_pirata2pirata(id); 

  uint cr3 = p->cr3;
  // LEVANTA BIEN EL CR3 Y SU ID TODO PIO WACHO
  int x = 0; 
  int y = 0;
  //uint fisica1;
  //uint fisica2;

  print("E", p->pos_x, p->pos_y, 2);
  game_dir2xy(dir,&x,&y); //para donde te tenes que mover
  p->pos_x = p->pos_x + x;
  p->pos_y = p->pos_y + y;
  print("E",p->pos_x, p->pos_y,2);
  if (game_posicion_valida(p->pos_x,p->pos_y)){ //pregunto si ese movimiento me deja en una pos valida del mapa
    //FALTA SI ES MINERO
    if (p->tipo == 0){
      uint actual = mmu_pos_fisica(&cr3,0x400000);
      uint fisica_a_moverse = dame_siguiente_pos_fisica(actual, dir);
        
        //mapeo las posiciones exploradas
      uint auxiliar = fisica_a_moverse + 0x300000;
//        mmu_mapear_pagina(auxiliar, &cr3, fisica_a_moverse);
        //breakpoint();
      
      uint aux2;
      uint auxf;
      jugador_t *j = p->jugador;
      int i = 0;
      while (i < 8){
        
        pirata_t pAux = j->piratas[i];
        uint cr3 = pAux.cr3;

        if (dir == IZQ){

          //abajo a la izquierda
          aux2 =auxiliar + MAPA_ANCHO * 0x1000 - 0x1000;
          auxf =fisica_a_moverse + MAPA_ANCHO * 0x1000 - 0x1000;
          mmu_mapear_pagina(aux2, &cr3, auxf);

          //arriba a la izquierda
          aux2 =auxiliar - MAPA_ANCHO * 0x1000 - 0x1000;
          auxf =fisica_a_moverse - MAPA_ANCHO * 0x1000 - 0x1000;
          mmu_mapear_pagina(aux2, &cr3, auxf);

          //a la izquierda
          aux2 = auxiliar - 0x1000;
          auxf = fisica_a_moverse - 0x1000;
          mmu_mapear_pagina(aux2, &cr3, auxf);//der

          screen_pintar(32,C_BG_GREEN, p->pos_y, p->pos_x-1);
          screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x-1);
          screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x-1);
        }
        if (dir == DER){

          //abajo a la derecha
          aux2 =auxiliar + MAPA_ANCHO * 0x1000 + 0x1000;
          auxf =fisica_a_moverse + MAPA_ANCHO * 0x1000 + 0x1000;
          mmu_mapear_pagina(aux2, &cr3, auxf);

          //arriba a la derecha
          aux2 =auxiliar - MAPA_ANCHO * 0x1000 + 0x1000;
          auxf =fisica_a_moverse - MAPA_ANCHO * 0x1000 + 0x1000;
          mmu_mapear_pagina(aux2, &cr3, auxf);

          //a la derecha
          aux2 = auxiliar + 0x1000;
          auxf = fisica_a_moverse + 0x1000;
          mmu_mapear_pagina(aux2, &cr3, auxf);//der

          screen_pintar(32,C_BG_GREEN, p->pos_y,p->pos_x+1);
          screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x+1);
          screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x+1);
        }
        if (dir == ABA){

          //abajo
          aux2 =auxiliar + MAPA_ANCHO * 0x1000;
          auxf =fisica_a_moverse + MAPA_ANCHO * 0x1000;
          mmu_mapear_pagina(aux2, &cr3,auxf);
        
          //abajo a la izquierda
          aux2 =auxiliar + MAPA_ANCHO * 0x1000 - 0x1000;
          auxf =fisica_a_moverse+ MAPA_ANCHO * 0x1000 - 0x1000;
          mmu_mapear_pagina(aux2, &cr3, auxf);
          
          //abajo a la derecha
          aux2 =auxiliar + MAPA_ANCHO * 0x1000 + 0x1000;
          auxf =fisica_a_moverse + MAPA_ANCHO * 0x1000 + 0x1000;
          mmu_mapear_pagina(aux2, &cr3, auxf);

          screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x-1);
          screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x);
          screen_pintar(32,C_BG_GREEN, p->pos_y+1,p->pos_x+1);
        }
        if (dir == ARR){
          
          //arriba
          aux2 =auxiliar - MAPA_ANCHO * 0x1000;
          auxf =fisica_a_moverse - MAPA_ANCHO * 0x1000;
          mmu_mapear_pagina(aux2, &cr3, auxf);
        
          //arriba a la derecha
          aux2 =auxiliar - MAPA_ANCHO * 0x1000 + 0x1000;
          auxf =fisica_a_moverse - MAPA_ANCHO * 0x1000 + 0x1000;
          mmu_mapear_pagina(aux2, &cr3, auxf);
        
          //arriba a la izquierda
          aux2 =auxiliar - MAPA_ANCHO * 0x1000 - 0x1000;
          auxf =fisica_a_moverse - MAPA_ANCHO * 0x1000 - 0x1000;
          mmu_mapear_pagina(aux2, &cr3, auxf);

          screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x-1);
          screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x);
          screen_pintar(32,C_BG_GREEN, p->pos_y-1,p->pos_x+1);
        }
        i++;
      }
      // SI DESCUBRI BOTIN TENGO QUE CREAR TAREA MINERO
      //if (descubrio_botin(p->pos_x, p->pos_y)){
      //  jugador_t *j = p->jugador;  
      //  game_jugador_lanzar_pirata(j, minero, p->pos_x, p->pos_y); 
      //}

      // SI DESCUBRI BOTIN TENGO QUE CREAR TAREA MINERO
      //int vistasX;
      //int vistasY;
      //game_calcular_posiciones_vistas(&vistasX, &vistasY, p->pos_x, p->pos_y);
      //i = 0;
      //while (i < 9){
      //  if (descubrio_botin((&vistasX)[i], (&vistasY)[i])){
      //    jugador_t *j = p->jugador;  
      //    game_jugador_lanzar_pirata(j, minero, p->pos_x, p->pos_y); 
      //  }
      //  i++;
      //}
    //mapeo momentaneamente para copiar el codigo de la tarea
    //uint* virtual = dame_pagina_unica();
      uint* virtual =(uint*) 0x3ff000;
      mmu_mapear_pagina(virtual, &cr3, actual); //primero mapeo y dsp copio codigo no????
      mmu_mapear_pagina(0x400000, &cr3, fisica_a_moverse); //primero mapeo y dsp copio codigo no????
      copiar_codigo_tarea((int*)0x400000, virtual);

      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      //!!!!!!!!!!!!!!! SI PONGO EL CODIGO DE ARRIBA ANTES DE LOS IFS NO FUNCA, WTF??????
      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


      //print_hex(actual,15, 2, 2,15);
      //breakpoint();
      //mmu_mapear_pagina((uint*)0x800000, &cr3,(uint*) 0x500000); //POR QUE NO LO HACESSSSSSS
      //print_hex(fisica_a_moverse,15, 4, 4,15);
      //print_hex(fisica1,15, 6, 6,15);
      //print_hex(fisica2,15, 8, 8,15);
      //TAL VEZ LO DE ARRIBA DEBERIA EJECUTARSE ANTES
      
      
      /*SI MAPEO 9 (JUST IN CASE) NO LO BORREN POR AHORA! 
      aux2 = auxiliar - 0x1000;
      auxf = fisica_a_moverse - 0x1000;
      print_hex(aux2,15, 8, 8,15);
      print_hex(auxf,15, 16, 16,15);
      mmu_mapear_pagina((uint*)aux2, &cr3,(uint*)auxf); //izq
      breakpoint();
      
      aux2 = auxiliar + 0x1000;
      auxf = fisica_a_moverse + 0x1000;
      breakpoint();
      print_hex(aux2,15, 8, 8,15);
      print_hex(auxf,15, 16, 16,15);
      //mmu_mapear_pagina(aux2, &cr3, auxf);//der
      mmu_mapear_pagina((uint*)aux2, &cr3,(uint*)auxf); //izq
      breakpoint();
  
      aux2 =auxiliar + MAPA_ANCHO * 0x1000;
      auxf =fisica_a_moverse + MAPA_ANCHO * 0x1000;
      print_hex(aux2,15, 8, 8,15);
      print_hex(auxf,15, 16, 16,15);
      mmu_mapear_pagina(aux2, &cr3,auxf);
      breakpoint();
      
      aux2 =auxiliar + MAPA_ANCHO * 0x1000 - 0x1000;
      auxf =fisica_a_moverse+ MAPA_ANCHO * 0x1000 - 0x1000;
      print_hex(aux2,15, 8, 8,15);
      print_hex(auxf,15, 16, 16,15);
      mmu_mapear_pagina(aux2, &cr3, auxf);
      breakpoint();
      
      aux2 =auxiliar + MAPA_ANCHO * 0x1000 + 0x1000;
      auxf =fisica_a_moverse + MAPA_ANCHO * 0x1000 + 0x1000;
      print_hex(aux2,15, 8, 8,15);
      print_hex(auxf,15, 16, 16,15);
      mmu_mapear_pagina(aux2, &cr3, auxf);
      breakpoint();
      
      aux2 =auxiliar - MAPA_ANCHO * 0x1000;
      auxf =fisica_a_moverse - MAPA_ANCHO * 0x1000;
      print_hex(aux2,15, 8, 8,15);
      print_hex(auxf,15, 16, 16,15);
      mmu_mapear_pagina(aux2, &cr3, auxf);
      breakpoint();
      
      aux2 =auxiliar - MAPA_ANCHO * 0x1000 + 0x1000;
      auxf =fisica_a_moverse - MAPA_ANCHO * 0x1000 + 0x1000;
      print_hex(aux2,15, 8, 8,15);
      print_hex(auxf,15, 16, 16,15);
      mmu_mapear_pagina(aux2, &cr3, auxf);
      breakpoint();
      
      aux2 =auxiliar - MAPA_ANCHO * 0x1000 - 0x1000;
      auxf =fisica_a_moverse - MAPA_ANCHO * 0x1000 - 0x1000;
      print_hex(aux2,15, 8, 8,15);
      print_hex(auxf,15, 16, 16,15);
      mmu_mapear_pagina(aux2, &cr3, auxf);
      
      breakpoint();
      */
    }
  } 
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
  if (tecla == KB_shiftA) {
    print_Lshift();
    sched_generar_pirata_jugadorA();
    //prueba_lanzar_pirata();
  }
  if (tecla == KB_shiftB) {
    print_Rshift();
    sched_generar_pirata_jugadorB();
  }
  if (tecla == 0x15) {
    print_Y();
    //Hacer debug
  }
  if (tecla == 0xAA || tecla == 0xB6 || tecla == 0x95)
    clear_screen_portion();
}
