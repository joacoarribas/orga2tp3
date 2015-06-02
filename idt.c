/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#include "defines.h"
#include "idt.h"
#include "isr.h"
#include "screen.h"

#include "tss.h"

idt_entry idt[255] = { 
/*
  [0] = (idt_entry) {
    (unsigned short)        offset_0_15     
    (unsigned short)        segsel          
    (unsigned short)        attr           
    (unsigned short)        offset_16_31   
  };
*/
};


idt_descriptor IDT_DESC = {
    sizeof(idt) - 1,
    (unsigned int) &idt
};

/*
    La siguiente es una macro de EJEMPLO para ayudar a armar entradas de
    interrupciones. Para usar, descomentar y completar CORRECTAMENTE los
    atributos y el registro de segmento. Invocarla desde idt_inicializar() de
    la siguiene manera:

    void idt_inicializar() {
        IDT_ENTRY(0);


        ...
        IDT_ENTRY(19);

        ...
    }
*/

char *errors[] = {
  "error 0",
  "error 1",
  "error 2",
  "error 3",
  "error 4",
  "error 5",
  "error 6",
  "error 7",
  "error 8",
  "error 9",
  "error 10",
  "error 11",
  "error 12",
  "error 13",
  "error 14",
  "error 15",
  "error 16",
  "error 17",
  "error 18",
  "error 19",
  "error 20"
}; 

#define IDT_ENTRY(numero)                                                                                        \
    idt[numero].offset_0_15 = (unsigned short) ((unsigned int)(&_isr ## numero) & (unsigned int) 0xFFFF);        \
    idt[numero].segsel = (unsigned short) 0x0040;                                                                  \
    idt[numero].attr = (unsigned short) 0x8E00;                                                                  \
    idt[numero].offset_16_31 = (unsigned short) ((unsigned int)(&_isr ## numero) >> 16 & (unsigned int) 0xFFFF);

void print_error(int a) {
  print(errors[a], 33, 20, 10); 
}


void idt_inicializar() {
    // Excepciones
    IDT_ENTRY(0);
    IDT_ENTRY(1);
    IDT_ENTRY(2);
    IDT_ENTRY(3);
    IDT_ENTRY(4);
    IDT_ENTRY(5);
    IDT_ENTRY(6);
    IDT_ENTRY(7);
    IDT_ENTRY(8);
    IDT_ENTRY(9);
    IDT_ENTRY(10);
    IDT_ENTRY(11);
    IDT_ENTRY(12);
    IDT_ENTRY(13);
    IDT_ENTRY(14);
    IDT_ENTRY(15);
    IDT_ENTRY(16);
    IDT_ENTRY(17);
    IDT_ENTRY(18);
    IDT_ENTRY(19);
    IDT_ENTRY(20);
}
