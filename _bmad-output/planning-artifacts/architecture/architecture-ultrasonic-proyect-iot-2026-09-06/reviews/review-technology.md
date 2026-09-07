# Revisión tecnológica de arquitectura

Fecha: 2026-09-06. Documento: `../ARCHITECTURE-SPINE.md`. Alcance: decisiones tecnológicas, evidencia local y fuentes primarias; sin cambios de requisitos ni del documento revisado.

## Resultado

**Aprobado; sin hallazgos tecnológicos bloqueantes.** Las decisiones están respaldadas por el entorno existente o señaladas expresamente como decisiones de implementación sujetas a pruebas. No se presume una actualización de versiones ni aceptación física.

## Comprobaciones

| Decisión | Evidencia | Resultado |
|---|---|---|
| PlatformIO, ESP32 DOIT DevKit V1 y Arduino existentes | `platformio.ini`; `platform.json` local; [placa en PlatformIO](https://docs.platformio.org/en/latest/boards/espressif32/esp32doit-devkit-v1.html) | Correcto: plataforma actual sin fijar versión; fijarla en implementación es una decisión declarada. |
| Plataforma 7.1.1, Arduino-ESP32 2.0.17, SDK 4.4.7 y GCC 8.4.0 | `platform.json`, `package.json`, `esp_arduino_version.h`, `esp_idf_version.h`, paquete del toolchain y ejecutable GCC instalados | Coinciden. No se afirma que sean las versiones más recientes. |
| C++17 y núcleo independiente | Toolchain Xtensa GCC 8.4.0 instalado; documento exige cambiar explícitamente estándar y verificar comando efectivo | Viable; actualmente es diseño y no compilación validada. |
| Reloj monotónico de 64 bits | Header instalado `esp_timer.h`; [ESP-IDF 4.4.7](https://docs.espressif.com/projects/esp-idf/en/v4.4.7/esp32/api-reference/system/esp_timer.html) | `esp_timer_get_time()` devuelve `int64_t` en microsegundos; lectura no requiere crear timers propios. |
| Captura GPIO con contexto y sincronización breve | `esp32-hal-gpio.h`, `portmacro.h` locales; [API GPIO oficial](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/gpio.html) | `attachInterruptArg` y primitivas críticas ISR existen en versión instalada. El documento reconoce atomicidad y verificación de ISR/IRAM, sin prometerla por atributo. |
| GPIO25/26/27/32/33 y límites eléctricos | [Datasheet ESP32, tablas de pines y características DC](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf) | Son I/O del ESP32 clásico. El mapa queda condicionado a placa física y CA-14. |
| HC-SR04: 5 V, pulso 10 µs, conversión `/58`, separación de disparos | [Ficha técnica del HC-SR04](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf) | Respaldado; las metas de producto siguen provenientes del PRD. |
| Divisor ECHO y TRIG 3,3 V | Cálculo del divisor en §5, especificación ESP32 y ficha HC-SR04 anteriores | Correctamente presentados como propuesta condicionada a medidas, pues la ficha genérica no garantiza umbrales/VOH de todos los módulos. |
| Arrays fijos, optional, sin tareas adicionales | Contratos y recursos del documento | Proporcional y viable; tamaños y rendimiento deberán verificarse sobre firmware real. |

## Condiciones ya recogidas, no nuevos requisitos

1. La aceptación de TRIG y el nivel ECHO real siguen pendientes de CA-14; no se debe convertir el cálculo de divisor en garantía de compatibilidad universal.
2. La sincronización de ISR y el tiempo de servicio del bucle requieren prueba física. El documento ya separa presupuesto de diseño de rendimiento medido.
3. Fijar solo la plataforma no congela por sí solo todos los paquetes transitivos; §10 ordena registrar sus versiones resueltas. La reproducción de una aceptación debe usar ese registro, sin asumir que toda resolución futura será idéntica.

No se ejecutó firmware, no se instaló software y no se modificó la arquitectura. Las pruebas físicas permanecen previstas, no aprobadas.
