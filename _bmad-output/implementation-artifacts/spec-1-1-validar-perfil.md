---
title: 'Historia 1.1 — Comprobar el perfil de medición'
type: feature
created: '2026-09-07'
status: done
route: dispatch
review_loop_iteration: 0
baseline_commit: 63f55cc602cc5e09af7c1dea134318d99aac1978
story_key: 1-1-comprobar-el-perfil-de-medición-antes-de-utilizarlo
context: []
---

<frozen-after-approval reason="Alcance aprobado en historia 1.1; usuario autoriza ejecución sin pausas">

## Intent

**Problema:** El proyecto solo contiene plantilla Arduino; no existe perfil central ni validación antes de usarlo. Implementar historia 1.1, no el detector completo.

**Enfoque:** Configuración inmutable central y validador puro C++17 con errores tipados, comprobables en host y compilables para ESP32. Conservar PlatformIO/Arduino, fijar espressif32@7.1.1 y registrar comandos/versiones. Eliminar la función de ejemplo sin propósito de main.cpp; no activar GPIO todavía.

## Boundaries & Constraints

**Siempre:** nombres propios camelCase incluidos tipos y constantes, funciones ≤60 líneas no vacías y ≤3 niveles de anidamiento, responsabilidad en una frase, inicialización explícita, resultados tipados, constantes técnicas nombradas/unidades, sin memoria dinámica en validación. Configuración central consumida por referencia constante; editable antes de compilar. Rechazar no finitos y errores antes de operaciones inseguras. Solo adaptadores incluyen Arduino.

**Fuera de alcance:** sensor, LEDs físicos, ISR, clasificación, redes, persistencia y carga en placa. Validar GPIO lógicamente no certifica montaje. Preservar archivos de planificación previos sin commits, reset ni push.

## I/O & Edge-Case Matrix

| Escenario | Entrada | Resultado |
|---|---|---|
| Perfil inicial | período/tolerancia 100/10 ms, eco 25000 µs; rango 10/200 cm; escala/offset 1/0; L=5; movimiento 4/2, media 12/10, alta 25/22 cm/s; C=3; inválidos=3; stale=350 ms; Hz baja/media/alta=1/2/4 y estado/error=1/2 | válido; cota 1147 ms |
| Perfil alternativo | entrada/salida movimiento 5/3 | válido |
| Período/eco | período 80..200 ms, tolerancia >0 y ≤10 %, timeout >maxDistanceCm×58 µs y <(período−tolerancia)×1000 | rechazar fuera; igualdades estrictas timeout rechazadas |
| Distancia/corrección | 10≤min<max≤200; escala 0,9..1,1; offset −5..5 | inclusivos; fuera o no finito rechazado |
| Ventana/contadores | L entero 3..8; C entero 2..5; invalidLimit entero 1..5 | fuera rechazado |
| Umbrales | 0≤salida<entrada movimiento; entradaMedia>salidaMedia>entradaMovimiento; entradaAlta>salidaAlta>entradaMedia | rechazar igualdad/inversión y no finitos |
| Caducidad | >período+tolerancia y ≤1000 ms | igual a suma inválido; 1000 válido si resto coherente |
| Frecuencias | 0,5≤baja<media<alta≤5; estado/error 0,5..5 | fuera/igualdad direccional/no finito inválido |
| GPIO | inicial TRIG25/ECHO26/LED27/32/33, polaridad activa alta | único y válido para rol; duplicados, inexistentes, reservados flash 6..11, strapping 0/2/5/12/15 y entrada exclusiva 34..39 usada como salida rechazados; ECHO en entrada exclusiva existente permitido |
| Cota/desbordamiento | (L+C+2)×(período+tolerancia)+timeout/1000+2+20; perfil 200/20,L8,C5 | rechazar >2000 ms; conservar fracción del timeout sin truncar; validar rangos antes de operar con entradas extremas |

</frozen-after-approval>

## Code Map

