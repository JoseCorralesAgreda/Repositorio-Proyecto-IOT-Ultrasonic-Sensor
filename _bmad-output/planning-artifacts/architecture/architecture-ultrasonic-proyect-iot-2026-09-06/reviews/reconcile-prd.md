# Reconciliación con el PRD y la solicitud

Fecha: 2026-09-06. Revisión documental independiente; no se ejecutaron pruebas de firmware ni de hardware.

Fuentes comparadas: PRD final `../../../prds/prd-ultrasonic-proyect-iot-2026-09-06/prd.md`, solicitud de arquitectura en español y `../ARCHITECTURE-SPINE.md`.

## Resultado

No se identifican requisitos de producto omitidos, nuevos ni contradichos. La arquitectura mantiene al PRD como fuente vinculante y distingue decisiones de implementación de requisitos del producto. No hay hallazgos bloqueantes de reconciliación.

| Cobertura | Evidencia en arquitectura | Resultado |
|---|---|---|
| RF-01 a RF-03: adquisición, invalidez, mediana de tres | AD-3 a AD-5, contratos y §§6–7 | Preserva muestras consecutivas, interrupción del segmento, timeout y ausencia de sustituciones. |
| RF-04 a RF-06: tiempo real, dirección, intensidad | AD-4/6 y §7 | Conserva signo, umbrales inclusivos, referencia publicada, histéresis y confirmación de pareja completa. |
| RF-07/08: estados, recuperación y LEDs | AD-7/8 y §§6–8 | Conserva estados no evaluables, prioridad, reconstrucción y patrones C-04; LEDs no reciben distancias. |
| RF-09/10: configuración y calibración | AD-9/12 y §9 | Remite a lista exhaustiva de parámetros del PRD y mantiene configuración compilada y validación física pendiente. |
| RNF-01/02: temporización | AD-3/4 y §6 | Conserva presupuestos de producto y ensayos CA-01/05/07/08/09; cota calculada no se presenta como evidencia física. |
| RNF-03/04: recursos | AD-11 y §11 | Buffers fijos, ≤8 KiB, ≤80 % de partición y ensayo de memoria/estabilidad conservados. |
| RNF-05 a RNF-08: C++, estilo, separación y dependencias | AD-1/11/12 y §§10–12 | camelCase, funciones/acoplamiento acotados, núcleo comprobable sin GPIO y entorno existente. |
| RT-01: hardware y electricidad | AD-10 y §5 | Únicamente hardware funcional solicitado; pasivos y adaptación autorizados por el PRD, sujetos a CA-14. |
| Trazabilidad y aceptación | §12 | Los 19 requisitos tienen responsable/decisión y CA asociados; CA-01 a CA-14 permanecen vinculantes. |
| Solicitud específica de arquitectura | §§2–15 | Responsabilidades, interfaces, datos, errores, calibración, muestreo, testing, recursos, decisiones y riesgos cubiertos en español. |

## Decisiones nuevas de ingeniería que no amplían el producto

La ISR breve con un registro fijo concreta el cumplimiento temporal sin tareas propias ni colas. C++17, tiempos de 64 bits, almacenamiento fijo, mapa GPIO propuesto, divisor ECHO y mapa seguro de arranque concretan implementación y montaje. No incorporan nuevas funciones observables al producto. El presupuesto interno de servicio de 1 ms se identifica expresamente como margen de diseño.

## Condiciones de cierre

El documento revisado conserva `status: draft`; el workflow debe actualizarlo al completar su finalización. No es una contradicción funcional ni evidencia de aceptación pendiente del documento por parte del usuario. La validación del montaje, las pruebas y la calibración siguen pendientes de implementación, tal como exige el PRD.
