/* Movimiento (app de fábrica de senew): el acelerómetro del W580i en vivo y el contador
 * de pasos. Es solo una VISTA: quien muestrea y cuenta es el servicio del sistema (el podómetro), que sigue contando al
 * salir de aquí. Mientras está abierta RETIENE el sensor (ap_acel(1): sin modo «vigilar», para que X e Y se vean en vivo)
 * y la pantalla (ap_retener(W5_RET_ENERGIA): con tope, a los 10 min sin teclas se atenúa y se apaga como siempre). Las dos
 * cosas las suelta el sistema al descargarla, aunque la app muera.
 *
 * Lo que se ve: X e Y en cuentas respecto al cero, un nivel de burbuja con la muestra (punto) y la gravedad estimada
 * (cruz), el eje de trabajo, los pasos, la distancia y la velocidad. Sel. izq. (o C) pone el contador a cero (el del
 * sistema: es el mismo que enseña el standby). «andando» sale de la bandera de actividad (inactivo), no del cerrojo.
 * [sin saber] cuentas por g y la orientación real de los ejes: los rótulos «X» e «Y» son los del ADC.
 * (2026-10-20) Debajo de los crudos, el RANGO visto desde que se abrió (se resalta si se abre más de 6 cuentas): es la prueba de
 * que el sensor se mueve, sin depender del podómetro. «A cero» lo borra también. */
#include "w5.h"
#include "w5_pantalla.h"                  /* (1.7) la letra nueva */

W5_APP(.api_menor = W5_PANT_MENOR, .nombre = "Movimiento", .icono_gen = W5_IC_MOVIMIENTO, .version = 1, .caps = W5_CAP_PASOS | W5_CAP_GIRA);

static struct {
    W5Pasos i;                                     /* la foto del servicio que se está pintando */
    u32 pasos_vistos, muestras_vistas;
    u8 inactivo_visto;
    int px, py, pgx, pgy;                          /* lo que PINTA el nivel: una foto tomada al subir `version` */
    u32 version;
    int rx0, rx1, ry0, ry1, rango;                 /* (2026-10-20, acel_ag) el RANGO de los crudos visto desde que se abrió (o «A cero»):
                                                      si al inclinar no se abre, el ADC no convierte (Sistema -> Opciones -> «Banco acel/audio») */
} s;

