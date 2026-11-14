/* validaciones.h - Ingreso seguro de datos y mensajes al usuario */
#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#include <stddef.h>   /* size_t */

#define TAM_BUFFER 64

/* Todas las funciones de lectura devuelven: 1 = dato valido cargado en *destino
 * 0 = no se pudo leer (EOF / Ctrl+D / puntero NULL). */
int  leer_entero(const char *mensaje, int min, int max, int *destino);
int  leer_double(const char *mensaje, double min, double max, double *destino);
int  leer_si_no(const char *mensaje, int *respuesta);
int  leer_cadena(const char *mensaje, char *destino, size_t tam);

void mostrar_error(const char *mensaje);
void mostrar_advertencia(const char *mensaje);

#endif