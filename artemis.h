/* artemis.h - Tipos, constantes y prototipos de la simulacion (Situacion 1: SLS) */
#ifndef ARTEMIS_H
#define ARTEMIS_H

#include <stdio.h>   /* FILE */

/* ---- Constantes del enunciado ---- */
#define G_TIERRA     9.80
#define A1_DEFECTO   20.0      /* m/s^2  SRB + nucleo        */
#define T1_DEFECTO   120.0     /* s      desacople de SRB    */
#define A2_DEFECTO   12.5      /* m/s^2  solo nucleo         */
#define H_DEFECTO    160000.0  /* m      apagado de motores  */

/* ---- Rangos permitidos para los datos del usuario ---- */
#define A_MIN   0.1
#define A_MAX   100.0
#define T1_MIN  1.0
#define T1_MAX  600.0
#define H_MIN   1000.0
#define H_MAX   1000000.0
#define DT_MIN  0.1
#define DT_MAX  60.0
#define DT_DEFECTO 10.0

#define MAX_FILAS             1000   /* tope de filas de la tabla (se agranda dt si hace falta) */
#define LIMITE_G_TRIPULACION  6.0    /* aviso si la carga supera este valor */
#define ARCHIVO_REPORTE       "reporte_mision.txt"

/* Datos de entrada de la mision */
typedef struct {
    double a1;   /* aceleracion fase 1 [m/s^2] */
    double t1;   /* instante de desacople de los SRB [s] */
    double a2;   /* aceleracion fase 2 [m/s^2] */
    double h;    /* altitud de apagado de motores [m] */
} ParametrosMision;

/* Resultados analiticos (preguntas a, b, c, d + extras) */
typedef struct {
    int    desacople_ocurre;  /* 1 si se llega a t1 antes de alcanzar h          */
    double t_desacople;       /* instante real en que termina la fase 1 [s]      */
    double v1;                /* a) rapidez al final de la fase 1 [m/s]          */
    double y1;                /* a) altitud al final de la fase 1 [m]            */
    double t_corte;           /* instante de apagado de motores [s]              */
    double v2;                /* b) rapidez maxima al apagar motores [m/s]       */
    double h_max;             /* c) altitud maxima (caida libre desde h) [m]     */
    double t_apogeo;          /* instante en que se alcanza h_max [s]            */
    double a1_en_g;           /* d) a1 expresada como multiplo de g              */
    double carga_g_pico;      /* extra: carga "sentida" (a + g)/g                */
} ResultadosMision;

/* ---- Parametros ---- */
void cargar_parametros_por_defecto(ParametrosMision *p);
int  parametros_validos(const ParametrosMision *p);
int  pedir_parametros(ParametrosMision *p);

/* ---- Calculo ---- */
int  calcular_resultados(const ParametrosMision *p, ResultadosMision *r);

/* ---- Salida (FILE* permite usar stdout o un archivo con el mismo codigo) ---- */
void   imprimir_parametros(FILE *out, const ParametrosMision *p);
void   imprimir_resultados(FILE *out, const ParametrosMision *p, const ResultadosMision *r);
/* Tabla basica t / fase / a / v / y cada 'dt' segundos. Devuelve el dt realmente usado. */
double imprimir_tabla(FILE *out, const ParametrosMision *p, const ResultadosMision *r, double dt);

/* ---- Opciones del menu ---- */
void ejecutar_analisis(const ParametrosMision *p);
int  ejecutar_tabla(const ParametrosMision *p);
int  guardar_reporte(const char *ruta, const ParametrosMision *p);

#endif
