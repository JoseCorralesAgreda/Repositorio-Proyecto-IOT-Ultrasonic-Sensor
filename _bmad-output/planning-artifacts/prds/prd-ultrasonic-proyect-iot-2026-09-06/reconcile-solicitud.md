# Reconciliación del PRD con la solicitud original

Fecha: 2026-09-06. Documento examinado: `prd.md`. Alcance de esta comprobación: cobertura de la petición; no certifica implementación ni resultados físicos.

## Cobertura

| Petición | Evidencia en el PRD | Resultado |
|---|---|---|
| PRD completo en español, workflow create | Metadatos `language: es`, `workflow: bmad-prd`, `mode: create`; secciones 1–12 | Cubierto; cierre documental pendiente según nota siguiente |
| ESP32, HC-SR04 y tres LEDs | §1, RT-01, CA-14 | Cubierto |
| Movimiento por variación temporal y no por distancia absoluta | §1, definiciones §2, fórmula C-02, RF-04, CA-04/05 | Cubierto explícitamente |
| Estacionario, acercamiento y alejamiento | C-02/03, RF-05, CA-04/05/06/08 | Cubierto |
| Intensidad según magnitud de variación | Velocidad radial estimada en §2, umbrales §3, C-02, RF-06, CA-05/06 | Cubierto; la normalización por tiempo hace comparable la variación |
| Flujo medición, filtrado, análisis temporal, detección, clasificación, estado y LEDs | C-01 a C-04, RF-01 a RF-08 | Cubierto |
| Requisitos funcionales y no funcionales | §5 y §6 | Cubierto |
| Comportamiento, restricciones y supuestos | §3/4/6/7/11 | Cubierto; inferencias identificadas como S-01 a S-04 |
| Criterios medibles, observables y verificables | CA-01 a CA-14, métricas ME-01 a ME-04 | Cubierto mediante pruebas de lógica, mediciones físicas e inspección |
| Errores y mediciones inválidas | C-01/03, RF-02/07, CA-02/07/08 | Cubierto; incluye caducidad, reinicio del historial y recuperación |
| Umbrales y tolerancias configurables | §3, RF-09, CA-10 | Cubierto; edición previa a compilación y aplicación al reiniciar |
| Calibración | §8, RF-10, CA-11, S-02/04 | Cubierto con resultados cuantitativos y repetición de pruebas |
| Trazabilidad requisitos, comportamiento y aceptación | Matrices §5/6 y explicación §10 | Cubierto |
| C++, camelCase y convenciones modernas | RNF-05/07, CA-13 | Cubierto; excepciones limitadas a nombres impuestos por plataforma |
| Simplicidad, legibilidad, mantenibilidad; evitar los code smells enumerados | RNF-06/07, CA-13 | Cubierto mediante reglas de constantes, duplicación, código muerto, estado global, tamaño, anidamiento y responsabilidades |
| SRP y bajo acoplamiento cuando corresponda | RNF-06/07, prueba lógica sin GPIO en CA-13 | Cubierto |
| Evitar dependencias, servicios, ML, apps y hardware innecesarios | Exclusiones §1, RNF-08, RT-01 | Cubierto; componentes eléctricos necesarios e instrumentos de prueba están justificados |
| Adecuación a recursos del ESP32 | RNF-03/04/07, CA-12/13 | Cubierto con límites de RAM, firmware y memoria retenida |

## Vacíos y cierre

No se detectan funcionalidades solicitadas sin cobertura ni ampliaciones funcionales injustificadas. La elección de frecuencias LED, condiciones físicas y umbrales queda declarada como perfil de referencia calibrable, sin atribuirla al usuario ni presentarla como validada en hardware.

En la versión leída, el metadato `status: draft` todavía no refleja la petición de entregar el documento final. Antes de cerrar el workflow corresponde actualizar el estado documental según su convención y mantener clara la distinción entre PRD finalizado y producto aceptado. No se requiere afirmar que el firmware o el montaje ya superaron las pruebas.

La variante exacta del módulo, GPIO, componentes eléctricos y calibración quedan pendientes de ejecución con responsable, momento y evidencia definidos en §11. No constituyen vacíos de alcance del PRD.
