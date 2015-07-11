/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "screen.h"
#include "game.h"


extern jugador_t jugadorA, jugadorB;


static ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;

const char reloj[] = "|/-\\";
#define reloj_size 4


void screen_actualizar_reloj_global()
{
    static uint reloj_global = 0;

    reloj_global = (reloj_global + 1) % reloj_size;

    screen_pintar(reloj[reloj_global], C_BW, 49, 79);
}

void screen_actualizar_reloj_pirata(pirata_t *p)
{
    static uint reloj_global = 0;

    reloj_global = (reloj_global + 1) % reloj_size;

    screen_pintar(reloj[reloj_global], C_BW, p->pos_clock_y, p->pos_clock_x);
}

void screen_pintar(uchar c, uchar color, uint fila, uint columna)
{
    p[fila][columna].c = c;
    p[fila][columna].a = color;
}

//void screen_copiar_pantalla(){
//  int i = 0;
//  while (i < 16){
//    mmu_mapear_pagina((uint*)0x)
//
//    }
//  
//  }
//

uchar screen_valor_actual(uint fila, uint columna)
{
    return p[fila][columna].c;
}

void print(const char * text, uint x, uint y, unsigned short attr) {
    int i;
    for (i = 0; text[i] != 0; i++)
     {
        screen_pintar(text[i], attr, y, x);

        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}
void print_interfaz_debbuger(){
  int x = 23;
  while (x < 52){
    screen_pintar(32, C_FG_BLACK, 6, x);
    screen_pintar(32, C_BG_MAGENTA, 7, x);
    screen_pintar(32, C_FG_BLACK, 41, x);
    x++;
  }

  int y = 6;
  while (y < 42){
    screen_pintar(32, C_FG_BLACK, y, 23);
    screen_pintar(32, C_FG_BLACK, y, 51);
    y++;  
  }
}
void print_registros(uint eax, uint ebx, uint ecx, uint edx, uint esi, uint edi, uint ebp, uint esp, uint eip, short cs, short ds, short es, short fs, short gs, short ss, uint eflags){
  const char* c = "eax" ;  
  print(c, 25, 9, C_FG_LIGHT_GREEN);
  print_hex(eax, 8, 29, 9, C_FG_WHITE);
  c = "ebx";
  print(c, 25, 11, C_FG_LIGHT_GREEN);
  print_hex(ebx, 8, 29, 11, C_FG_WHITE);
  
  c = "ecx";
  print(c, 25, 13, C_FG_LIGHT_GREEN);
  print_hex(ecx, 8, 29, 13, C_FG_WHITE);

  c = "edx";
  print(c, 25, 15, C_FG_LIGHT_GREEN);
  print_hex(edx, 8, 29, 15, C_FG_WHITE);

  c = "esi";
  print(c, 25, 17, C_FG_LIGHT_GREEN);
  print_hex(esi, 8, 29, 17, C_FG_WHITE);

  c = "edi";
  print(c, 25, 19, C_FG_LIGHT_GREEN);
  print_hex(edi, 8, 29, 19, C_FG_WHITE);

  c = "ebp";
  print(c, 25, 21, C_FG_LIGHT_GREEN);
  print_hex(ebp, 8, 29, 21, C_FG_WHITE);

  c = "esp";
  print(c, 25, 23, C_FG_LIGHT_GREEN);
  print_hex(esp, 8, 29, 23, C_FG_WHITE);

  c = "eip";
  print(c, 25, 25, C_FG_LIGHT_GREEN);
  print_hex(esp, 8, 29, 25, C_FG_WHITE);

  c = "cs";
  print(c, 26, 27, C_FG_LIGHT_GREEN);
  print_hex(cs, 4, 29, 27, C_FG_WHITE);

  c = "ds";
  print(c, 26, 29, C_FG_LIGHT_GREEN);
  print_hex(ds, 4, 29, 29, C_FG_WHITE);

  c = "es";
  print(c, 26, 31, C_FG_LIGHT_GREEN);
  print_hex(es, 4, 29, 31, C_FG_WHITE);

  c = "fs";
  print(c, 26, 33, C_FG_LIGHT_GREEN);
  print_hex(fs, 4, 29, 33, C_FG_WHITE);

  c = "gs";
  print(c, 26, 35, C_FG_LIGHT_GREEN);
  print_hex(gs, 4, 29, 35, C_FG_WHITE);

  c = "ss";
  print(c, 26, 37, C_FG_LIGHT_GREEN);
  print_hex(ss, 4, 29, 37, C_FG_WHITE);

  c = "eflags";
  print(c, 26, 39, C_FG_LIGHT_GREEN);
  print_hex(0x0, 8, 33, 39, C_FG_WHITE);

  c = "cr0";
  print(c, 38, 9, C_FG_LIGHT_GREEN);
  print_hex(0x0, 8,  42, 9, C_FG_WHITE);

  c = "cr2";
  print(c, 38, 11, C_FG_LIGHT_GREEN);
  print_hex(0x0, 8, 42, 11, C_FG_WHITE);

  c = "cr3";
  print(c, 38, 13, C_FG_LIGHT_GREEN);
  print_hex(0x0, 8, 42, 13, C_FG_WHITE);

  c = "cr4";
  print(c, 38, 15, C_FG_LIGHT_GREEN);
  print_hex(0x0, 8, 42, 15, C_FG_WHITE);

  c = "stack";
  print(c, 38, 27, C_FG_LIGHT_GREEN);
  
  print_hex(cs, 8, 38, 29, C_FG_WHITE);
  print_hex(cs, 8, 38, 30, C_FG_WHITE);
  print_hex(cs, 8, 38, 31, C_FG_WHITE);
  print_hex(cs, 8, 38, 32, C_FG_WHITE);
  print_hex(cs, 8, 38, 33, C_FG_WHITE);
  }


void print_hex(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char hexa[8];
    char letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}

void clear_screen_error() {

  screen_pintar(32, C_BG_LIGHT_GREY, 20, 10); 

}

void print_dec(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char letras[16] = "0123456789";

    for(i = 0; i < size; i++) {
        int resto  = numero % 10;
        numero = numero / 10;
        p[y][x + size - i - 1].c = letras[resto];
        p[y][x + size - i - 1].a = attr;
    }
}

void screen_inicializar() {
  int i = 0;
  int j = 0;
  for (i=0 ; i < 50 ; i++) {
    for (j=0 ; j < 80 ; j++) {
      screen_pintar(32, C_BG_LIGHT_GREY, i, j);
    }
  }
    for (j=0 ; j < 80 ; j++) {
      screen_pintar(32, C_BG_BLACK, 0, j);
    }
  for (i=43 ; i < 50 ; i++) {
    for (j=0 ; j < 80 ; j++) {
      screen_pintar(32, C_BG_BLACK, i, j);
    }
  }
  for (i=43 ; i < 50 ; i++) {
    for (j=29 ; j < 38 ; j++) {
      screen_pintar(32, C_BG_RED, i, j);
    }
  }
  for (i=43 ; i < 50 ; i++) {
    for (j=38 ; j < 47 ; j++) {
      screen_pintar(32, C_BG_BLUE, i, j);
    }
  }
  uint puntaje = 0;
  print_hex(puntaje, 3, 41, 46, 15);
  print_hex(puntaje, 3, 32, 46, 15);
  //i = 45;
  //j = 8
  //while (j <= 24) {
  //  screen_pintar()
  //
  //}
  print_dec(1, 1, 6, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_RED, 47, 6);
  print_dec(2, 1, 8, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_RED, 47, 8);
  print_dec(3, 1, 10, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_RED, 47, 10);
  print_dec(4, 1, 12, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_RED, 47, 12);
  print_dec(5, 1, 14, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_RED, 47, 14);
  print_dec(6, 1, 16, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_RED, 47, 16);
  print_dec(7, 1, 18, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_RED, 47, 18);
  print_dec(8, 1, 20, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_RED, 47, 20);

  print_dec(1, 1, 60, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_BLUE, 47, 60);
  print_dec(2, 1, 62, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_BLUE, 47, 62);
  print_dec(3, 1, 64, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_BLUE, 47, 64);
  print_dec(4, 1, 66, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_BLUE, 47, 66);
  print_dec(5, 1, 68, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_BLUE, 47, 68);
  print_dec(6, 1, 70, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_BLUE, 47, 70);
  print_dec(7, 1, 72, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_BLUE, 47, 72);
  print_dec(8, 1, 74, 45, C_FG_WHITE);
  screen_pintar(88, C_FG_BLUE, 47, 74);
}

void screen_escribir_nombre() {
  char *nombre = {
    "Super Smash Bros"
  };
  print(nombre, 64, 0, 15);
}

void print_Lshift() {
  print("LShift", 70, 2, 15);
}

void print_Rshift() {
  print("RShift", 70, 2, 15);
}

void print_Y() {
  print ("y", 72, 2, 15);
}
void print_puntaje(jugador_t *j) {

  if (j == &jugadorA) {
    print_hex(j->puntaje, 3, 32, 46, 15);
  } else {
    print_hex(j->puntaje, 3, 41, 46, 15);
  }

}

void limpiar_clock(pirata_t *p) {

  jugador_t *j = p->jugador;

  if (j == &jugadorA) {
    screen_pintar(88, C_FG_RED, p->pos_clock_y, p->pos_clock_x);
  } else {
    screen_pintar(88, C_FG_BLUE, p->pos_clock_y, p->pos_clock_x);
  }

}

void clear_screen_portion() {
  int j = 0;
  for (j=70; j<76; j++)
    screen_pintar(32, C_BG_LIGHT_GREY , 2, j); 
}
