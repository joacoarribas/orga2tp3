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

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_tick
extern sched_tarea_actual


;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 0
global _isr%0

_isr%0:
xchg bx, bx
    mov eax, %0
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 1
global _isr%1

_isr%1:
xchg bx, bx
    mov eax, %1
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 2
global _isr%2

_isr%2:
xchg bx, bx
    mov eax, %2
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 3
global _isr%3

_isr%3:
xchg bx, bx
    mov eax, %3
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 4
global _isr%4

_isr%4:
xchg bx, bx
    mov eax, %4
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 5
global _isr%5

_isr%5:
xchg bx, bx
    mov eax, %5
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 6
global _isr%6

_isr%6:
xchg bx, bx
    mov eax, %6
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 7
global _isr%7

_isr%7:
xchg bx, bx
    mov eax, %7
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 8
global _isr%8

_isr%8:
xchg bx, bx
    mov eax, %8
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 9
global _isr%9

_isr%9:
xchg bx, bx
    mov eax, %9
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 10
global _isr%10

_isr%10:
xchg bx, bx
    mov eax, %10
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 11
global _isr%11

_isr%11:
xchg bx, bx
    mov eax, %11
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 12
global _isr%12

_isr%12:
xchg bx, bx
    mov eax, %12
    push eax
    call print_error
    jmp $

%endmacro


%macro ISR 13
global _isr%13

_isr%13:
xchg bx, bx
    mov eax, %13
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 14
global _isr%14

_isr%14:
xchg bx, bx
    mov eax, %14
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 15
global _isr%15

_isr%15:
xchg bx, bx
    mov eax, %15
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 16
global _isr%16

_isr%16:
xchg bx, bx
    mov eax, %16
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 17
global _isr%17

_isr%17:
xchg bx, bx
    mov eax, %17
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 18
global _isr%18

_isr%18:
xchg bx, bx
    mov eax, %18
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 19
global _isr%19

_isr%19:
xchg bx, bx
    mov eax, %19
    push eax
    call print_error
    jmp $

%endmacro

%macro ISR 20
global _isr%20

_isr%20:
xchg bx, bx
    mov eax, %20
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
ISR 20

;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;

;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;


