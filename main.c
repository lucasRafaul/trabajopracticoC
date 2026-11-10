
#include <stdio.h>
#include "artemis.h"
#include "validaciones.h"

static void mostrar_menu(void)
{
    printf("\n=========================================\n");
    printf("   ARTEMIS - SIMULADOR DE ASCENSO (SLS)\n");
    printf("=========================================\n");
    printf(" 1. Configurar parametros de la mision\n");
    printf(" 2. Restaurar valores del enunciado\n");
    printf(" 3. Ver parametros actuales\n");
    printf(" 4. Analisis analitico (preguntas a, b, c, d)\n");
    printf(" 5. Simulacion por eventos (telemetria)\n");
    printf(" 6. Guardar reporte final en archivo\n");
    printf(" 0. Salir\n");
}

int main(void)
{

    do {
        mostrar_menu();
        if (!leer_entero("Elegi una opcion: ", 0, 6, &opcion)) {
            printf("\nEntrada cerrada. Saliendo...\n");
            break;
        }

        switch (opcion) {
        case 1: pedir_parametros(); break;
        case 2:
            cargar_parametros_por_defecto();
            printf("Se restauraron los valores del enunciado.\n");
            break;
        case 3: imprimir_parametros(); break;
        case 4: ejecutar_analisis(); break;
        case 5: ejecutar_simulacion(); break;
        case 6:
            
            else
                mostrar_error("No se pudo generar el reporte.");
            break;
        case 0: printf("Fin de la mision. Hasta la proxima!\n"); break;
        }
    } while (opcion != 0);

    return 0;
}