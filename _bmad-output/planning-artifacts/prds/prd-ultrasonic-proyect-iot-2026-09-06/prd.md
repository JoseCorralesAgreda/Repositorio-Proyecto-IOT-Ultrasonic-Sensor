---
title: "PRD — Sistema inteligente de detección de movimiento ultrasónico"
status: final
created: 2026-09-06
updated: 2026-09-06
language: es
workflow: bmad-prd
mode: create
---

# PRD — Sistema inteligente de detección de movimiento ultrasónico

## 1. Propósito, visión y alcance

Definir el comportamiento verificable de un sistema embebido con ESP32, HC-SR04 y tres LEDs que permita observar si un objeto permanece estacionario, se acerca al sensor o se aleja, y distinguir la intensidad de su movimiento. Este documento es la referencia de producto para diseño, implementación en C++ y aceptación. No constituye evidencia de que el hardware o el firmware ya hayan superado las pruebas.

La detección depende de **cómo cambia la distancia con el tiempo**. Dos objetos inmóviles a distancias distintas deben producir la misma clasificación de movimiento. La distancia absoluta solo determina si una lectura pertenece al intervalo operativo; nunca determina por sí sola la dirección o intensidad.

[SUPUESTO S-01] Se trata de un prototipo educativo de mesa, operado por una persona en interiores, sin funciones de seguridad crítica. Su tarea es observar una superficie reflectante que se mueve aproximadamente sobre el eje del sensor. No se requiere una aplicación ni una interfaz de usuario adicional.

El alcance incluye adquisición periódica, validación, filtrado, análisis temporal, dirección, intensidad, estado, LEDs, configuración y calibración. Quedan fuera conectividad Wi-Fi/Bluetooth, nube, bases de datos, ML, aplicaciones móviles, alarmas sonoras, almacenamiento histórico, OTA y detección de personas o identificación de objetos. No se añaden sensores ni actuadores. Instrumentos de laboratorio utilizados para verificar el prototipo no forman parte del producto.

Contexto existente: `platformio.ini` selecciona `esp32doit-devkit-v1`, plataforma `espressif32` y framework Arduino. `src/main.cpp` contiene una plantilla sin comportamiento de medición implementado. Se conserva ese entorno como punto de partida; no se exige incorporar otro framework.

## 2. Definiciones y límites de interpretación

| Término | Definición observable |
|---|---|
| Muestra válida | Distancia finita dentro del intervalo configurado, obtenida mediante un eco completo dentro del plazo de espera y acompañada por una marca temporal monotónica. |
| Distancia filtrada | Mediana de las últimas tres muestras válidas consecutivas del segmento actual. Su tiempo efectivo es el de la muestra central. |
| Segmento | Secuencia de muestras válidas sin interrupciones por mediciones inválidas ni intervalos temporales fuera de tolerancia. |
| Variación temporal | Diferencia entre dos distancias filtradas separadas por el intervalo de análisis configurado. |
| Velocidad radial estimada | Variación temporal dividida por el tiempo real transcurrido, en cm/s. Es una estimación sobre una ventana, no velocidad instantánea ni velocidad tridimensional. |
| Estacionario | Variación por debajo del umbral de salida del movimiento, o dentro de la banda muerta al iniciar. No demuestra inmovilidad física absoluta. |
| Dirección | Acercamiento si la velocidad es negativa; alejamiento si es positiva. |
| Intensidad | Baja, media o alta según el módulo de la velocidad radial estimada. En estacionario es «ninguna». |
| Estado no evaluable | Inicializando, recuperando, sin datos válidos o configuración inválida; dirección e intensidad no están disponibles. |

Un único HC-SR04 observa cambios en el reflector dominante. No garantiza seguir al mismo objeto cuando cambia el reflector, distinguir varios objetos, ni detectar movimiento lateral a distancia radial constante. Ausencia de eco puede significar objeto fuera de alcance, mala reflexión o desconexión: el sistema informa indisponibilidad de datos, sin inventar una causa.

## 3. Parámetros y reglas de configuración