static void foto(W5Ctx c)
{
    if (ap_pasos(&s.i) != W5_OK) memset(&s.i, 0, sizeof s.i);
    s.pasos_vistos = s.i.pasos; s.muestras_vistas = s.i.muestras; s.inactivo_visto = s.i.inactivo;
    s.px = s.i.x; s.py = s.i.y; s.pgx = s.i.gx; s.pgy = s.i.gy;
    if (s.i.hay_sensor && s.i.muestras) {
        if (!s.rango) { s.rx0 = s.rx1 = s.i.crudo_x; s.ry0 = s.ry1 = s.i.crudo_y; s.rango = 1; }
        if (s.i.crudo_x < s.rx0) s.rx0 = s.i.crudo_x;
        if (s.i.crudo_x > s.rx1) s.rx1 = s.i.crudo_x;
        if (s.i.crudo_y < s.ry0) s.ry0 = s.i.crudo_y;
        if (s.i.crudo_y > s.ry1) s.ry1 = s.i.crudo_y;
    }
    s.version++; ui_sucio(c);
}
static void sondeo(W5Ctx c)                        /* la pantalla, 5 veces por segundo (y en cada paso, gesto o parada) */
{
    W5Pasos i;
    if (ap_pasos(&i) != W5_OK) return;
    if (i.pasos != s.pasos_vistos || i.inactivo != s.inactivo_visto || i.muestras - s.muestras_vistas >= 5 ||
        i.gesto != s.i.gesto || i.sacudidas != s.i.sacudidas ||
        (!i.hay_sensor && i.fallos % 25 == 1)) foto(c);
}
static void iniciar(W5Ctx c, void *dato)
{
    (void)dato;
    s.rango = 0;
    ap_acel(1);
    ap_retener(W5_RET_ENERGIA, 1);
    foto(c);
    ui_cada(c, 40, sondeo);
}
static void desechar(W5Ctx c, void *dato) { (void)c; (void)dato; ap_acel(0); ap_retener(W5_RET_ENERGIA, 0); }
static int tecla(W5Ctx t, int k, void *dato)
{
    (void)dato;
    if (k == W5K_SEL_IZQ || k == W5K_C) { ap_pasos_a_cero(); s.rango = 0; foto(ui_mi_pantalla(t)); return 1; }
    return ui_teclas_defecto(t, k);
}
/* El nivel: un cuadrado de ±40 cuentas; el punto es la muestra, la cruz la gravedad estimada. */
static void pintar(W5Lienzo l, const W5Rect *rr, void *dato)
{
    W5Rect r = *rr;
    unsigned barra = ui_color(W5_COL_BARRA), tenue = ui_color(W5_COL_TENUE), texto = ui_color(W5_COL_TEXTO), acento = ui_color(W5_COL_ACENTO);
    int lado = (r.w < r.h ? r.w : r.h) - 2, x0 = r.x + (r.w - lado) / 2, y0 = r.y + (r.h - lado) / 2, cx = x0 + lado / 2, cy = y0 + lado / 2;
    int px, py, gx, gy, i;
    (void)dato;
    li_rect(l, x0, y0, lado, lado, barra);
    li_marco(l, x0, y0, lado, lado, tenue);
    li_linea_h(l, x0 + 1, cy, lado - 2, tenue);
    for (i = 1; i < lado - 1; i++) li_punto(l, cx, y0 + i, tenue);
    li_marco(l, cx - lado / 4, cy - lado / 4, lado / 2 + 1, lado / 2 + 1, tenue);       /* ±20 cuentas: hasta donde llega la gravedad estimada */
#define ESC(v) ((v) < -40 ? -40 : (v) > 40 ? 40 : (v))
    gx = cx + ESC(s.pgx) * (lado / 2 - 4) / 40; gy = cy - ESC(s.pgy) * (lado / 2 - 4) / 40;
    px = cx + ESC(s.px) * (lado / 2 - 4) / 40; py = cy - ESC(s.py) * (lado / 2 - 4) / 40;
#undef ESC
    li_linea_h(l, gx - 4, gy, 9, texto);
    for (i = -4; i <= 4; i++) li_punto(l, gx, gy + i, texto);
    li_rect(l, px - 3, py - 3, 7, 7, acento);
}
static const char *signo(int v) { return ui_fmt(v < 0 ? "-%d" : "+%d", v < 0 ? -v : v); }
static W5W construir(W5Ctx c, void *dato)
{
    static const char *const gesto[8] = { "golpe en -X", "golpe en +X", "golpe en +Y", "golpe en -Y", "", "", "SACUDIDA", "quieto" };
    const W5Pasos *p = &s.i;
    unsigned tenue = ui_color(W5_COL_TENUE), acento = ui_color(W5_COL_ACENTO);
    u32 dm = p->dist_dm, kmh10 = p->kmh10;
    int anda = p->andando && !p->inactivo;
    const char *estado = !p->hay_sensor ? "el PMU no contesta: sin sensor"
        : p->eje == 0 ? "tumbado: sin eje, no se cuentan pasos"
        : anda ? "andando"
        : p->andando ? "parado"
        : p->candidatos > 0 && p->idle <= 3u * p->hz ? ui_fmt("posible paso %u de 8…", p->candidatos) : "esperando 8 pasos regulares";
    (void)c; (void)dato;
    return Teclas(tecla, ui_andamio("Movimiento",
        Columna(A_CRUZADO(W5P_ESTIRAR), HIJOS(
            RellenoXY(6, 4, Fila(A_PRINCIPAL(W5P_ENTRE), A_CRUZADO(W5P_FIN), HIJOS(
                Texto(ui_fmt("%u", p->pasos), A_ESCALA(4), A_COLOR(acento)),
                Texto("pasos", A_COLOR(tenue))))),
            RellenoXY(6, 0, Texto(estado, A_COLOR(anda ? acento : ui_color(W5_COL_TEXTO)))),
            RellenoXY(6, 2, Texto(ui_fmt("%u,%u m   %u,%u km/h", dm / 10, dm % 10, kmh10 / 10, kmh10 % 10), A_COLOR(tenue), A_MARCAS(W5_F_ESTILO(W5_T_PEQUENA)))),
            Expandido(Pintura(pintar, A_VERSION(s.version))),
            RellenoXY(6, 1, Fila(A_PRINCIPAL(W5P_ENTRE), HIJOS(
                Texto(ui_fmt("X %s", signo(s.px)), A_MARCAS(W5_F_ESTILO(W5_T_PEQUENA))),
                Texto(ui_fmt("Y %s", signo(s.py)), A_MARCAS(W5_F_ESTILO(W5_T_PEQUENA))),
                Texto(ui_fmt("crudo %d / %d", p->crudo_x, p->crudo_y), A_COLOR(tenue), A_MARCAS(W5_F_ESTILO(W5_T_PEQUENA)))))),
            RellenoXY(6, 1, Texto(s.rango ? ui_fmt("rango X %d..%d  Y %d..%d", s.rx0, s.rx1, s.ry0, s.ry1) : "rango: sin muestras", A_COLOR(s.rango && (s.rx1 - s.rx0 > 6 || s.ry1 - s.ry0 > 6) ? acento : tenue), A_MARCAS(W5_F_ESTILO(W5_T_PEQUENA)))),
            RellenoXY(6, 1, Fila(A_PRINCIPAL(W5P_ENTRE), HIJOS(
                Texto(ui_fmt("eje %s", p->eje == 1 ? "X" : p->eje == 2 ? "Y" : "-"), A_MARCAS(W5_F_ESTILO(W5_T_PEQUENA))),
                Texto(ui_fmt("gravedad %s", signo(s.pgx)), A_MARCAS(W5_F_ESTILO(W5_T_PEQUENA))),
                Texto(signo(s.pgy), A_MARCAS(W5_F_ESTILO(W5_T_PEQUENA)))))),
            RellenoXY(6, 1, Fila(A_PRINCIPAL(W5P_ENTRE), HIJOS(
                Texto(gesto[p->gesto & 7], A_COLOR(p->gesto == 6 ? acento : tenue), A_MARCAS(W5_F_ESTILO(W5_T_PEQUENA))),
                Texto(ui_fmt("%u sacudidas · %u muestras", p->sacudidas, p->muestras), A_COLOR(tenue), A_MARCAS(W5_F_ESTILO(W5_T_PEQUENA)))))))),
        "A cero", "Atrás"));
}
W5Pal app_suceso(int suceso, W5Pal a, W5Pal b)
{
    (void)a; (void)b;
    if (suceso == W5_ABRIR) return (W5Pal)ui_pantalla(construir, iniciar, desechar, NULL);
    return 0;
}
