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
extern screen_actualizar_reloj_global

extern kernel_create_page_directory
extern kernel_create_page_table

extern idt_inicializar

extern deshabilitar_pic
extern resetear_pic
extern habilitar_pic

extern tss_inicializar
extern cargar_tarea_inicial

extern mmu_inicializar_dir_pirata
extern mmu_inicializar
extern prueba_lanzar_pirata

extern game_inicializar

extern inicializar_sched 
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
    cli
    xor eax, eax
    mov ax, 1001000b
    mov ds, ax
    mov ss, ax
    mov ax, 1100000b
    mov fs, ax

    ; Establecer la base de la pila
    mov esp, 0x27000

    ; Inicializar pantalla
    call screen_inicializar
    call screen_escribir_nombre

    ; Inicializar las tss 
    call tss_inicializar
    lgdt [GDT_DESC]

    ; Inicializar el manejador de memoria
    call mmu_inicializar

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

    ; Inicializo las estructuras del juego
    call game_inicializar
    ; Inicializar el scheduler
    call inicializar_sched

    ; Inicializar la IDT
    call idt_inicializar

    ; Cargar IDT
    lidt [IDT_DESC]

    ; Configurar controlador de interrupciones
    call resetear_pic
    call habilitar_pic

    ; Cargar tarea inicial
    mov ax, 0x68
    ltr ax

    ; Habilitar interrupciones
    sti

    ; Saltar a la primera tarea: Idle
    jmp 0x70:0x00000000

    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
