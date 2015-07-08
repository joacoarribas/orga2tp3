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

extern sched_tick
extern sched_proxima_a_ejecutar
extern sched_generar_pirata_jugadorA
extern sched_generar_pirata_jugadorB

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_tick
extern sched_tarea_actual
extern sched_ejecutar_tarea

;; Syscalls
extern game_syscall_pirata_mover
extern game_syscall_cavar
extern game_syscall_pirata_posicion
extern game_atender_teclado

;; Game
extern game_id_from_selector
extern game_id_from_index
extern prueba_lanzar_pirata
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
offset : dd 0
selector : dw 0

%macro ISR 1
global _isr%1
_isr%1:
    mov eax, %1
    push eax
    call print_error
    ; Faltaria borrar la tarea
;    call sched_proxima_a_ejecutar
;    mov [selector], ax
;    jmp far [offset]
;;    #jmp $
;
;    .fin:
;      ; deberia limpiar la pantalla aca?
;      popad
;      iret
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
  xor eax, eax
  call fin_intr_pic1
  call sched_tick 

  str cx
  cmp ax, cx
  je .fin

  push eax
  xchg bx, bx
  call sched_ejecutar_tarea
  pop eax

  mov [selector], ax
  jmp far [offset]

  .fin:
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
    xor eax, eax
    in al, 0x60
    push eax 
    call game_atender_teclado
    add esp, 4

  .fin:
    popad
    iret

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;

; global _isr70
; _isr70:
; ;xchg bx, bx
;   pushad
;   mov eax, 0x42
;   popad
;   iret

;---------------------------------------------------------------------------- ;;

global _isr70
_isr70:
;en eax recibo la accion
;xchg bx, bx
  pushad ;estoy aca ?
  cmp eax, 0x1  
  je .moverse
  cmp eax, 0x2
  je .cavar
  cmp eax, 0x3
  je .posicion

  .moverse:
    str eax
    push ecx
    push eax
    call game_id_from_selector
    pop edx ; levanto en un registro cualquiera
    pop ecx
    push ecx
    push eax
    ;xchg bx, bx
    call game_syscall_pirata_mover
    pop edx
    pop edx
    jmp .fin

  .cavar:
    str eax
    push eax
    call game_id_from_selector
    add esp, 4
    push eax
    xchg bx, bx
    call game_syscall_cavar
    add esp, 4
    jmp .fin

  .posicion:
    str eax
    push eax
    call game_id_from_selector
    add esp, 4
    push ecx  ;id del pirata que quiero ver la posicion, va de 0 a 7 o -1 si es el mismo pirata
    push eax
    xchg bx, bx
    call game_syscall_pirata_posicion
    add esp, 8
    jmp .fin

  .fin:
  popad
  iret