[SUPUESTO S-02] Los siguientes valores son una configuración inicial propuesta, pendiente de calibración del montaje. Son normativos para las pruebas de referencia; cualquier ajuste debe quedar registrado y repetir las pruebas afectadas. No representan prestaciones ya medidas.

| Identificador camelCase | Inicial | Restricción verificable |
|---|---:|---|
| `samplePeriodMs` | 100 ms | Entre 80 y 200 ms; no disparar ráfagas para recuperar ciclos perdidos. |
| `sampleToleranceMs` | 10 ms | Mayor que cero y como máximo 10 % del período. |
| `echoTimeoutUs` | 25000 µs | Mayor que el tiempo de ida y vuelta para `maxDistanceCm`; menor que el período menos su tolerancia. |
| `minDistanceCm` / `maxDistanceCm` | 10 / 200 cm | 10 ≤ mínimo < máximo ≤ 200 cm para esta versión. |
| `distanceScale` / `distanceOffsetCm` | 1 / 0 cm | Escala finita entre 0,9 y 1,1; desplazamiento finito entre −5 y +5 cm. |
| `analysisLagSamples` | 5 | Entero de 3 a 8; separación nominal = valor × período. |
| `motionEnterCmPerSec` | 4 cm/s | Positivo y mayor que el umbral de salida. |
| `motionExitCmPerSec` | 2 cm/s | 0 ≤ salida < entrada. |
| `mediumEnterCmPerSec` / `mediumExitCmPerSec` | 12 / 10 cm/s | Entrada > salida > `motionEnterCmPerSec`. |
| `highEnterCmPerSec` / `highExitCmPerSec` | 25 / 22 cm/s | Entrada > salida > `mediumEnterCmPerSec`. |
| `confirmationCount` | 3 evaluaciones | Entero entre 2 y 5. |
| `invalidLimit` | 3 intentos | Entero entre 1 y 5. |
| `staleTimeoutMs` | 350 ms | Mayor que período + tolerancia; ≤ 1000 ms. |
| `lowBlinkHz` / `mediumBlinkHz` / `highBlinkHz` | 1 / 2 / 4 Hz | 0,5 ≤ baja < media < alta ≤ 5 Hz. |
| `statusBlinkHz` / `errorBlinkHz` | 1 / 2 Hz | Entre 0,5 y 5 Hz; patrones distinguidos también por combinación de LEDs. |

La mediana de tres y los límites del perfil operativo son decisiones de esta versión; modificar el tamaño del filtro o ampliar el rango requiere actualizar esta especificación. Todos los demás parámetros se agrupan en una configuración explícita, editable antes de compilar. No se exige edición en ejecución ni persistencia en memoria no volátil.

La validación rechaza valores no finitos, relaciones incompatibles, desbordamientos de tiempos o una configuración cuyo peor tiempo calculado de clasificación exceda 2 s. Ese cálculo incluye llenado del filtro, ventana temporal y confirmaciones. Los cambios se aplican tras reiniciar, sin mezclar muestras de configuraciones distintas. La asignación GPIO y polaridad LED son configurables y deben verificarse contra la placa real, sin compartir pines entre señales.

## 4. Comportamiento del sistema

### C-01. Adquisición y filtrado

Cada período se realiza como máximo una adquisición. Cada intento termina en una muestra válida o una causa de invalidez: sin eco, eco incompleto, plazo agotado, valor no finito o distancia fuera de rango. Se aplica la corrección `distanceScale × distancia + distanceOffsetCm` antes de validar el rango. Los extremos del rango son inclusivos.

Se calcula la mediana móvil de tres muestras válidas consecutivas; antes de reunirlas no existe distancia filtrada. Una muestra inválida corta el segmento y vacía el historial temporal y el filtro: nunca se inserta cero, la distancia máxima ni el último valor como sustituto. Un intervalo entre muestras fuera de período ± tolerancia también corta el segmento, pero la muestra actual, si es válida, puede iniciar uno nuevo. Los saltos aislados se atenúan con la mediana; un cambio sostenido dentro de rango debe conservarse, no eliminarse mediante un límite arbitrario de salto.

