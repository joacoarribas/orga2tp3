/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCREEN_H__
#define __SCREEN_H__

/* Definicion de la pantalla */
#define VIDEO_FILS 50
#define VIDEO_COLS 80

#define VIDEO                   0x000B8000 /* direccion fisica del buffer de video */


#include "colors.h"
#include "defines.h"

#include "game.h"

/* Estructura de para acceder a memoria de video */
typedef struct ca_s {
    unsigned char c;
    unsigned char a;
} ca;


//struct pirata_t;
//typedef struct pirata_t pirata_t;

//struct jugador_t;
//typedef struct jugador_t jugador_t;

int ee_printf(const char *fmt, ...);


void screen_pintar(unsigned char c, unsigned char color, uint fila, uint columna);
void print(const char * text, unsigned int x, unsigned int y, unsigned short attr);
void print_hex(unsigned int numero, int size, unsigned int x, unsigned int y, unsigned short attr);
void screen_inicializar();

void screen_actualizar_reloj_global();
void screen_actualizar_reloj_pirata (pirata_t *pirata);

void screen_actualizar_posicion_mapa(uint x, uint y);

void screen_escribir_nombre(); // Como coño funcionaba si no lo tenia definido
void clear_screen_portion();
void print_Lshift();
void print_Rshift();
void print_Y();
void print_registros(uint cr0, uint cr2, uint cr3, uint cr4, uint eax, uint ebx, uint ecx, uint edx, uint esi, uint edi, uint ebp, uint esp, uint eip, uchar cs, uchar ds, uchar es, uchar fs, uchar gs, uchar ss, uint eflags, uint pila0, uint pila1, uint pila2, uint pila3, uint pila4);
void print_puntaje(jugador_t *j);
void limpiar_clock(pirata_t *p);
void screen_inversa_copiar_pantalla();
void screen_copiar_pantalla();
#endif  /* !__SCREEN_H__ */
