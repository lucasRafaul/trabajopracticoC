/* artemis.c - Logica de la simulacion: calculos fisicos, tabla basica y reportes */
#include <stdio.h>
#include <math.h>
#include "artemis.h"
#include "validaciones.h"

/*PARAMETROS  */

void cargar_parametros_por_defecto(ParametrosMision *p)
{
    if (p == NULL) return;
    p->a1 = A1_DEFECTO;
    p->t1 = T1_DEFECTO;
    p->a2 = A2_DEFECTO;
    p->h  = H_DEFECTO;
}

/* 1 si todos los datos estan dentro de rango (con NaN las comparaciones
 * dan falso, asi que tambien lo rechaza). */
int parametros_validos(const ParametrosMision *p)
{
    if (p == NULL) return 0;
    return p->a1 >= A_MIN  && p->a1 <= A_MAX  &&
           p->t1 >= T1_MIN && p->t1 <= T1_MAX &&
           p->a2 >= A_MIN  && p->a2 <= A_MAX  &&
           p->h  >= H_MIN  && p->h  <= H_MAX;
}

int pedir_parametros(ParametrosMision *p)
{
    ParametrosMision tmp;              /* copia de trabajo */

    if (p == NULL) return 0;
    tmp = *p;

    printf("\n--- Configuracion de la mision ---\n");
    printf("Rangos: a1,a2 en [%g, %g] m/s^2 | t1 en [%g, %g] s | h en [%g, %f] m\n",
           A_MIN, A_MAX, T1_MIN, T1_MAX, H_MIN, H_MAX);
    printf("(Podes usar coma o punto decimal)\n");

    if (!leer_double("  a1 - aceleracion con SRB [m/s^2]: ", A_MIN,  A_MAX,  &tmp.a1)) return 0;
    if (!leer_double("  t1 - desacople de SRB [s]       : ", T1_MIN, T1_MAX, &tmp.t1)) return 0;
    if (!leer_double("  a2 - aceleracion solo nucleo    : ", A_MIN,  A_MAX,  &tmp.a2)) return 0;
    if (!leer_double("  h  - altitud de apagado [m]     : ", H_MIN,  H_MAX,  &tmp.h))  return 0;

    *p = tmp;                          /* recien aca se pisan los datos originales */
    printf("Parametros actualizados.\n");
    return 1;
}

/*CALCULO */

int calcular_resultados(const ParametrosMision *p, ResultadosMision *r)
{
    double a_pico;

    if (r == NULL || !parametros_validos(p))
        return 0;

    /* Fase 1 (MRUV desde el reposo): v = a*t   y = 1/2 * a * t^2 */
    r->v1 = p->a1 * p->t1;
    r->y1 = 0.5 * p->a1 * p->t1 * p->t1;

    if (r->y1 <= p->h) {
        /* Caso normal. Fase 2: v2^2 = v1^2 + 2*a2*(h - y1) */
        r->desacople_ocurre = 1;
        r->t_desacople = p->t1;
        r->v2 = sqrt(r->v1 * r->v1 + 2.0 * p->a2 * (p->h - r->y1));
        r->t_corte = p->t1 + (r->v2 - r->v1) / p->a2;
    } else {
        /* Caso borde: llega a h ANTES de t1 (los SRB nunca se sueltan).
         * h = 1/2 * a1 * t^2  ->  t = sqrt(2h / a1) */
        double t_h = sqrt(2.0 * p->h / p->a1);
        r->desacople_ocurre = 0;
        r->t_desacople = t_h;
        r->v1 = p->a1 * t_h;
        r->y1 = p->h;
        r->v2 = r->v1;
        r->t_corte = t_h;
    }

    /* c) Vuelo inercial desde h con v2 hacia arriba: dh = v2^2 / (2g) */
    r->h_max    = p->h + (r->v2 * r->v2) / (2.0 * G_TIERRA);
    r->t_apogeo = r->t_corte + r->v2 / G_TIERRA;

    /* d) a1 como multiplo de g */
    r->a1_en_g = p->a1 / G_TIERRA;

    /* extra: carga que "siente" la tripulacion = (a + g)/g, con la mayor a */
    a_pico = p->a1;
    if (r->desacople_ocurre && p->a2 > a_pico)
        a_pico = p->a2;
    r->carga_g_pico = (a_pico + G_TIERRA) / G_TIERRA;

    return 1;
}

