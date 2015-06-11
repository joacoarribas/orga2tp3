; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

BITS 32

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

extern print_error
extern print_Lshift
extern print_Rshift
extern print_Y
extern clear_screen_portion

extern game_tick

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_tick
extern sched_tarea_actual


;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

;%macro PEPE 2
;hola %1
;pepe %2
;%endmacro
;
;
;PEPE carola chau
;
;hola carola
;pepe chau

%macro ISR 1
global _isr%1
_isr%1:
;xchg bx, bx
    mov eax, %1
    push eax
    call print_error
    jmp $
%endmacro


;;
;; Datos
;; -------------------------------------------------------------------------- ;;
; Scheduler

;;
;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19



;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;

global _isr32
_isr32:
;xchg bx, bx
  pushad
  call fin_intr_pic1
  call game_tick 
  popad
  iret


;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;

global _isr33
_isr33:
;xchg bx, bx
  pushad
  call fin_intr_pic1
  .ciclo:
    in al, 0x60
    cmp al, 0x2A
    je .Lshift
    cmp al, 0xAA
    je .clearScreenPortion
    cmp al, 0x36
    je .Rshift
    cmp al, 0xB6
    je .clearScreenPortion
    cmp al, 0x15
    je .printY
    cmp al, 0x95
    je .clearScreenPortion
    jmp .fin

  .Lshift:
    call print_Lshift
    jmp .ciclo

  .Rshift:
    call print_Rshift
    jmp .ciclo

  .printY:
    call print_Y
    jmp .ciclo

  .clearScreenPortion:
    call clear_screen_portion
    jmp .fin

  .fin:
    popad
    iret

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;