- `platformio.ini`: ESP32 DOIT DevKit V1, espressif32 sin fijar; framework Arduino instalado en C:/Users/JOSEFRANZ/.platformio.
- `src/main.cpp`: plantilla con myFunction y variable no usada; sustituir por arranque mínimo sin GPIO que evalúa perfil.
- `include/` y `test/`: solo README; no hay implementación que migrar.
- Compilador host encontrado: MSVC x64 19.51.36244. Inicializar con `C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/VC/Auxiliary/Build/vcvars64.bat`, compilar con cl /std:c++17 /EHsc /W4. Ejecutar mediante cmd /c con rutas correctamente entrecomilladas. No requiere instalación.
- PRD §3 y arquitectura AD-9/11 son fuentes de límites; historia completa en ../planning-artifacts/epics.md, sección Story 1.1. Política conservadora de pines de placa documentada arriba, sin pretender validación eléctrica.
- Contratos de esa historia: tipo `systemConfig`, instancia constante `defaultConfig`; nombres `samplePeriodMs`, `sampleToleranceMs`, `distanceScale`, `distanceOffsetCm`, `analysisLagSamples`, `motionEnterCmPerSec`, `motionExitCmPerSec`, `mediumEnterCmPerSec`, `mediumExitCmPerSec`, `highEnterCmPerSec`, `highExitCmPerSec`, `staleTimeoutMs`, `lowBlinkHz`, `mediumBlinkHz`, `highBlinkHz`, `statusBlinkHz`, `errorBlinkHz`. Roles `stationaryLedPin`, `approachingLedPin`, `recedingLedPin`; no roles alternativos de error/status/motion. Distancias y umbrales usan float conforme arquitectura; calcular cota sin perder fracciones. Comentarios propios en español.

## Tasks & Acceptance

**Execution:**
- [x] `include/systemConfig.h`, `src/systemConfig.cpp`: perfil con tipos explícitos y valores iniciales, constantes técnicas filtro3/capacidad9/TRIG10µs/58µs por cm, validación pura con causas tipadas y cota verificable.
- [x] `platformio.ini`, `src/main.cpp`: fijar plataforma, retirar estándar previo y seleccionar gnu++17 con Wall/Wextra; integrar validación sin activar hardware.
- [x] `test/host/systemConfigTest.cpp`, `test/host/runTests.ps1`: pruebas nativas del validador de producción, todas las filas, fronteras válidas y fuera para cada familia y no finitos; salida no cero al fallar; runner descubre compilador instalado o acepta ruta explícita sin instalar dependencias.
- [x] `docs/configuration-validation.md`: comandos repetibles, política GPIO, versiones resueltas, evidencia de compilación efectiva y alcance físico pendiente.

**Acceptance Criteria:**
- Dado perfil inicial y alternativo, cuando se validan, entonces pasan y la cota inicial es 1147 ms.
- Dado cada fila de la matriz, cuando se ejecutan pruebas registradas con el código real, entonces cubren éxito/error y todas pasan; no se usan asserts que desaparezcan con NDEBUG.
- Dado entorno objetivo, cuando se compila con flags efectivos registrados, entonces no hay advertencias propias y no se añaden dependencias de ejecución.
- Dado código final, cuando se inspecciona, entonces límites de funciones/estilo y ausencia de estado o memoria innecesarios se cumplen; sin GPIO/ISR ni afirmaciones de aceptación física.

## Implementation Notes

### Continuación del 2026-09-07: C++ orientado a objetos

Por instrucción explícita del usuario, encapsular las reglas en una clase `profileValidator` sin estado mutable, con método público `validate(const systemConfig&) const` y auxiliares privados. Mantener `systemConfig` como objeto de valores central y `defaultConfig` constante. Integrar la clase en `setup()` y probar directamente su API; evitar herencia, asignación dinámica o estado persistente innecesarios. Traducir los comentarios propios al español. Completar las tareas pendientes y verificar el código existente, incluida la política GPIO de la placa. Esta decisión complementa el alcance aprobado sin ampliar la historia a adquisición o clasificación.

Autorización global previa permite continuar sin checkpoints adicionales. Árbol inicial solo tiene documentos BMAD sin seguimiento creados en esta sesión; se preservan. Rama main sin cambio de contexto; ningún commit autorizado ni necesario. Sin decisiones de producto pendientes ni efectos irreversibles. Las selecciones de herramientas de prueba son decisiones de implementación.

## Spec Change Log

- 2026-09-07: continuación autorizada en C++ orientado a objetos. La clase `profileValidator` sustituye la función libre; se conserva el bloque aprobado, la configuración central y la ausencia de hardware activo. Se completa la documentación y se corrige la admisión de GPIO37/38.

## Review Triage Log

Revisión del 2026-09-07 con tres agentes independientes; cada hallazgo conserva su veredicto individual.