/*  SALIDA  */

void imprimir_parametros(FILE *out, const ParametrosMision *p)
{
    if (out == NULL || p == NULL) return;
    fprintf(out, "---- Parametros de la mision ----\n");
    fprintf(out, "  a1 (SRB + nucleo)    : %.2f m/s^2\n", p->a1);
    fprintf(out, "  t1 (desacople SRB)   : %.2f s\n", p->t1);
    fprintf(out, "  a2 (solo nucleo)     : %.2f m/s^2\n", p->a2);
    fprintf(out, "  h  (apagado motores) : %.2f m (%.2f km)\n", p->h, p->h / 1000.0);
    fprintf(out, "  g                    : %.2f m/s^2\n", G_TIERRA);
}

void imprimir_resultados(FILE *out, const ParametrosMision *p, const ResultadosMision *r)
{
    if (out == NULL || p == NULL || r == NULL) return;

    fprintf(out, "\n========== RESULTADOS ANALITICOS ==========\n");

    if (r->desacople_ocurre) {
        fprintf(out, "a) Al desacople de los SRB (t = %.2f s):\n", r->t_desacople);
        fprintf(out, "     v1 = %.2f m/s (%.2f km/h)\n", r->v1, r->v1 * 3.6);
        fprintf(out, "     y1 = %.2f m (%.2f km)\n", r->y1, r->y1 / 1000.0);
    } else {
        fprintf(out, "a) AVISO: la nave llega a h = %.2f m a los %.2f s, ANTES del\n", p->h, r->t_desacople);
        fprintf(out, "   desacople previsto (t1 = %.2f s). Los SRB no se desacoplan.\n", p->t1);
        fprintf(out, "     v al apagado = %.2f m/s | y = %.2f m\n", r->v1, r->y1);
    }

    fprintf(out, "b) Rapidez maxima al apagar motores (h = %.2f m):\n", p->h);
    fprintf(out, "     v2 = %.2f m/s (%.2f km/h)\n", r->v2, r->v2 * 3.6);
    fprintf(out, "     instante de apagado: t = %.2f s\n", r->t_corte);

    fprintf(out, "c) Si quedara en caida libre al llegar a h:\n");
    fprintf(out, "     h_max = %.2f m (%.2f km)\n", r->h_max, r->h_max / 1000.0);
    fprintf(out, "     altura extra sobre h: %.2f m | apogeo en t = %.2f s\n",
            r->h_max - p->h, r->t_apogeo);

    fprintf(out, "d) Aceleracion de la fase 1 como multiplo de g:\n");
    fprintf(out, "     a1 / g = %.3f g\n", r->a1_en_g);
    fprintf(out, "     (carga 'sentida' por la tripulacion (a+g)/g = %.3f g)\n", r->carga_g_pico);

    if (r->carga_g_pico > LIMITE_G_TRIPULACION)
        fprintf(out, "  [AVISO] La carga supera %.1f g: riesgo para la tripulacion.\n",
                LIMITE_G_TRIPULACION);
}

/* ARMAMOS LA TABLA PARA LOS REPORTES*/

typedef struct { int fase; double a, v, y; } Estado;

/* Estado en el instante t con las formulas por tramos. En t >= apogeo
 * devuelve exactamente v = 0 e y = h_max (evita "-0.00" por redondeo). */
