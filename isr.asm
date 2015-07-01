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

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_tick
extern sched_tarea_actual

;; Syscalls
extern game_syscall_pirata_mover
extern game_syscall_cavar
extern game_syscall_pirata_posicion

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
  call fin_intr_pic1
  call sched_tick 
;  str cx
;  cmp ax, cx
;  je .fin
;
;  mov [selector], ax
;  jmp far [offset]

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
    push ecx
    push eax
    call game_syscall_pirata_mover
    ;aca rompi la pila no ? pop pop ? HAREEEE
    pop ebx
    pop edx
    ;add esp, 16
    jmp .fin

  .cavar:
    push eax
    call game_syscall_cavar
    pop ebx
    ;add esp, 8
    jmp .fin

  .posicion:
    push ecx  ;id del pirata que quiero ver la posicion, va de 0 a 7 o -1 si es el mismo pirata
    push eax
    call game_syscall_pirata_posicion
    pop ebx
    pop edx
    ;add esp, 16
    jmp .fin

  .fin:
  popad
  iret
