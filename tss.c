/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"


tss tss_inicial = (tss) {
  .ptl     = 0x0, 
  .unused0 = 0x0, 
  .esp0    = 0x0,
  .ss0     = 0x0,
  .unused1 = 0x0, 
  .esp1    = 0x0, 
  .ss1     = 0x0, 
  .unused2 = 0x0, 
  .esp2    = 0x0, 
  .ss2     = 0x0, 
  .unused3 = 0x0, 
  .cr3     = 0x0,
  .eip     = 0x0,
  .eflags  = 0x0,
  .eax     = 0x0, 
  .ecx     = 0x0, 
  .edx     = 0x0, 
  .ebx     = 0x0, 
  .esp     = 0x0,
  .ebp     = 0x0,
  .esi     = 0x0, 
  .edi     = 0x0, 
  .es      = 0x0,
  .unused4 = 0x0, 
  .cs      = 0x0, 
  .unused5 = 0x0, 
  .ss      = 0x0,
  .unused6 = 0x0, 
  .ds      = 0x0,
  .unused7 = 0x0, 
  .fs      = 0x0,
  .unused8 = 0x0, 
  .gs      = 0x0,
  .unused9 = 0x0, 
  .ldt     = 0x0, 
  .unused10= 0x0, 
  .dtrap   = 0x0, 
  .iomap   = 0x0, 
};
tss tss_idle = (tss) {
  .ptl     = 0x0,
  .unused0 = 0x0,
  .esp0    = 0x0, // Preguntar por las pilas de nivel 0,1,2
  .ss0     = 0x48, // y sus selectores, deberia ir el selector de datos de nivel cero?
  .unused1 = 0x0,
  .esp1    = 0x0,
  .ss1     = 0x0,
  .unused2 = 0x0,
  .esp2    = 0x0,
  .ss2     = 0x0,
  .unused3 = 0x0,
  .cr3     = 0x27000, // Dir. del Kernel Page Directory
  .eip     = 0x16000, // Dir. donde se encuentra la tarea Idle
  .eflags  = 0x202, // Eflags con interrupciones habilitadas
  .eax     = 0x0,
  .ecx     = 0x0,
  .edx     = 0x0,
  .ebx     = 0x0,
  .esp     = 0x27000, // Pila del kernel
  .ebp     = 0x27000, // Pila del kernel
  .esi     = 0x0,
  .edi     = 0x0,
  .es      = 0x48, // Selector de segmentos de datos 0
  .unused4 = 0x0,
  .cs      = 0x40, // Selector de segmentos de codigo 0
  .unused5 = 0x0,
  .ss      = 0x48, // Selector de segmentos de datos 0
  .unused6 = 0x0,
  .ds      = 0x48, // Selector de segmentos de datos 0
  .unused7 = 0x0 ,
  .fs      = 0x48, // Selector de segmentos de datos 0
  .unused8 = 0x0,
  .gs      = 0x48, // Selector de segmentos de datos 0
  .unused9 = 0x0,
  .ldt     = 0x0,
  .unused10= 0x0,
  .dtrap   = 0x0,
  .iomap   = 0xFFFF
};

tss tss_jugadorA[MAX_CANT_PIRATAS_VIVOS];
tss tss_jugadorB[MAX_CANT_PIRATAS_VIVOS];

void tss_inicializar() {
  int i = 0;

    /* Segmento Tarea Inicial*/
    /* Offset = 0x068 */    
    /* A REVISARR     */    
    gdt[13] = (gdt_entry) {
        (unsigned short)    0x0067,         /* limit[0:15]  */
        (unsigned short)    ((uint) (&tss_inicial)) & 0x0000FFFF,         /* base[0:15]  DONDE ESTA MI TSS */
        (unsigned char)     (((uint) (&tss_inicial)) & 0x00FF0000) >> 16,           /* base[23:16]  */
        (unsigned char)     0x09,           /* type  CREO QUES ASI       */
        (unsigned char)     0x00,           /* s            */
        (unsigned char)     0x01,           /* dpl NO SE SABE */
        (unsigned char)     0x01,           /* p            */
        (unsigned char)     0x00,           /* limit[16:19] */
        (unsigned char)     0x01,           /* avl          */
        (unsigned char)     0x00,           /* l            */
        (unsigned char)     0x01,           /* db           */
        (unsigned char)     0x00,           /* g            */
        (unsigned char)     (((uint) (&tss_inicial)) & 0xFF000000) >> 24,           /* base[31:24]  */
    };

    /* Segmento Tarea IDLE*/
    /* Offset = 0x070 */    
    /* A REVISARR     */    
    gdt[14] = (gdt_entry) {
        (unsigned short)    0x0067,         /* limit[0:15]  */
        (unsigned short)    ((uint) (&tss_idle)) & 0x0000FFFF,         /* base[0:15]  DONDE ESTA MI TSS */
        (unsigned char)     (((uint) (&tss_idle)) & 0x00FF0000) >> 16,           /* base[23:16]  */
        (unsigned char)     0x09,           /* type         */
        (unsigned char)     0x00,           /* s            */
        (unsigned char)     0x00,           /* dpl NO SE SABE */
        (unsigned char)     0x01,           /* p            */
        (unsigned char)     0x00,           /* limit[16:19] */
        (unsigned char)     0x01,           /* avl          */
        (unsigned char)     0x00,           /* l            */
        (unsigned char)     0x01,           /* db           */
        (unsigned char)     0x00,           /* g            */
        (unsigned char)     (((uint) (&tss_idle)) & 0xFF000000) >> 24,           /* base[31:24]  */
    };

    /* Para reevisar */
  for (i=0; i < 2*MAX_CANT_PIRATAS_VIVOS; i++){
    // completar_tss(&tss_jugadorA[i], i+15); // Esta se va a llamar desdes completar_tss_para_tarea
    cargar_tss_en_gdt(&tss_jugadorA[i], i+15);
  }
}

