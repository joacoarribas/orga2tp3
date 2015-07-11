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
extern clear_screen_error 

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
extern game_pirata_tick
extern game_pirata_exploto
extern pirata_target_X
extern pirata_target_Y

;; Game
extern game_id_from_selector
extern game_id_from_index
extern prueba_lanzar_pirata
extern mapearle_pila_tarea
extern desmapearle_pila_tarea
extern ver_si_exploto 

extern dame_tipo
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
    mov eax, %1
    push eax
    call print_error
    add esp, 4

    str ax
    push ax
    call game_pirata_exploto 
    pop ax

    jmp 0x70:0x00000000

    .fin:
    call clear_screen_error 
      ; deberia limpiar la pantalla aca?
      popad
      iret
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

  xor eax, eax
  xor ecx, ecx
  call sched_tick 

  push ax
  call game_pirata_tick
  pop ax

  str cx
  cmp ax, cx
  je .fin

  push ax
  call ver_si_exploto
  pop ax
  mov [sched_tarea_selector], ax
  jmp far [sched_tarea_offset]

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
    xor eax, eax
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
    
    jmp 0x70:0x00000000
    jmp .fin

  .cavar:
    xor eax, eax
    str eax
    push eax
    call game_id_from_selector
    add esp, 4
    push eax
    ;xchg bx, bx
    call game_syscall_cavar
    add esp, 4
    jmp 0x70:0x00000000
    jmp .fin

  .posicion:
    xor eax, eax
    str eax
    push eax
    call game_id_from_selector
    add esp, 4
    push ecx  ;id del pirata que quiero ver la posicion, va de 0 a 7 o -1 si es el mismo pirata
    push eax
    ;xchg bx, bx
    call game_syscall_pirata_posicion
    add esp, 8
    jmp 0x70:0x00000000
    jmp .fin

  .fin:
  popad
  iret


  ;xchg bx, bx
  ;push eax
  ;call mapearle_pila_tarea
  ;pop eax

  ;push eax
  ;call pirata_target_Y
  ;mov dword [0x400FFC], 0x20 
  ;pop eax

  ;push eax
  ;call pirata_target_X
  ;mov dword [0x400FF8], 0x23
  ;pop eax

  ;push eax
  ;call desmapearle_pila_tarea
  ;pop eax
  ;push eax
  ;call dame_tipo
  ;pop eax