### C-02. Estimación, confirmación y clasificación

Con `analysisLagSamples + 1` distancias filtradas consecutivas se calcula:

`radialVelocity = (filteredDistanceNow - filteredDistancePrevious) / elapsedSeconds`

`filteredDistancePrevious` es la salida separada por `analysisLagSamples` posiciones y `elapsedSeconds` usa sus tiempos efectivos reales. Tiempos nulos o no crecientes invalidan la evaluación e inician recuperación. Con el perfil inicial se necesitan ocho muestras crudas válidas para la primera evaluación; tres evaluaciones concordantes permiten publicar la primera clasificación en la décima muestra.

Las reglas utilizan la clasificación publicada como referencia:

1. Al iniciar o desde estacionario, se propone movimiento cuando el módulo es ≥ `motionEnterCmPerSec`; en otro caso se propone estacionario.
2. Desde movimiento, módulo ≤ `motionExitCmPerSec` propone estacionario. Dentro de la banda entre el umbral de salida del movimiento y el umbral de entrada se conserva la clasificación publicada, incluso si cambia el signo; no se declara una nueva dirección sin superar el umbral de entrada.
3. Desde movimiento, módulo ≥ entrada determina la dirección por el signo. Una inversión de dirección puede confirmarse directamente sin publicar un estacionario intermedio ficticio.
4. Al entrar en movimiento o cambiar dirección, intensidad alta si módulo ≥ entrada alta, media si ≥ entrada media, baja en otro caso. Mientras se mantiene la dirección: desde baja se asciende al cruzar entradas; desde media se baja si módulo ≤ salida media o se sube si ≥ entrada alta; desde alta se baja a baja si módulo ≤ salida media, a media si ≤ salida alta, y en otro caso se conserva alta. Se permiten saltos de nivel.
5. La pareja candidata (dirección, intensidad), o estacionario, debe repetirse `confirmationCount` evaluaciones consecutivas antes de publicarse. Una candidata distinta reinicia el contador a uno; una candidata igual al estado publicado cancela un cambio pendiente. Una invalidez borra toda confirmación pendiente.

### C-03. Estados y recuperación

| Estado publicado | Entrada | Salida |
|---|---|---|
| Inicializando | Reinicio con configuración válida; sin historial. | Primera clasificación confirmada; o sin datos válidos al cumplirse una condición de fallo. |
| Estacionario | Candidata estacionaria confirmada. | Movimiento confirmado, recuperación o fallo. |
| Acercándose / alejándose | Pareja de dirección e intensidad confirmada. | Otra pareja o estacionario confirmado, recuperación o fallo. |
| Recuperando | Primer intento inválido o discontinuidad temporal desde un estado operativo. | Nueva clasificación confirmada con historial fresco; o sin datos válidos. |
| Sin datos válidos | `invalidLimit` intentos inválidos consecutivos o edad de última muestra válida ≥ `staleTimeoutMs`. Desde arranque, la edad se mide desde el inicio si no hubo muestra válida. | Primera muestra válida inicia recuperación; esta no publica movimiento hasta completar historial y confirmación. |
| Configuración inválida | Falla la validación al arranque. | Reinicio con configuración corregida. No se inicia adquisición. |

Prioridad: configuración inválida, sin datos válidos, inicialización/recuperación y finalmente clasificación operativa. La comprobación de caducidad se realiza aunque no termine un intento de medición. Cualquier estado no evaluable publica dirección e intensidad «no disponibles»; no conserva como vigente la última clasificación. El primer dato inválido durante inicialización conserva ese estado hasta que se cumpla la condición de fallo. Un dato válido reinicia el contador de inválidos consecutivos.

### C-04. Retroalimentación visual

[SUPUESTO S-03] Se asignan roles por posición o etiqueta: `stationaryLed`, `approachingLed` y `recedingLed`. No se presupone disponer de colores específicos. El montaje debe permitir reconocer esos roles.

