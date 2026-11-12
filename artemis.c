/* artemis.c - Logica de la simulacion: calculos fisicos, tabla basica y reportes */
#include <stdio.h>
#include <math.h>
#include "artemis.h"
#include "validaciones.h"

/* ======================= PARAMETROS ======================= */

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
    printf("Rangos: a1,a2 en [%g, %g] m/s^2 | t1 en [%g, %g] s | h en [%g, %g] m\n",
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

/* ======================= CALCULO ======================= */

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