static Estado estado_en_tiempo(const ParametrosMision *p, const ResultadosMision *r, double t)
{
    Estado e;

    if (t < r->t_desacople) {                       /* fase 1 */
        e.fase = 1;
        e.a = p->a1;
        e.v = p->a1 * t;
        e.y = 0.5 * p->a1 * t * t;
    } else if (t < r->t_corte) {                    /* fase 2 */
        double s = t - r->t_desacople;
        e.fase = 2;
        e.a = p->a2;
        e.v = r->v1 + p->a2 * s;
        e.y = r->y1 + r->v1 * s + 0.5 * p->a2 * s * s;
    } else {                                        /* fase 3: solo gravedad */
        double s = t - r->t_corte;
        e.fase = 3;
        e.a = -G_TIERRA;
        if (t < r->t_apogeo) {
            e.v = r->v2 - G_TIERRA * s;
            e.y = p->h + r->v2 * s - 0.5 * G_TIERRA * s * s;
        } else {                                    /* apogeo */
            e.v = 0.0;
            e.y = r->h_max;
        }
    }
    return e;
}

static void imprimir_fila(FILE *out, const ParametrosMision *p, const ResultadosMision *r, double t)
{
    Estado e = estado_en_tiempo(p, r, t);

    fprintf(out, "%9.2f | %4d | %9.2f | %10.2f | %12.1f\n", t, e.fase, e.a, e.v, e.y);
}

double imprimir_tabla(FILE *out, const ParametrosMision *p, const ResultadosMision *r, double dt)
{
    double dt_min, t;
    int k;

    if (out == NULL || p == NULL || r == NULL || dt <= 0.0)
        return 0.0;

    /* Tope de filas: si dt es muy chico, se agranda */
    dt_min = r->t_apogeo / MAX_FILAS;
    if (dt < dt_min)
        dt = dt_min;

    fprintf(out, "\n%9s | %4s | %9s | %10s | %12s\n", "t [s]", "Fase", "a [m/s^2]", "v [m/s]", "y [m]");
    fprintf(out, "----------+------+-----------+------------+-------------\n");

    /* t = k*dt (y no t += dt) para no acumular error de redondeo */
    for (k = 0; ; k++) {
        t = k * dt;
        if (t >= r->t_apogeo - 1e-9) {              /* ultima fila: el apogeo exacto */
            imprimir_fila(out, p, r, r->t_apogeo);
            break;
        }
        imprimir_fila(out, p, r, t);
    }
    fprintf(out, "Fase 1 = SRB + nucleo | 2 = solo nucleo | 3 = inercial (ultima fila = apogeo)\n");
    return dt;
}

/* OPCIONES DEL MENU  */

void ejecutar_analisis(const ParametrosMision *p)
{
    ResultadosMision res;

    if (!calcular_resultados(p, &res)) {
        mostrar_error("Los parametros no son validos.");
        return;
    }
    imprimir_parametros(stdout, p);
    imprimir_resultados(stdout, p, &res);
}

int ejecutar_tabla(const ParametrosMision *p)
{
    ResultadosMision res;
    double dt = DT_DEFECTO, dt_usado;

    if (!calcular_resultados(p, &res)) {
        mostrar_error("Los parametros no son validos.");
        return 0;
    }
    if (!leer_double("Intervalo de la tabla dt [s] (0.1 a 60): ", DT_MIN, DT_MAX, &dt))
        return 0;

    dt_usado = imprimir_tabla(stdout, p, &res, dt);
    if (dt_usado > dt)
        mostrar_advertencia("dt se aumento para no superar el maximo de filas.");

    imprimir_resultados(stdout, p, &res);
    return 1;
}

int guardar_reporte(const char *ruta, const ParametrosMision *p)
{
    ResultadosMision res;
    FILE *f;

    if (ruta == NULL || !calcular_resultados(p, &res))
        return 0;

    f = fopen(ruta, "w");
    if (f == NULL) {
        mostrar_error("No se pudo abrir el archivo para escribir.");
        return 0;
    }

    fprintf(f, "REPORTE FINAL - ASCENSO DEL SLS (ARTEMIS)\n\n");
    imprimir_parametros(f, p);
    imprimir_resultados(f, p, &res);
    fprintf(f, "\nTabla con dt = %.2f s", DT_DEFECTO);
    imprimir_tabla(f, p, &res, DT_DEFECTO);

    if (fclose(f) != 0) {
        mostrar_error("Error al cerrar el archivo (el reporte puede estar incompleto).");
        return 0;
    }
    return 1;
}