| Estado | Patrón de los tres LEDs |
|---|---|
| Estacionario | Solo `stationaryLed` encendido continuamente. |
| Acercándose | Solo `approachingLed` parpadea a 1, 2 o 4 Hz según intensidad baja, media o alta. |
| Alejándose | Solo `recedingLed` parpadea a 1, 2 o 4 Hz según intensidad. |
| Inicializando o recuperando | Solo `stationaryLed` parpadea a `statusBlinkHz`. |
| Sin datos válidos | Los tres parpadean sincronizados a `errorBlinkHz`. |
| Configuración inválida | Los tres encendidos continuamente. |

Todo parpadeo tiene ciclo de trabajo de 50 %. Al cambiar el estado o intensidad se inicia la fase encendida del nuevo patrón y se apagan los LEDs ajenos a él. Inicialización y recuperación comparten patrón porque ambas significan «esperando evidencia suficiente». Se necesita observar al menos un período completo para distinguir un LED fijo de uno intermitente. El parpadeo no detiene la adquisición.

## 5. Requisitos funcionales

Todos los requisitos siguientes son obligatorios para la versión inicial. Las referencias C identifican la definición normativa del comportamiento; las CA de §9 establecen su aceptación.

### Adquisición y calidad del dato

| ID | Requisito | Comportamiento | Aceptación |
|---|---|---|---|
| RF-01 | Medir periódicamente y registrar tiempo y validez de cada intento con espera acotada. | C-01 | CA-01, CA-07 |
| RF-02 | Rechazar datos inválidos y evitar que entren al filtro o simulen distancia cero. | C-01, C-03 | CA-02, CA-07 |
| RF-03 | Aplicar la mediana de tres y reconstruir el historial tras una interrupción. | C-01 | CA-03, CA-08 |

### Análisis de movimiento

| ID | Requisito | Comportamiento | Aceptación |
|---|---|---|---|
| RF-04 | Calcular variación y velocidad usando diferencias temporales reales, sin clasificar movimiento por proximidad. | C-02 | CA-04, CA-05 |
| RF-05 | Publicar estacionario, acercándose o alejándose según umbrales, signo y confirmación. | C-02 | CA-04, CA-05, CA-06 |
| RF-06 | Clasificar movimiento en intensidad baja, media o alta con histéresis. | C-02 | CA-05, CA-06 |

### Estado, salida y ajuste

| ID | Requisito | Comportamiento | Aceptación |
|---|---|---|---|
| RF-07 | Gestionar arranque, indisponibilidad y recuperación sin publicar información obsoleta. | C-03 | CA-07, CA-08 |
| RF-08 | Representar todos los estados e intensidades mediante los tres LEDs de §4. | C-04 | CA-09 |
| RF-09 | Permitir configurar los parámetros de §3 y rechazar combinaciones inválidas antes de adquirir. | §3, C-03 | CA-10 |
| RF-10 | Permitir aplicar calibración de distancia y umbrales y registrar el perfil usado en las pruebas. | §3, §8 | CA-11 |

## 6. Requisitos no funcionales y restricciones de implementación

