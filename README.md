# senew-app-movimiento — Movimiento

*Español más abajo — [ir a la versión en español](#español).*

> [!WARNING]
> **senew is a new project; use it at your own risk.** The apps, the system and the tools are young software for a discontinued phone.
> Back up your phone before installing anything and expect rough edges.

Motion: the accelerometer live and a step counter (the counting itself is done by a system service; the app is a view).

This is an app (`.sen`) for **[senew](https://github.com/semakers/senew-w580)**, the open operating system for the Sony Ericsson W580i
(see <https://semakers.com>). It is written against the public API of **[senew-sdk](https://github.com/semakers/senew-sdk)**.

## Build

You need an `arm-none-eabi-gcc` toolchain (Arm GNU Toolchain 15.x tested), GNU make, Python 3.8+ and git.

```sh
git clone https://github.com/semakers/senew-app-movimiento
cd senew-app-movimiento
make              # -> build/movimiento.sen
```

The Makefile looks for the SDK next to this folder (`../senew-sdk`) and, if it is not there, clones it into `./.senew-sdk` the first time.
To use another copy: `make SENEW_SDK=/path/to/senew-sdk`. Everything about the toolchain and the API is in the SDK's documentation.

## Install

Open **SENUS** with the phone connected, go to **Apps** and use **"Subir .sen"** (upload a .sen) to put `build/movimiento.sen` on the phone.

## License

MIT — see [`LICENSE`](LICENSE). The comments in the sources are in Spanish (the project's working language).
Sony Ericsson, W580i and other names are trademarks of their respective owners and are used only to say which hardware this is about; this project is not affiliated with them.

---

<a id="español"></a>

# senew-app-movimiento — Movimiento

> [!WARNING]
> **senew es un proyecto nuevo; úsalo bajo tu propio riesgo.** Las apps, el sistema y las herramientas son software joven para un teléfono descatalogado.
> Haz una copia de seguridad del teléfono antes de instalar nada y cuenta con asperezas.

Movimiento: el acelerómetro en vivo y el contador de pasos (quien cuenta es un servicio del sistema; la app es una vista).

Esta es una app (`.sen`) para **[senew](https://github.com/semakers/senew-w580)**, el sistema operativo abierto del Sony Ericsson W580i
(ver <https://semakers.com>). Está escrita contra la API pública de **[senew-sdk](https://github.com/semakers/senew-sdk)**.

## Compilar

Necesitas una cadena `arm-none-eabi-gcc` (probada la Arm GNU Toolchain 15.x), GNU make, Python 3.8+ y git.

```sh
git clone https://github.com/semakers/senew-app-movimiento
cd senew-app-movimiento
make              # -> build/movimiento.sen
```

El Makefile busca el SDK junto a esta carpeta (`../senew-sdk`) y, si no está, lo clona en `./.senew-sdk` la primera vez.
Para usar otra copia: `make SENEW_SDK=/ruta/a/senew-sdk`. Todo sobre la cadena de herramientas y la API está en la documentación del SDK.

## Instalar

Abre **SENUS** con el teléfono conectado, entra en **Apps** y usa **«Subir .sen»** para poner `build/movimiento.sen` en el teléfono.

## Licencia

MIT — ver [`LICENSE`](LICENSE). Los comentarios de los fuentes están en español (la lengua de trabajo del proyecto).
Sony Ericsson, W580i y otros nombres son marcas de sus respectivos dueños y se usan solo para decir de qué aparato se trata; este proyecto no tiene relación con ellos.
