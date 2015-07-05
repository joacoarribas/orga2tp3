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
extern prueba_lanzar_pirata

extern game_inicializar
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

    ; Imprimir mensaje de bienvenida

    ; Inicializar pantalla
    call screen_inicializar
    call screen_escribir_nombre

    ; Inicializar el juego
    call tss_inicializar
    lgdt [GDT_DESC]
    call game_inicializar

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
    ;;mov edx, 0
    ;;div edx 

    ; Inicializar tss

    ; Inicializar tss de la tarea Idle

    ; Inicializar el scheduler

    ; Inicializar la IDT
    call idt_inicializar

    ; Cargar IDT
    lidt [IDT_DESC]

    ; Configurar controlador de interrupciones
    ;call deshabilitar_pic
    call resetear_pic
    call habilitar_pic

    ; Cargar tarea inicial
    mov ax, 0x68
    ltr ax
    ;call cargar_tarea_inicial


    ; Habilitar interrupciones
    sti

    ;prueba tarea
;;    xchg bx,bx
    call mmu_inicializar_dir_pirata
    mov cr3, eax
    call prueba_lanzar_pirata
    ; mov ax, 0x88
    ; ltr ax
    ; mov
    ; mov
    jmp 0x80:0x00000000
    
;xchg bx,bx
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
