# Revisión adversarial de contratos entre componentes

## Resolución comprobada — 2026-09-06

**Veredicto: RC-01 y RC-02 resueltos; ningún hallazgo pendiente de esta revisión.** Se comprobaron únicamente las correcciones solicitadas en AD-7, contratos de §4 y orden de integración de §6, sin ampliar la revisión.

- **RC-01 cerrado:** AD-7 enumera causas de reinicio y excluye el mero cambio entre estados no evaluables. `noValidData → recovering` conserva explícitamente la primera muestra fresca. §6 protege esa muestra frente a un reinicio posterior y mantiene la primera publicación tras diez muestras concordantes del perfil inicial.
- **RC-02 cerrado:** `sampleFresh` se calcula una sola vez y acompaña al resultado hasta procesamiento y salud. El eco válido caducado se consume una vez, reinicia el contador de inválidos y actualiza la marca original solo si es posterior; no entra al filtro, no genera candidata y permanece en fallo sin recuperación transitoria. Quedan definidos tanto el descarte temporal como su efecto sobre salud.

Los hallazgos iniciales se conservan debajo como registro histórico. Este cierre verifica contratos documentales; no afirma ejecución de las pruebas de firmware propuestas.

Fuente revisada: ARCHITECTURE-SPINE.md, decisiones AD-1 a AD-12 y contratos de §4. Se contrastan las reglas C-01/C-03 y CA-07/08 del PRD vinculante. No se modifica el alcance ni se exige implementación de firmware.

## RC-01 — Media: propiedad del reinicio al salir del fallo

**Ubicación:** AD-7, contrato de `systemState::onMeasurement`, `resetSegment` y pasos 3–4 de §6.

**Par incompatible:** el procesador recibe el primer eco válido después de `noValidData`, lo conserva como primera muestra y devuelve `valid`. El gestor de estado cambia a `recovering`; un integrador interpreta literalmente AD-7 («la transición a no evaluable borra segmento y candidata una vez») y llama `resetSegment` al observar esa transición, borrando la muestra ya conservada. Otro integrador interpreta que AD-7 exige borrado únicamente al abandonar el estado operativo o entrar en fallo, y conserva la muestra. Ambos respetan sus reglas locales, pero uno requiere once muestras recibidas para clasificar tras el fallo y otro diez.

La advertencia de §6.3 protege expresamente la ruptura comunicada por el procesador; no define de forma general qué transiciones de estado requieren reinicio después de `onMeasurement`. Además, esa operación no devuelve una acción de reinicio, mientras `checkFreshness` sí devuelve un indicador de transición.

**Corrección:** enumerar transiciones que invalidan segmento y aclarar que `noValidData → recovering` no reinicia después de incorporar la muestra actual. Definir una acción tipada o un orden inequívoco de integración, incluyendo quién ejecuta cada reset. **Prueba:** forzar fallo, entregar exactamente diez muestras válidas concordantes y comprobar publicación en la décima y tamaño del filtro uno tras la primera.

## RC-02 — Media: resultado válido caducado sin contrato de descarte

**Ubicación:** AD-4/7, `processingResult`, `onMeasurement` y §6, párrafo «el resultado se admite únicamente si su propia marca sigue fresca».

**Par incompatible:** el driver cierra y entrega una única vez un eco completo cuya marca ya tiene edad ≥ `staleTimeoutMs`, por un retraso del bucle. El procesador puede devolver `measurementStatus::valid` por duración, rango y continuidad. Un integrador lo entrega a `onMeasurement`, que según AD-7 reinicia inválidos y permite recuperar por dato válido; el segundo chequeo de caducidad devuelve inmediatamente a fallo. Otro integrador lo descarta antes de procesar y mantiene el fallo. El primero puede modificar salud/filtro y reiniciar fases de LED pese a no existir dato fresco; el segundo omite la actualización de salud «exactamente una vez» del intento cerrado. Falta especificar qué actualización representa el descarte sin tratarlo como un eco físicamente inválido.

**Corrección:** fijar en qué capa se comprueba frescura y qué entrada recibe `systemState` para un resultado caducado. Precisar que no entra al filtro, no inicia recuperación, no refresca la última muestra válida y si afecta o no al contador de inválidos. Mantener la distinción entre validez física y utilidad temporal. **Prueba:** cerrar un eco correcto, retener su resultado más de 350 ms y verificar ausencia de transición transitoria a recuperación y de cambios en el historial.

## Cobertura sin hallazgos adicionales

La unidad común de microsegundos, el origen del reloj, el tiempo efectivo de la mediana, la separación de candidata y publicación, las formas de instantánea operativa/no evaluable y la máscara LED están suficientemente definidos. El cálculo de ocho/diez muestras y 1147 ms resulta consistente. No se identificó otro par incompatible que justifique ampliar la revisión a elecciones internas de implementación.
