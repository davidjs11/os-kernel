# Kernel para x86_64 y ARMv8

Núcleo de sistema operativo desarrollado desde cero con soporte para las
arquitecturas **x86_64** y **AArch64 (ARMv8)**, como parte de un Trabajo de Fin
de Grado del Grado en Ingeniería Informática (Universidad de Granada).

El núcleo arranca con el cargador [Limine](https://codeberg.org/Limine/limine)
y se ejecuta sobre QEMU. Incluye gestión de memoria física y virtual, montículo
del núcleo, planificador expropiativo, procesos en espacio de usuario, llamadas
al sistema y una pequeña *shell* de usuario.

## Estructura del código

```
arch/        Código específico de cada arquitectura y plataforma
device/      Controladores de dispositivo (serie, IRQ, temporizador, framebuffer)
include/     Cabeceras públicas del núcleo
kernel/      Núcleo común (mm, sched, syscall, tests, kmain)
user/        Programas de espacio de usuario
external/    Dependencias externas (Limine, se descarga al compilar)
config.mk    Selección de arquitectura y plataforma
Makefile     Sistema de construcción
```

## Requisitos

- Cadena de herramientas cruzada: `x86_64-elf-{gcc,as,ld,objcopy}` o
  `aarch64-elf-{gcc,as,ld,objcopy}`.
- `qemu-system-x86_64` o `qemu-system-aarch64`.
- x86_64: `xorriso`.
- AArch64: `parted`, `mtools` y el firmware `qemu-efi-aarch64`.

## Compilación y ejecución

La arquitectura y la plataforma se eligen en `config.mk` (por defecto,
**x86_64 + PC**; para **AArch64 + QEMU virt** basta con comentar el primer bloque
y descomentar el segundo).

```bash
make          # Compila y genera la imagen arrancable
make run      # Compila y ejecuta en QEMU
make debug    # Ejecuta en QEMU junto a GDB
make clean    # Elimina los artefactos de compilación
make help     # Muestra las arquitecturas disponibles
```

La salida del núcleo y la *shell* de usuario se muestran por el puerto serie,
en la propia terminal. Con `DEBUG` activado en `config.mk` se ejecutan además
las pruebas del núcleo durante el arranque.

## Docker

El `Dockerfile` incluye todas las dependencias, para no tener que instalar la
cadena de herramientas en el sistema:

```bash
docker build -t tfg-kernel .
docker run --rm -v "$PWD":/kernel tfg-kernel make          # Compila
docker run --rm -it -v "$PWD":/kernel tfg-kernel make run  # Compila y ejecuta
```