| ID | Requisito verificable | Aceptación |
|---|---|---|
| RNF-01 | Con el perfil inicial, iniciar adquisiciones cada 100 ± 10 ms durante 10 min y aplicar cambios LED en ≤ 20 ms desde una transición publicada. La adquisición no puede bloquear más de `echoTimeoutUs` + 2 ms. | CA-01, CA-09 |
| RNF-02 | Con perfil inicial y datos válidos continuos, clasificar desde arranque en ≤ 1,2 s y responder en ≤ 1,5 s a inicio, detención o inversión de las rampas constantes de CA-05/08. El límite no se aplica a estímulos que alternen candidatas sin completar la confirmación. | CA-05, CA-08 |
| RNF-03 | En 60 min de funcionamiento alternando fases válidas e inválidas no debe haber reinicios inesperados, bloqueos ni crecimiento monotónico de memoria. El incremento máximo de memoria dinámica retenida, respecto al final del arranque, será ≤ 1 KiB. | CA-12 |
| RNF-04 | La RAM reservada por buffers y estado propios de la aplicación será ≤ 8 KiB; firmware ≤ 80 % de la partición de aplicación seleccionada. No se crean buffers que crezcan con el tiempo. | CA-12 |
| RNF-05 | Implementar en C++ con identificadores propios camelCase, incluidos tipos y constantes; se exceptúan nombres impuestos por bibliotecas, macros o plataforma. Compilar sin advertencias atribuibles al código propio con `-Wall -Wextra`. | CA-13 |
| RNF-06 | Usar constantes con nombre y unidad para parámetros físicos/temporales, sin duplicar reglas de clasificación. No dejar código muerto ni estado global mutable salvo objetos necesarios de integración, documentados. Separar adquisición, análisis y LEDs mediante responsabilidades identificables y permitir probar el análisis con datos sintéticos sin hardware. | CA-13 |
| RNF-07 | Limitar funciones propias a 60 líneas no vacías y tres niveles de anidamiento; toda unidad tiene una responsabilidad descrita en una frase. Aplicar SRP y bajo acoplamiento sin exigir jerarquías ni clases donde basten funciones. Usar alcance y tipos explícitos, inicialización y gestión automática de recursos; evitar asignaciones dinámicas por muestra. | CA-13 |
| RNF-08 | Funcionar sin red ni servicios externos; no incorporar dependencias de ejecución adicionales al framework y herramientas ya necesarios para ESP32. | CA-13 |
| RT-01 | Usar un ESP32, un HC-SR04 y tres LEDs. Alimentación, resistencias limitadoras, adaptación de nivel ECHO y cableado son elementos eléctricos necesarios, no nuevas funciones. Ningún GPIO debe recibir tensión o corriente fuera de los límites documentados del dispositivo. | CA-14 |

Los límites de recursos, estilo y latencia son objetivos del proyecto propuestos como parte de S-02; se verifican sobre la compilación y placa objetivo. No se afirma que el esqueleto existente los cumpla.

## 7. Entorno, supuestos y riesgos

[SUPUESTO S-04] La aceptación física se realiza a 20–25 °C, en interior, con sensor fijo, alimentación estable, un reflector plano rígido de al menos 20 × 20 cm orientado perpendicularmente al eje, entre 10 y 200 cm, sin otros emisores ultrasónicos activos cercanos. Velocidades de referencia: 6, 16 y 30 cm/s. Fuera de estas condiciones no se garantiza el rendimiento sin caracterización adicional.

| Riesgo o limitación | Respuesta exigida y verificación |
|---|---|
| Ruido alrededor de un umbral | Histéresis y confirmaciones; CA-06 y CA-11. No elevar umbrales hasta ocultar los movimientos de referencia. |
| Eco perdido, sensor desconectado u objeto absorbente | Estado no evaluable según C-03; CA-07. No prometer diagnóstico de la causa. |
| Reflector sustituido o ecos múltiples | Documentar que un salto persistente puede interpretarse como movimiento; no añadir identificación de objetos. CA-03 solo exige rechazo del pico aislado. |
| Movimiento lateral o demasiado lento | Puede clasificarse estacionario: se mide componente radial dentro de la sensibilidad configurada. |
| Temperatura, orientación y montaje | Calibración bajo S-04; repetirla al cambiar montaje o condiciones. No añadir sensor térmico. |
| Compatibilidad eléctrica | Verificar ECHO y GPIO antes de conectar; CA-14. El firmware no corrige una sobretensión. |

## 8. Calibración y puesta en servicio

