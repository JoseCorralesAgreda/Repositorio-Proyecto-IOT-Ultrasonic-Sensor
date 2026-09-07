---
title: 'Completar detector radial en C++ orientado a objetos'
type: feature
created: '2026-09-07'
status: in-progress
route: dispatch
review_loop_iteration: 0
baseline_commit: 368a9571471c3365e86fc2689b12d83bdfacf284
context:
  - _bmad-output/planning-artifacts/architecture/architecture-ultrasonic-proyect-iot-2026-09-06/ARCHITECTURE-SPINE.md
  - _bmad-output/planning-artifacts/epics.md
---

<frozen-after-approval reason="El usuario solicita todos los pasos, conserva C++ y POO y excluye verificaciones de compilación">

## Intent

Completar el software pendiente de las historias 1.2–1.8 y preparar los procedimientos, plantillas de evidencia y guía de 1.6/1.9/2.1–2.3. El usuario autoriza el alcance conjunto y continuar sin aprobaciones intermedias. No hay acceso confirmado al montaje: no se inventan ensayos físicos, calibración ni aceptación.

## Boundaries & Constraints

**Siempre:** C++17 con clases concretas por responsabilidad, nombres camelCase, comentarios en español, funciones de hasta 60 líneas no vacías y tres niveles de anidamiento. Referencias constantes a configuración de vida persistente; almacenamiento fijo y ninguna asignación por muestra. Solo adaptadores incluyen Arduino/ESP-IDF; usar versiones instaladas y APIs inspeccionadas. Seguir C-01 a C-04 y contratos de los documentos de contexto. Conservar todo el trabajo previo.

**Nunca:** ejecutar compiladores, PlatformIO build, runners que compilan o pruebas basadas en binarios anteriores; tampoco instalar dependencias, cargar firmware, acceder a hardware, hacer commits o push. Escribir pruebas C++ para ejecución posterior y revisar estáticamente. No añadir consola, red, persistencia, RTOS propio ni telemetría de producto. La prohibición de compilaciones prevalece sobre las verificaciones del flujo BMAD y los CA que las prescriben.

## I/O & Edge-Case Matrix

| Escenario | Resultado exigido |
|---|---|
| Ecos 580/11600 µs, corrección y distancia no finita | Conversión y corrección antes de validar rango inclusivo; causas tipadas y ausencia de sustitutos. Punto de prueba de distancia flotante real. |
| Tres válidas 100/180/100 a 0/100/200 ms | Mediana 100, fechada 100 ms; pico aislado atenuado y escalón sostenido conservado. |
| Inválido, discontinuidad 89999/110001 µs, tiempo igual/decreciente | Romper segmento; la válida fresca actual es primera del nuevo filtro. Caducada válida actualiza salud pero no filtro. |
| L=3/5/8, constantes y rampas ±6/16/30 con 90/110 ms | L+1 filtradas para velocidad, tiempo real, capacidad nueve, cruce 2^32 µs. |
| Movimiento 4/2, intensidad 12/10 y 25/22 | Igualdades inclusivas, bandas conservan referencia publicada incluso con signo distinto en banda 2–4; inversión usa entradas nuevas. |
| Confirmación, inválidos y caducidad | Publicar pareja completa tras C iguales; cancelar si coincide publicada. Primer inválido retira publicación, tercero sin datos. Edad 350000 caduca aunque no haya cierre. |
| Recuperación e integración | Comprobar caducidad antes/después; salud una vez por resultado. Ocho crudas evalúan, diez publican; no borrar dos veces primera fresca tras fallo. |
| LEDs | Todos los estados y seis parejas; 50 %, fase encendida en cambios, frecuencias del perfil y sin delay. |
| Captura | ECHO alto inicial incompleto; no ascenso noEcho; sin descenso incompleto; descenso fuera de plazo timedOut, exactamente en plazo válido aun leído tarde. Primer par y cierre único. |
| Arranque y GPIO | LEDs seguros independientes antes de validar; perfil inválido nunca arma ni escribe GPIO solicitados. Configuración válida compone servicios persistentes. |

</frozen-after-approval>

## Code Map