| ID | Veredicto | Evidencia y resolución |
|---|---|---|
| BH-1 | low | El batch ASCII sustituye caracteres en rutas con acentos. Corrección directa de codificación y comprobación de una copia en ruta Unicode; patch compartido con EC-1. |
| BH-2 | false | `loop()` está vacío y no existe adquisición que pueda ejecutarse después del retorno de `setup()`. El alcance retomado es la historia 1.1; el guardado de estado de adquisición corresponde a la integración futura, no hay una barrera incumplida por un consumidor actual. |
| BH-3 | maybe-false | El validador cumple exactamente la fórmula aprobada `maxDistanceCm × 58`; el efecto de aplicar la transformación inversa afecta a adquisición/calibración todavía inexistentes. Posible impacto medio sin verificar: registrar para contrastar con los contratos y pruebas del procesador antes de integrar perfiles calibrados. |
| BH-4 | low | La aceptación del perfil y su cota no detectan cambios accidentales en todos los valores iniciales. Añadir expectativas independientes del perfil completo como patch de pruebas. |
| BH-5 | low | `field()` solo compara el error; no protege el cero de cota en rechazo previo ni la conservación de cota excesiva. Añadir expectativas explícitas como patch. |
| BH-6 | low | La alternativa válida de salida probada era únicamente GPIO18. Ampliar la tabla explícita de pines permitidos protege la política declarada; patch de pruebas. |
| BH-7 | medium | Los límites relativos de timeout y caducidad se ejercitaban principalmente con el perfil inicial. Añadir fronteras con período, tolerancia y distancia alternativos; patch compartido para caducidad con VG-1. |
| BH-8 | low | Los límites de validación son literales locales, pero no hay fuentes de producción duplicadas ni consumidor divergente. No se introduce una tabla adicional de constantes para una política fija: el mantenimiento de sus valores no es una operación cotidiana y la propuesta añade abstracción sin corregir un fallo actual. Rechazado. |
| BH-9 | low | El resumen de contexto usa «ventana de cinco salidas», mientras la especificación y épicas definen separación por cinco posiciones. Es un archivo de contexto previo, no modificado en esta continuación; defer para evitar propagar ambigüedad a 1.3. |
| BH-10 | low | La documentación menciona dos logs pero solo nombra uno y omite la captura. Corregir rutas y comandos reproducibles como patch documental. |
| EC-1 | low | Verificado el mismo reemplazo ASCII de BH-1 en `runTests.ps1`; mismo patch, sin hallazgo adicional de dominio. |
| VG-1 | medium | Evidencia de regresión verificada por el revisor: reemplazar la suma por 110 pasa las pruebas originales. Cubrir 111/220/221 ms con período/tolerancia 200/20 mediante patch de pruebas. |

## Verification

Resultado final del 2026-09-07: **705 comprobaciones, 0 fallos**, MSVC x64 con `/std:c++17 /W4 /WX /DNDEBUG`. También pasan en la copia `.pio/validación-perfil`. PlatformIO ESP32 terminó con `SUCCESS` en compilación detallada y normal; última comprobación incremental detallada con salida cero. Firmware: 21032 bytes RAM y 235509 bytes flash. Sin advertencias propias, asignaciones dinámicas ni hardware activado; funciones inspeccionadas dentro de 60 líneas no vacías y tres niveles de anidamiento.

Auditoría de matriz: `profiles`/`initialValues` cubren perfiles y cotas; `integers`/`relativeLimits` cubren período, eco, ventana, contadores y caducidad; `floating`/`ordering` cubren distancia, corrección, no finitos, umbrales y frecuencias; `pins`/`allowedOutputs` cubren GPIO. Todas se invocan desde `main()` y participaron en la ejecución final.

Se aplicaron los patches de las tres revisiones y se registraron dos observaciones en [trabajo diferido](deferred-work.md). Comandos y alcance de la verificación: [documentación](../../docs/configuration-validation.md). No se crea commit: se conserva la restricción explícita del alcance retomado.

- Ejecutar runner PowerShell de test/host; exigir salida cero y casos registrados.
- Usar C:/Users/JOSEFRANZ/.platformio/penv/Scripts/python.exe -m platformio run -e esp32doit-devkit-v1 -v; guardar evidencia de comandos efectivos y versiones en documentación, sin volcar entorno sensible.
