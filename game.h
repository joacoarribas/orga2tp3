/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

*/

#ifndef __GAME_H__
#define __GAME_H__

#include "defines.h"

typedef enum direccion_e { ARR = 0x4, ABA = 0x7, DER = 0xA, IZQ = 0xD} direccion;
typedef enum tipoPirata_e {explorador = 0x0, minero = 0x1} tipoPirata;

#define MAX_CANT_PIRATAS_VIVOS           8
#define MAX_CANT_MINEROS_PENDIENTES      8 //cantidad de botines que hay en el mapa acotada por la variable BOTINES_CANTIDAD

#define JUGADOR_A                         0
#define JUGADOR_B                         1

#define MAPA_ANCHO                       80
#define MAPA_ALTO                        42

#define POS_INIT_A_X                      1
#define POS_INIT_A_Y                      2
#define POS_INIT_B_X         MAPA_ANCHO - 2
#define POS_INIT_B_Y          MAPA_ALTO - 1


struct jugador_t;

typedef struct pirata_t
{
    uint id;
    uint index_gdt;
    uint cr3;
    uint pila0;
    uchar exploto;
    uchar estaVivo; // 0 esta muerto, 1 esta vivo
    uint tipo; // 0 explorador, 1 minero
    uint fisica_actual;
    uchar ya_corrio;
    int pos_x;
    int pos_y;
    struct jugador_t *jugador;
    int pos_clock_x;
    int pos_clock_y;
    int target_x; // Estas son en caso de sea minero 
    int target_y; // Estas son en caso de sea minero 

    // id unica, posicion, tipo, reloj
    // booleano que diga si esta vivo o muerto
} pirata_t;


typedef struct jugador_t

{
    uint puntaje; //cantidad de monedas 
    uint index;
    pirata_t piratas[MAX_CANT_PIRATAS_VIVOS];
    pirata_t minerosPendientes[MAX_CANT_MINEROS_PENDIENTES];
    uint fisicas_vistas[3360];  //guarda direcciones fisicas vistas 
    uint indice_fisicas_vistas_libre;
    uint pos_puntaje_x;
    uint pos_puntaje_y;
    //estas las puedo actualizar directamente usando game_calcular_posiciones_vistas
  // coordenadas puerto, posiciones exploradas, mineros pendientes, etc
} jugador_t;


extern jugador_t jugadorA;
extern jugador_t jugadorB;

// ~ auxiliares dadas ~
uint game_xy2lineal();
pirata_t* id_pirata2pirata(uint id);

// ~ auxiliares sugeridas o requeridas (segun disponga enunciado) ~
void game_pirata_inicializar(pirata_t *pirata, jugador_t *jugador, uint index, uint id, int clock_x);
void game_pirata_erigir(pirata_t *pirata, jugador_t *j, uint tipo);
void game_pirata_habilitar_posicion(jugador_t *j, pirata_t *pirata, int x, int y);
void game_pirata_exploto(uchar id);

void game_jugadores_inicializar(jugador_t *jA, jugador_t *jB);
void game_jugador_lanzar_pirata(jugador_t *j, uint tipo, int x, int y);
pirata_t* game_jugador_erigir_pirata(jugador_t *j, uint tipo);
void game_jugador_anotar_punto(jugador_t *j);
void game_explorar_posicion(jugador_t *jugador, int x, int y);

uint game_valor_tesoro(uint x, uint y);
void game_calcular_posiciones_vistas(int *vistas_x, int *vistas_y, int x, int y);
pirata_t* game_pirata_en_posicion(uint x, uint y);

uint game_syscall_pirata_posicion(uint id, int idx);
uint game_syscall_pirata_mover(uint id, direccion key);
uint game_syscall_manejar(uint syscall, uint param1);
void game_pirata_tick(uchar id_pirata);
void game_tick();
void game_terminar_si_es_hora();
void game_atender_teclado(unsigned char tecla);
void prueba_lanzar_pirata(pirata_t *p);
int game_id_from_selector(int index);
int game_id_from_index(int index);

void print_pirata_mapa(pirata_t *p, direccion dir, int *x , int *y);
void print_cr3(jugador_t *j);
void mapearle_pila_tarea();
void desmapearle_pila_tarea();



#endif  /* !__GAME_H__ */