1. Registrar placa, módulo, GPIO, polaridad de LEDs, alimentación, adaptación de nivel, versión del firmware, temperatura aproximada y configuración. Confirmar RT-01 antes de energizar el conjunto conectado.
2. Con el reflector inmóvil a 20, 100 y 180 cm, obtener 100 muestras válidas por posición. Ajustar escala/desplazamiento si es necesario dentro de §3. Tras el ajuste, exigir error de la mediana respecto a la referencia ≤ 2 cm y al menos 95 % de lecturas válidas por posición. Si no se cumple, revisar montaje y módulo; no declarar calibración aprobada.
3. En cada posición registrar 60 s de velocidad filtrada. Medir el percentil 99 de su módulo; debe quedar por debajo del umbral de salida. Si no, corregir montaje o ajustar umbrales dentro de §3 y repetir toda la aceptación de sensibilidad y latencia. El ajuste no puede eliminar la detección a 6 cm/s.
4. Ejecutar recorridos de referencia en ambas direcciones a 6, 16 y 30 cm/s. Confirmar baja, media y alta, respectivamente. Los umbrales revisados deben conservar esas clasificaciones en el perfil aceptado.
5. Archivar resultados y configuración en un informe de pruebas del proyecto. El registro puede realizarse por instrumentación de prueba o depurador; no obliga a incluir consola, pantalla, almacenamiento o telemetría en el producto.

Recalibrar tras cambiar sensor, montaje, alimentación o perfil. Si una calibración no alcanza los criterios, se registra como fallida y se corrige o se revisa explícitamente el PRD; no se sustituyen los límites de aceptación silenciosamente.

## 9. Criterios de aceptación y método de prueba

Las pruebas de lógica inyectan muestras y tiempos en el análisis sin requerir un segundo sensor. Las físicas usan marcas de distancia, referencia temporal y observación/medición de los GPIO. Registrar entrada, estado, intensidad, tiempo y resultado esperado/obtenido. La instrumentación se limita a la verificación y no amplía funcionalidades del producto.

Los números de las pruebas temporales y de frontera corresponden al perfil inicial. Tras calibrar, repetirlas sustituyendo fronteras por los parámetros efectivos y conservando pruebas de igualdad y ±0,01 cm/s. Recalcular períodos, caducidad y número de muestras con §3, y documentar límites de latencia del perfil final, siempre ≤ 2 s. Los objetivos físicos de ruido, error, recorridos a 6/16/30 cm/s y sus clasificaciones se conservan. CA-10 sigue incluyendo el perfil inicial y el perfil alternativo indicado.

