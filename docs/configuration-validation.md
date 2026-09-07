# Validación del perfil de medición

`systemConfig` reúne los valores editables antes de compilar; `defaultConfig` es constante. La clase sin estado `profileValidator` recibe el perfil por referencia constante en `validate()`. Devuelve el primer `configError` y una cota en milisegundos; ante errores previos al cálculo, la cota es cero. Un error `responseBound` conserva la cota calculada para diagnóstico. El validador no incluye Arduino, no asigna memoria dinámica y no mantiene estado mutable.

El perfil inicial produce **1147 ms**: `(5 + 3 + 2) × (100 + 10) + 25000 / 1000 + 2 + 20`. La división utiliza `double`, preservando fracciones de milisegundo. Los rangos se comprueban antes de sumar, restar o multiplicar entradas potencialmente extremas. La cota de 2000 ms es inclusiva.

## Repetir las pruebas

Desde la raíz del proyecto, con MSVC instalado:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File test/host/runTests.ps1
```

El runner descubre Visual Studio con `vswhere`; también acepta `-compilerSetup 'C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/VC/Auxiliary/Build/vcvars64.bat'`. No instala dependencias. La opción de ejecución afecta únicamente al proceso PowerShell iniciado. Compila el mismo `src/systemConfig.cpp` empleado por el firmware con `/std:c++17 /EHsc /W4 /WX /DNDEBUG`. Los fallos se registran explícitamente y producen salida distinta de cero; no dependen de `assert`. Los ejecutables se guardan en `.pio/host-tests`. El batch se escribe en UTF-8 sin BOM y activa `chcp 65001` antes de usar rutas. Se verificó una copia de los fuentes y del runner en `.pio/validación-perfil`, con 705 comprobaciones y salida cero, sin borrar ni mover archivos originales.

La ejecución del 2026-09-07 pasó **705 comprobaciones, 0 fallos**, con MSVC x64 19.51.36244 y sin advertencias. Cubre perfiles inicial y alternativo, extremos enteros, límites inclusivos y estrictos, todos los campos flotantes con NaN e infinitos positivos/negativos, orden de umbrales y frecuencias, caducidad, roles y duplicados GPIO, cota exacta y exceso fraccionario. Incluye el perfil 200/20 ms con L=8 y C=5 que excede 2000 ms.

## Compilación ESP32

```powershell
New-Item -ItemType Directory -Force -Path '.pio/host-tests' | Out-Null
& 'C:/Users/JOSEFRANZ/.platformio/penv/Scripts/python.exe' -m platformio run -e esp32doit-devkit-v1 -v *> '.pio/host-tests/platformio-build.log'
$verboseExitCode = $LASTEXITCODE
Get-Content -Tail 12 '.pio/host-tests/platformio-build.log'
if ($verboseExitCode -ne 0) { exit $verboseExitCode }
& 'C:/Users/JOSEFRANZ/.platformio/penv/Scripts/python.exe' -m platformio run -e esp32doit-devkit-v1 *> '.pio/host-tests/platformio-build-normal.log'
$normalExitCode = $LASTEXITCODE
Get-Content -Tail 12 '.pio/host-tests/platformio-build-normal.log'
exit $normalExitCode
```

Versiones resueltas el 2026-09-07: PlatformIO Core 6.2.0; espressif32 7.1.1; framework-arduinoespressif32 3.20017.241212+sha.dcc1105b (Arduino 2.0.17); toolchain-xtensa-esp32 8.4.0+2021r2-patch5; tool-esptoolpy 2.41100.260830 (esptool 4.11.0).

Se comprobó la compilación efectiva de `main.cpp` y `systemConfig.cpp` mediante `xtensa-esp32-elf-g++` con `-std=gnu++17 -Wall -Wextra`, sin `-std=gnu++11` y sin advertencias propias. El framework también aporta `-Wno-unused-parameter -Wno-sign-compare`. La compilación detallada y la normal finalizaron correctamente; después de ampliar las pruebas, se repitieron las **705 comprobaciones** y la compilación detallada incremental, ambas con salida cero. `.pio/host-tests/platformio-build.log` conserva esta última ejecución incremental y `.pio/host-tests/platformio-build-normal.log` la verificación normal del mismo firmware. Una ejecución previa tuvo un fallo de impresión de SCons (`_Null` + `str`) que no reapareció en las comprobaciones finales. RAM: 21032/327680 bytes; flash: 235509/1310720 bytes. No se modificaron herramientas instaladas.


## Política GPIO y alcance

Los cinco roles TRIG, ECHO y LED estacionario/acercamiento/alejamiento deben usar pines diferentes. Se admiten como GPIO candidatos 0–19, 21–23, 25–27, 32–36 y 39, excluyendo flash 6–11 y strapping 0/2/5/12/15. GPIO 34/35/36/39 solo pueden asignarse a ECHO; 37/38 no están expuestos por ESP32-WROOM-32 y se rechazan para todos los roles. Los demás números inexistentes o fuera de rango también se rechazan. La distinción entre los GPIO del chip y los terminales del módulo se fundamenta en la sección 2 de la [hoja de datos oficial de ESP32-WROOM-32](https://documentation.espressif.com/esp32-wroom-32_datasheet_en.html). Esta política es deliberadamente conservadora respecto a flash y arranque.

La configuración inicial usa 25/26/27/32/33 y polaridad activa alta. `setup()` valida este perfil; `loop()` permanece vacío. No se configuran GPIO, interrupciones, adquisición ni redes y no se cargó el firmware en una placa. La validación lógica no certifica cableado, alimentación, adaptación eléctrica de ECHO, precisión ni tiempos físicos: esas comprobaciones quedan para las historias de hardware.