void completar_tss_para_tarea(pirata_t pirata) {
  // Hay que llamar a mmu_inicializar_dir_pirata
//  if ((*(pirata.jugador)) == jugadorA) {
//  
//  } else {
//  
//  }
}

void completar_tss(tss* t, uint cr3, uint pila0) {
// void completar_tss(int t, uint cr3, uint pila0) {
  // int player = t / MAX_CANT_PIRATAS_VIVOS
  // int pirata = t % MAX_CANT_PIRATAS_VIVO

  // tss *t;
  // switch (player) {
  //   case 0:
  //     t = tss_jugadorA[pirata];
  //     break;
  //   case 1:
  //     t = tss_jugadorB[pirata];
  //     break;

  // }

  t->ptl     = 0x0;
  t->unused0 = 0x0;
  t->esp0    = pila0; // Nueva pagina libre 
  t->ss0     = 0x48; // y sus selectores; deberia ir el selector de datos de nivel cero?
  t->unused1 = 0x0;
  t->esp1    = 0x0;
  t->ss1     = 0x0;
  t->unused2 = 0x0;
  t->esp2    = 0x0;
  t->ss2     = 0x0;
  t->unused3 = 0x0;
  t->cr3     = 0x00000; // Dir. del Kernel Page Directory
  t->eip     = 0x00400000; // Dir. donde se encuentra el codigo 
  t->eflags  = 0x202; // Eflags con interrupciones habilitadas
  t->eax     = 0x0;
  t->ecx     = 0x0;
  t->edx     = 0x0;
  t->ebx     = 0x0;
  t->esp     = 0x40ff4; // Pila del kernel
  t->ebp     = 0x40ff4; // Pila del kernel
  t->esi     = 0x0;
  t->edi     = 0x0;
  t->es      = 0x58; // Selector de segmentos de datos 0
  t->unused4 = 0x0;
  t->cs      = 0x50; // Selector de segmentos de codigo 0
  t->unused5 = 0x0;
  t->ss      = 0x58; // Selector de segmentos de datos 0
  t->unused6 = 0x0;
  t->ds      = 0x58; // Selector de segmentos de datos 0
  t->unused7 = 0x0 ;
  t->fs      = 0x58; // Selector de segmentos de datos 0
  t->unused8 = 0x0;
  t->gs      = 0x58; // Selector de segmentos de datos 0
  t->unused9 = 0x0;
  t->ldt     = 0x0;
  t->unused10= 0x0;
  t->dtrap   = 0x0;
  t->iomap   = 0xFFFF;
}

void cargar_tss_en_gdt(tss *t, int i) {
    gdt[i] = (gdt_entry) {
        (unsigned short)    0x0067,         /* limit[0:15]  */
        (unsigned short)    ((uint) (t)) & 0x0000FFFF,         /* base[0:15]  DONDE ESTA MI TSS */
        (unsigned char)     (((uint) (t)) & 0x00FF0000) >> 16,           /* base[23:16]  */
        (unsigned char)     0x09,           /* type         */
        (unsigned char)     0x00,           /* s            */
        (unsigned char)     0x00,           /* dpl NO SE SABE */
        (unsigned char)     0x01,           /* p            */
        (unsigned char)     0x00,           /* limit[16:19] */
        (unsigned char)     0x01,           /* avl          */
        (unsigned char)     0x00,           /* l            */
        (unsigned char)     0x01,           /* db           */
        (unsigned char)     0x00,           /* g            */
        (unsigned char)     (((uint) (t)) & 0xFF000000) >> 24,           /* base[31:24]  */
    };
}