| ID | Procedimiento y resultado exigido |
|---|---|
| CA-01 | Medir 10 min de adquisiciones bajo datos válidos y ausencia de eco por separado: todos los intervalos dentro de 100 ± 10 ms; espera individual ≤ 27 ms; ningún disparo de recuperación en ráfaga. |
| CA-02 | Inyectar 10 y 200 cm: válidos. Inyectar 9,99; 200,01; cero; negativo; NaN; infinito; eco ausente e incompleto: inválidos. No aumentan el tamaño del historial ni generan velocidad. |
| CA-03 | Inyectar 100, 100, 180, 100, 100 cm a 100 ms: las tres medianas son 100 cm. Inyectar después suficientes valores constantes de 120 cm: la salida alcanza 120 cm tras dos muestras nuevas; no queda bloqueada en 100 cm. |
| CA-04 | Inyectar por separado 20, 100 y 180 cm constantes durante 10 s: tras inicialización todas las evaluaciones publicadas son estacionario. En prueba física, tras 2 s de asentamiento, cero transiciones a movimiento durante 60 s en cada posición de calibración. |
| CA-05 | Inyectar rampas a ±6, ±16 y ±30 cm/s durante 3 s, dentro de rango: dirección correcta y baja/media/alta en el 100 % de evaluaciones tras 1,5 s. Repetir con intervalos alternos de 90 y 110 ms, construyendo distancia con el tiempo real; error de velocidad ≤ 0,1 cm/s. Realizar 10 recorridos físicos por combinación, de ≥ 3 s, velocidad de referencia ±10 %: al menos 9 de 10 alcanzan dirección/intensidad correctas dentro de 1,5 s y las conservan el resto del recorrido. |
| CA-06 | En el clasificador, entrada exacta de 4 cm/s durante tres evaluaciones produce movimiento; 3,99 desde estacionario no. Desde movimiento, un módulo de velocidad de 2 cm/s produce estacionario tras tres evaluaciones; 2,01 cm/s conserva movimiento. Probar ascensos en 12 y 25, descensos en 10 y 22, y valores ±0,01 de cada frontera. Una candidata distinta durante solo una o dos evaluaciones no cambia el estado. Alternar 11,9/12,1 partiendo de baja y 24,9/25,1 partiendo de media durante 20 evaluaciones: sin cambio. Invertir signo con módulo 6: nueva dirección tras tres evaluaciones. |
| CA-07 | Desde movimiento, un inválido produce recuperando y valores no disponibles en ≤ 20 ms tras cerrar el intento. Tres inválidos consecutivos producen sin datos válidos en el tercer intento. Sin intentos completados, edad de 350 ms produce ese estado en ≤ 20 ms adicionales. En ningún caso aparece estacionario como sustituto. Repetir desde arranque sin eco. |
| CA-08 | Desde encendido, ocho muestras válidas de una rampa constante de CA-05 o distancia fija producen primera evaluación y diez producen primera publicación; total ≤ 1,2 s con perfil inicial. Desde fallo, repetir con diez válidas consecutivas del mismo estímulo: primera muestra inicia recuperación y décima permite clasificar, sin reutilizar historial anterior. Un intervalo de 150 ms reinicia historial. Probar reposo → rampa, rampa → reposo y rampa → rampa de signo opuesto para cada módulo 6/16/30 cm/s; mantener cada fase ≥ 3 s, dentro de rango: publicación correcta ≤ 1,5 s desde el cambio. Simular cruce del contador temporal de plataforma: sin tiempos negativos, falsos fallos ni bloqueo. |
| CA-09 | Forzar cada estado y las seis combinaciones dirección/intensidad: GPIO coinciden con C-04; frecuencia y ciclo de trabajo con error ≤ 10 %, LEDs ajenos apagados. Observar cada patrón por ≥ 4 s. Cambio de patrón ≤ 20 ms; continuar cumpliendo CA-01 mientras parpadea. |
| CA-10 | Compilar y ejecutar perfil inicial y un perfil válido distinto (por ejemplo entrada/salida 5/3 cm/s): cambia la frontera de clasificación. Probar salida ≥ entrada, NaN, período de 50 ms, rango invertido, GPIO repetidos y latencia calculada > 2 s: estado configuración inválida, sin disparos TRIG. |
| CA-11 | Completar todos los pasos de §8: errores ≤ 2 cm, validez ≥ 95 %, percentil 99 de ruido bajo umbral de salida, CA-04 y recorridos CA-05 aprobados. Informe identifica perfil y montaje reproducibles. |
| CA-12 | Ejecutar 60 min alternando 30 s con reflector válido y 30 s sin eco: cero reinicios/bloqueos, recuperación automática conforme CA-08. Medir memoria cada minuto; crecimiento retenido ≤ 1 KiB y sin tendencia creciente. Informe de compilación/mapa confirma buffers/estado ≤ 8 KiB y firmware ≤ 80 % de partición. |
| CA-13 | Compilar para el entorno declarado con advertencias habilitadas; inspeccionar código propio y dependencias: cumplir cada condición RNF-05 a RNF-08. Ejecutar CA-02 a CA-08 sobre lógica desacoplada de GPIO. Registrar evidencia de nombres, límites de funciones, responsabilidades, constantes y ausencia de duplicación/estado innecesario. |
| CA-14 | Inspeccionar esquema y montaje; medir ECHO en el pin ESP32 y corrientes de LEDs. Cumplir límites de hoja de datos de la placa/chip y LEDs concretos; documentar alimentación, masa común, limitación de corriente y adaptación de nivel necesaria. No conectar ECHO de 5 V directamente a GPIO de 3,3 V. |

## 10. Éxito y cierre de aceptación

