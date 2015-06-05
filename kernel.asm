; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "imprimir.mac"

global start

extern GDT_DESC
extern IDT_DESC
extern screen_inicializar 
extern screen_escribir_nombre
extern create_page_directory
extern create_page_table
extern idt_inicializar
;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
iniciando_mr_msg db     'Iniciando kernel (Modo Real)...'
iniciando_mr_len equ    $ - iniciando_mr_msg

iniciando_mp_msg db     'Iniciando kernel (Modo Protegido)...'
iniciando_mp_len equ    $ - iniciando_mp_msg

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    imprimir_texto_mr iniciando_mr_msg, iniciando_mr_len, 0x07, 0, 0

    cli
    ; Habilitar A20
    call habilitar_A20
    ; Cargar la GDT
    lgdt [GDT_DESC]
    ; Setear el bit PE del registro CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    ; Saltar a modo protegido
    jmp 0x40:modoProtegido
    BITS 32
    ; Establecer selectores de segmentos
    modoProtegido:
    xor eax, eax
    mov ax, 1001000b
    mov ds, ax
    mov ss, ax
    mov ax, 1100000b
    mov fs, ax

    ; Establecer la base de la pila
    mov esp, 0x27000

    ; Imprimir mensaje de bienvenida

    ;FALTA PONER TODOS LOS SEGMENTOS IGUALES; ESTARIAMOS CREYENDO QUE HAY QUE HACERLO
    ;mov ax, 1001000b
    ;mov ds, ax 
    ;mov fs, ax 
    ;mov ss, ax 
    ;mov es, ax 
    ;mov gs, ax 
    ; Inicializar el juego
    call idt_inicializar

    lidt [IDT_DESC]
    ; Inicializar pantalla
    call screen_inicializar
    call screen_escribir_nombre

    ; Inicializar el manejador de memoria

    ; Inicializar el directorio de paginas
    call kernel_create_page_directory
    ; Cargar directorio de paginas
    call kernel_create_page_table

    ; Habilitar paginacion
    mov eax, 0x27000
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000 ;habilito paginacion
    mov cr0, eax



    ; Error division por cero, funciona
    ; mov edx, 0
    ; div edx 

    ; Inicializar tss

    ; Inicializar tss de la tarea Idle

    ; Inicializar el scheduler

    ; Inicializar la IDT

    ; Cargar IDT

    ; Configurar controlador de interrupciones

    ; Cargar tarea inicial

    ; Habilitar interrupciones

    ; Saltar a la primera tarea: Idle

    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
