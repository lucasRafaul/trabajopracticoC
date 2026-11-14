Trabajo Practica de C, "grupo 30 comision C" 
Simulador de Misiones Artemis - Escenario 1: Ascenso y Propulsion con el SLS

UTN FRRe - Algoritmos y Estructuras de Datos - Ingenieria en Sistemas de Informacion

1. Integrantes, Comision Y legajo
Rafaul, Lucas Ezequiel - Com C - Legajo: 30997
Barrios Jonathan Ariel - Com C - Legajo: 29213
Torales Gastón Ariel - Com C - Legajo : 30431
Mendoza Gonzalo Jesus - Com C - Legajo: 31200


2. Analisis del Escenario **Situación-problema 1: Ascenso y propulsión con el SLS (MRUV y desprendimiento).**
La nave Orion parte del reposo (v₀ = 0 m/s) desde la plataforma (y₀ = 0 m). El cohete SLS la eleva en dos fases con aceleración constante, despreciando la resistencia del aire:

2.1. *Fase 1 — el núcleo y los dos SRB aceleran con `a1` hasta el instante `t1`, en que los SRB se desacoplan.
2.2. *Fase 2 — el núcleo continúa con `a2` hasta la altitud `h`, donde se apagan los motores.

Valores del enunciado: a1 = 20,0 m/s², t1 = 120,0 s, a2 = 12,5 m/s², h = 160.000 m, g = 9,80 m/s².

Reglas del juego

- El usuario elige `a1`, `t1`, `a2` y `h` (o usa los valores del enunciado).
- El programa responde las cuatro preguntas del enunciado:
  - a) rapidez y altitud al desacople de los SRB;
  - b) rapidez al apagar los motores;
  - c) altitud máxima si los motores fallan en `h` (caída libre);
  - d) aceleración de la tripulación como múltiplo de g.
- La misión es EXITOSA si la tripulación no supera 4,0 g en ningún momento del ascenso (regla definida por el equipo) y FALLIDA en caso contrario.
- Los datos deben ser coherentes: al desacople la nave tiene que estar por debajo de la altitud de apagado.

3. Funcionamiento

3.1. Menú principal con 5 opciones (ingresar datos, usar valores del enunciado, ver parametros actuales, guardar reporte, analisis del enunciado, salir).
3.2. Se validan los datos (tipo, rango y coherencia física) antes de simular.
3.3. La simulación calcula las magnitudes con las ecuaciones del MRUV.
3.4. Se muestra un reporte final por pantalla, que también puede guardarse en `reporte_mision.txt`.

Ecuaciones usadas:

- Fase 1: `v1 = a1·t1`, `y1 = ½·a1·t1²`
- Fase 2: `v2² = v1² + 2·a2·(h − y1)`, `t2 = (v2 − v1)/a2`
- Caída libre: `hmax = h + v2²/(2g)`
- Aceleración en g: `a/g`

Estructura del codigo.


main.c --> menú y flujo general
artemis.c                 validaciones.c 
artemis.h                 validaciones.h 
(cálculos físicos,          (ingreso seguro de enteros,
structs                    decimales y opciones,
reporte)                 mensajes de error)

Los resultados de los cálculos se devuelven POR PUNTERO (`ResultadosMision *`) y los datos de entrada se pasan como `const ParametrosMision *`.

4. Compilación y ejecución

Requiere `gcc`
```bash
gcc -Wall -Wextra -std=c11 -o MisionArtemis main.c artemis.c validaciones.c -lm
```

5. Registro de uso de Inteligencia Artificial

| Herramienta |  Prompt / consulta realizada | Cómo se adaptó


Claude Sonnet 5.5 | Ayuda con tratamiento de punteros |  Resolvimos problemas que nos daban muchos problemas a la hora de tratar los punteros sobre todo en artemis.c
Claude Sonnet 5.5 | Como Crear un reporte en un archivo entero, que librerias necesitamos si requerimos librerias |  usamos la libreria estandar <stdio.h> con el comando FILE.

Claude Sonnet 5.5 | Aclaracion: No usamos prompt, en el prompt anterior nos recomendo estos comandos | banderas de compilacion para hacer mas facil encontrar bugs y errores en el codigo -Wall -Wextra -std=c11