| Métrica | Objetivo y trazabilidad |
|---|---|
| ME-01: clasificación útil | ≥ 90 % de recorridos físicos correctos por combinación, según CA-05; RF-04 a RF-06. |
| ME-02: rechazo de falso movimiento | Cero transiciones durante cada ensayo estacionario de 60 s, CA-04; RF-03 y RF-05. |
| ME-03: respuesta | Límites de RNF-01/02 y CA-07 satisfechos; RF-01, RF-07 y RF-08. |
| ME-04: continuidad | CA-12 aprobado; RNF-03/04. |

ME-02 contrapesa la sensibilidad de ME-01; ME-03 impide ocultar ruido con un filtrado excesivamente lento. No se acepta mejorar una sacrificando otra. La aceptación exige aprobar **todos** los CA aplicables al perfil final, adjuntar evidencias reproducibles y no mantener incumplimientos abiertos. Un PRD finalizado no equivale a producto aceptado.

Las tablas de §5 y §6 constituyen la matriz de trazabilidad requisito → comportamiento → aceptación (para restricciones, el comportamiento está contenido en el propio requisito). Las métricas anteriores añaden la relación con el resultado del producto.

## 11. Supuestos y decisiones pendientes

| ID | Supuesto adoptado | Responsable y condición de revisión |
|---|---|---|
| S-01 | Uso educativo en mesa, un operador y movimiento radial. | Responsable del proyecto: antes de ampliar el uso; cambiar a uso crítico obliga a redefinir alcance. |
| S-02 | Perfil inicial, filtrado, umbrales, presupuestos y tiempos propuestos. | Responsable de firmware: durante calibración y antes de aceptación física. Conservar un perfil que apruebe todos los CA. |
| S-03 | LEDs diferenciados por rol/posición, intensidad por frecuencia. | Responsable del montaje: al documentar conexiones; comprobar CA-09. |
| S-04 | Entorno y reflector de referencia. | Responsable de pruebas: antes de ensayos; cambios requieren nueva caracterización. |

Pendientes de ejecución: identificar variante exacta del HC-SR04 y LEDs, asignar GPIO compatibles y dimensionar componentes eléctricos necesarios. Responsable: montaje, antes de cablear; evidencia: CA-14. Obtener valores calibrados y medir recursos reales. Responsable: firmware/pruebas, antes de aceptación; evidencia: CA-11/12. Estas tareas no bloquean el diseño lógico ni justifican añadir funciones; sí condicionan energización y aceptación física. No queda una decisión funcional imprescindible sin comportamiento de referencia definido.

## 12. Fuentes y procedencia

- Solicitud del proyecto: fuente de alcance, flujo principal, hardware, lenguaje, estilo y trazabilidad exigida.
- `platformio.ini` y `src/main.cpp`: contexto local revisado; configuración ESP32/Arduino y plantilla inicial.
- [Ficha HC-SR04 de ElecFreaks, alojada por SparkFun](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf): alimentación nominal de 5 V, disparo TRIG de al menos 10 µs y ciclo recomendado mayor de 60 ms. El período inicial de 100 ms respeta esa recomendación; el intervalo operativo 10–200 cm es una decisión conservadora del proyecto, no el rango nominal completo del sensor.
- [Guía técnica de Adafruit para sensores ultrasónicos](https://learn.adafruit.com/ultrasonic-sonar-distance-sensors/overview): ECHO de 5 V en el HC-SR04 estándar y necesidad de adaptación para lógica de 3,3 V.
- [FAQ de diseño de hardware de Espressif](https://docs.espressif.com/projects/esp-faq/en/latest/hardware-related/hardware-design.html#what-is-the-voltage-tolerance-of-gpios-of-esp-chips): tolerancia GPIO de 3,6 V y adaptación cuando la señal supera ese límite. CA-14 exige contrastar además los componentes concretos.

Fuentes consultadas durante la creación. Los umbrales, condiciones de ensayo y metas del PRD son decisiones de referencia identificadas en los supuestos; no se atribuyen a los fabricantes. La limitación a movimiento radial se deduce de observar una sola distancia escalar.