- `include/systemConfig.h`, `src/systemConfig.cpp`: perfil y clase `profileValidator` ya implementados; conservar API y valores. Resolver la observación de contexto: L=5 separa cinco posiciones, requiere seis filtradas.
- `src/main.cpp`: arranque mínimo actual; sustituir por composición persistente y delegación a `application`.
- `test/host/runTests.ps1`: runner MSVC UTF-8; ampliar fuentes y ejecutables manteniendo prueba de configuración. No ejecutarlo.
- Framework instalado en `C:/Users/JOSEFRANZ/.platformio/packages/framework-arduinoespressif32`: consultar cabeceras/fuentes de `attachInterruptArg`, GPIO, reloj y exclusión ISR antes de usarlos, sin compilar.
- Arquitectura §4/6/7: contratos y orden normativo. Adaptar `classifyMotion` a método puro de una clase `motionClassifier` sin cambiar reglas. Componer una clase de núcleo puro reutilizada por `application` para probar integración sin hardware.
- `docs/configuration-validation.md`: evidencia histórica exclusiva de 1.1; nunca presentar sus compilaciones como verificación del sistema nuevo.

## Tasks & Acceptance

**Execution:**
- [ ] `include/measurementTypes.h`, `include/systemTypes.h`: contratos tipados compartidos, tiempo int64, resultados opcionales y unidades.
- [ ] `src/processing/measurementProcessor.{h,cpp}`: conversión, corrección, validez, mediana y continuidad.
- [ ] `src/domain/temporalAnalyzer.{h,cpp}`, `src/domain/motionClassifier.{h,cpp}`: ventana circular y clasificación pura con histéresis publicada.
- [ ] `src/state/systemState.{h,cpp}`, `src/outputs/ledController.{h,cpp}`: salud, confirmación, instantánea y patrones temporales.
- [ ] `src/hardware/`: `monotonicClock`, `ultrasonicDriver`, `ledDriver` y mapa `safeBoardPins`. Aislar decisiones de captura en una clase pura testeable con flancos inyectados; sincronización de registro real dentro del adaptador. Planificar próximo inicio real sin ráfagas.
- [ ] `src/application.{h,cpp}`, núcleo puro y `src/main.cpp`: componer e integrar orden completo, arranque inválido seguro y reinicios coordinados. Detener/desarmar limpiamente si se reinicializa.
- [ ] `test/host/`: pruebas de producción para matriz y CA lógicos detallados de las historias, incluido núcleo integrado, captura y reinicio. Registrar pruebas escritas, no ejecutadas; ampliar runner sin ejecutarlo.
- [ ] `docs/`: guía de montaje y operación, plan completo CA-01–14, procedimientos de calibración/60 minutos, plantillas CSV vacías con columnas y criterios, trazabilidad por historia y estado real. Preparar toda evidencia obtenible sin ejecutar hardware; no números medidos inventados ni perfil aceptado ficticio.

**Acceptance Criteria:**
- Dado el sistema pendiente, cuando se inspecciona el cambio, entonces cada componente y su integración están implementados según contratos, sin placeholders de lógica.
- Dada la matriz, cuando se inspeccionan pruebas, entonces existen casos registrados que llaman código de producción y comprueban esperados independientes; su ejecución figura pendiente por instrucción del usuario.
- Dadas historias físicas, cuando se consulta el expediente, entonces existen procedimientos y plantillas utilizables y la aceptación figura pendiente, sin aparentar que documentación sustituye mediciones.
- Dada la prohibición del usuario, cuando se entrega el resultado, entonces no se han ejecutado compilaciones, upload ni binarios de pruebas; solo comprobaciones estáticas y revisión independiente.

## Implementation Notes

Árbol inicial sucio por cambios autorizados de 1.1; se preserva y no requiere nueva aprobación. Alcance conjunto elegido explícitamente mediante «todos los pasos». No hay decisiones funcionales abiertas ni efectos irreversibles. La revisión estática sustituye exclusivamente la actividad de verificación de esta sesión, no los CA físicos o de compilación. Estados de historias con código preparado quedan `review`; historias físicas sin evidencia, `in-progress`. No marcar épicas aceptadas.

## Spec Change Log

## Review Triage Log

## Verification

Solo lectura y análisis estático: `git diff --check`, referencias/includes, firmas y tipos, unidades, fronteras, índices, propiedad de estado, llamadas ISR y límites de funciones. Revisar matriz contra casos escritos; la columna de ejecución siempre indica «no ejecutado por instrucción del usuario». Revisar en paralelo tres lentes BMAD después de implementar. No ejecutar `cl`, `g++`, PlatformIO, runners ni binarios de pruebas.
