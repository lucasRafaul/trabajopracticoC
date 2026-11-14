#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include "validaciones.h"

#define LONG_LINEA 64

void mostrarError(const char *mensaje)
{
    printf("  [!] %s\n", mensaje);
}

int leerLinea(char *buf, size_t tam)
{
    size_t len;
    int c;

    if (fgets(buf, (int)tam, stdin) == NULL) {
        return 0;
    }
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[--len] = '\0';
        if (len > 0 && buf[len - 1] == '\r') {
            buf[--len] = '\0';
        }
    } else {
        /* La linea no entro en el buffer: se descarta el resto y se invalida */
        while ((c = getchar()) != '\n' && c != EOF) {
            /* descartar */
        }
        buf[0] = '\0';
    }
    return 1;
}

/* Quita espacios al inicio y al final (modifica la cadena) */
static char *recortar(char *s)
{
    char *fin;

    while (isspace((unsigned char)*s)) {
        s++;
    }
    fin = s + strlen(s);
    while (fin > s && isspace((unsigned char)*(fin - 1))) {
        fin--;
    }
    *fin = '\0';
    return s;
}

int leerEntero(const char *mensaje, int min, int max, int *valor)
{
    char buf[LONG_LINEA];
    char msg[96];
    char *txt;
    char *fin;
    long n;

    for (;;) {
        printf("%s", mensaje);
        if (!leerLinea(buf, sizeof buf)) {
            return 0;
        }
        txt = recortar(buf);
        if (*txt == '\0') {
            mostrarError("No ingreso ningun valor.");
            continue;
        }
        errno = 0;
        n = strtol(txt, &fin, 10);
        if (*fin != '\0') {
            mostrarError("Ingrese solo un numero entero, sin letras ni simbolos.");
            continue;
        }
        if (errno == ERANGE || n < min || n > max) {
            snprintf(msg, sizeof msg, "El valor debe estar entre %d y %d.", min, max);
            mostrarError(msg);
            continue;
        }
        *valor = (int)n;
        return 1;
    }
}

int leerDouble(const char *mensaje, double min, double max, double *valor)
{
    char buf[LONG_LINEA];
    char msg[96];
    char *txt;
    char *fin;
    char *p;
    double d;
    int caracteresOk;

    for (;;) {
        printf("%s", mensaje);
        if (!leerLinea(buf, sizeof buf)) {
            return 0;
        }
        txt = recortar(buf);
        if (*txt == '\0') {
            mostrarError("No ingreso ningun valor.");
            continue;
        }

        /* Solo se aceptan digitos, signo, punto/coma decimal y notacion cientifica */
        caracteresOk = 1;
        for (p = txt; *p != '\0'; p++) {
            if (*p == ',') {
                *p = '.';   /* aceptar coma decimal */
            }
            if (strchr("0123456789+-.eE", *p) == NULL) {
                caracteresOk = 0;
                break;
            }
        }
        if (!caracteresOk) {
            mostrarError("Ingrese solo un numero (por ejemplo 12.5), sin letras.");
            continue;
        }

        errno = 0;
        d = strtod(txt, &fin);
        if (fin == txt || *fin != '\0') {
            mostrarError("El formato del numero no es valido.");
            continue;
        }
        if (errno == ERANGE || !isfinite(d)) {
            mostrarError("El numero es demasiado grande.");
            continue;
        }
        if (d < min || d > max) {
            snprintf(msg, sizeof msg, "El valor debe estar entre %.1f y %.1f.", min, max);
            mostrarError(msg);
            continue;
        }
        *valor = d;
        return 1;
    }
}

char leerOpcionMenu(const char *mensaje, const char *validas)
{
    char buf[LONG_LINEA];
    char *txt;

    for (;;) {
        printf("%s", mensaje);
        if (!leerLinea(buf, sizeof buf)) {
            return '\0';
        }
        txt = recortar(buf);
        if (strlen(txt) == 1 && strchr(validas, txt[0]) != NULL) {
            return txt[0];
        }
        mostrarError("Opcion invalida. Elija una de las opciones del menu.");
    }
}

void pausar(void)
{
    char buf[LONG_LINEA];

    printf("\nPresione Enter para continuar...");
    leerLinea(buf, sizeof buf);
}
