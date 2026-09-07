# Epic 1 Context: Observar el movimiento radial y reconocer cuándo la lectura no es fiable

<!-- Compiled from planning artifacts. Edit freely. Regenerate with compile-epic-context if planning docs change. -->

## Goal

Entregar el recorrido completo desde medición ultrasónica hasta indicación de dirección e intensidad mediante tres LEDs, distinguiendo reposo de información no evaluable y recuperándose con evidencia nueva. Incluye configuración validada, montaje comprobado y ensayos físicos del perfil inicial. La calibración completa y la estabilidad durante una hora pertenecen a la épica 2; completar esta épica no implica aceptación final del producto.

## Stories

- Story 1.1: Comprobar el perfil de medición antes de utilizarlo
- Story 1.2: Obtener distancias filtradas sin ocultar errores de medición
- Story 1.3: Estimar movimiento radial con tiempo real e histéresis
- Story 1.4: Publicar una clasificación confirmada y retirar datos obsoletos
- Story 1.5: Reconocer todos los estados mediante patrones de LEDs
- Story 1.6: Preparar y comprobar el montaje antes de utilizar el sensor
- Story 1.7: Capturar ecos periódicos sin detener la atención del sistema
- Story 1.8: Observar la clasificación completa y recuperarse automáticamente
- Story 1.9: Verificar respuesta e indicadores sobre el montaje real

## Requirements & Constraints

Prototipo educativo con ESP32 DOIT DevKit V1, HC-SR04 y tres LEDs, sin red, servicios, almacenamiento ni interfaces adicionales. Solo detecta componente radial. Ensayos en interior a 20–25 °C, sensor fijo, alimentación estable y reflector plano de al menos 20 × 20 cm, perpendicular al eje, entre 10 y 200 cm y sin otros emisores cercanos.

Medir como máximo una vez por período, sin ráfagas; distinguir eco ausente, incompleto y fuera de plazo. Corregir distancia antes de validar finitud y rango inclusivo. Mediana móvil de tres válidas consecutivas; cualquier inválido rompe filtro, historial y confirmaciones, sin valores sustitutos. Una discontinuidad también rompe el segmento, pero puede conservar la muestra válida actual como primera del nuevo.

Calcular velocidad con tiempos reales. Dirección por signo; intensidad e histéresis usan la clasificación publicada. Confirmar la pareja candidata completa; las inversiones pueden publicarse directamente. Retirar dirección e intensidad ante indisponibilidad, nunca representar falta de eco como reposo. Evaluar caducidad aunque no se cierre una adquisición y reconstruir historial antes de recuperar clasificación.

Perfil inicial: período 100 ±10 ms, timeout de eco 25 ms, ventana de cinco salidas filtradas, tres confirmaciones, tres inválidos y caducidad 350 ms. Entradas/salidas de movimiento 4/2 cm/s, intensidad media 12/10 y alta 25/22. Ocho muestras permiten evaluar y diez publicar. Configuración central editable antes de compilar, inmutable hasta reiniciar; rechazar valores no finitos, relaciones inválidas, GPIO repetidos/no admitidos, desbordamientos y latencia calculada superior a 2 s.

Con perfil inicial: arranque ≤1,2 s, respuesta a cambios concordantes ≤1,5 s y actualización LED ≤20 ms; espera individual ≤27 ms. Exigir cero falsas transiciones en pruebas estacionarias y al menos 9/10 recorridos correctos por combinación de dirección e intensidad a 6/16/30 cm/s. La simulación no sustituye los ensayos físicos.

RAM propia ≤8 KiB, imagen ≤80 % de la partición de aplicación, almacenamiento fijo y sin asignación dinámica por muestra. Funciones ≤60 líneas no vacías y tres niveles de anidamiento, responsabilidad única documentada, identificadores propios camelCase y compilación sin advertencias propias con `-Wall -Wextra`.

## Technical Decisions

Conservar PlatformIO/Arduino, fijar `espressif32@7.1.1` y C++17. Núcleo comprobable en host mediante C++ estándar; solo adaptadores incluyen Arduino/ESP-IDF. Flujo secuencial: adquisición → validación/filtro → análisis → candidata → confirmación/estado → LEDs. Un único propietario publica estado coherente; salidas reciben esa instantánea, nunca distancia o velocidad.

Bucle cooperativo sin tareas RTOS, colas ni callbacks de temporizadores propios. Captura asíncrona con ISR GPIO y contexto explícito, registro fijo de capacidad uno y acceso atómico compartido; la ISR solo captura flancos y tiempos. El pulso TRIG de 10 µs es la única espera activa prevista.

Tiempo común en microsegundos monotónicos `std::int64_t`, obtenido de `esp_timer_get_time()`. Fechar adquisición al iniciar TRIG; continuidad, velocidad y caducidad usan esa marca. Mediana fechada con la muestra cronológica central. Conversión mediante `echoUsPerCm = 58.0F`; distancias/velocidades `float`, resultados tipados y ausencia explícita con `std::optional`. Historial temporal circular de hasta nueve salidas; no agregar suavizados ni límites arbitrarios de salto.

Actualizar salud una vez por intento. Validación física y frescura son distintas: un válido caducado actualiza salud con su marca original, pero no entra al filtro ni inicia recuperación. La transición a recuperación no debe borrar de nuevo la primera muestra fresca. Comprobar caducidad antes de consumir resultados y nuevamente al terminar la iteración.

Validar antes de habilitar TRIG. Un mapa independiente `safeBoardPins`, previamente verificado contra el cableado, permite indicar error sin escribir ni armar interrupciones en pines solicitados rechazados. Comprobar adaptación de ECHO, alimentación, masa común y resistencias LED antes de operar el montaje; ECHO de 5 V nunca va directamente al GPIO.

## UX & Interaction Patterns

Identificar LEDs por rol/posición: estacionario fijo para reposo; acercándose o alejándose parpadea a 1/2/4 Hz según intensidad. Inicialización y recuperación comparten parpadeo del LED estacionario; sin datos parpadean los tres sincronizados; configuración inválida enciende los tres fijos. Ciclo 50 %, nueva fase encendida al cambiar estado/intensidad, LEDs ajenos apagados y cálculo temporal sin bloquear adquisición.

## Cross-Story Dependencies

La 1.1 habilita el núcleo; 1.2 → 1.3 → 1.4 desarrollan procesamiento, clasificación y estado. La 1.5 requiere 1.1/1.4; 1.6 requiere 1.1/1.5; captura 1.7 requiere 1.1/1.2 y montaje validado 1.6. La integración 1.8 depende de 1.1–1.7 y la campaña física 1.9 depende de 1.8. Las pruebas puras no dependen de hardware futuro. La épica 2 parte de la evidencia física de 1.9.
