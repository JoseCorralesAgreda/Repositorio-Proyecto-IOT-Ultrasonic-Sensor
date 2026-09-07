# Revisión editorial del PRD

Este documento ayuda a responsables de firmware, montaje y pruebas a implementar y aceptar un detector de movimiento radial reproducible. Revisión para lectores humanos en español, con Microsoft Writing Style Guide como guía. Modelo: Strategic/Context (Pyramid), con tablas de referencia para configuración y trazabilidad.

Recuento mediante `word_metrics.py`: 4372 palabras. Se preservan el tono técnico directo, los identificadores de trazabilidad, los ejemplos numéricos y la separación entre decisiones propuestas y prestaciones comprobadas. No se evalúa aquí la validez técnica del contenido.

| Pass | Original Text | Revised Text | Changes |
|---|---|---|---|
| structure | §10, «Éxito y cierre de aceptación» (158 palabras), después de la tabla detallada de pruebas. | MOVE la sección junto al propósito, antes de las definiciones; actualizar numeración y referencias. | Los objetivos de éxito aparecen después de parámetros y procedimientos extensos. Presentarlos antes ayuda a comprender qué resultados justifican esos detalles. Impacto: 0 palabras; conservar vínculos a pruebas posteriores. |
| structure | §9, «Criterios de aceptación y método de prueba» (807 palabras), tabla con procedimientos extensos por celda. | CONDENSE la presentación mediante entradas individuales con encabezados CA-01 a CA-14, separando procedimiento y resultado esperado cuando la celda contenga varios ensayos. Conservar íntegro el contenido normativo. | Facilita ejecutar y registrar cada prueba; en particular CA-05 y CA-06 son difíciles de recorrer como una sola celda. Impacto estimado: 0 palabras de reducción; puede requerir algunas etiquetas breves. |
| structure | §5 y §6, tablas de requisitos; §10 explica su función de trazabilidad. | PRESERVE las referencias cruzadas y la explicación de la matriz. | La repetición de IDs permite navegación y comprobación; no es redundancia eliminable. Impacto: 0 palabras. |
| prose | C-02: «Dentro de la banda entre salida y entrada se conserva la clasificación publicada» | «Dentro de la banda entre el umbral de salida del movimiento y el umbral de entrada se conserva la clasificación publicada» | Explicita los referentes: «salida» y «entrada» también se usan para estados y otros umbrales. |
| prose | CA-06: «Salida exacta de 2 produce estacionario tras tres; 2,01 conserva movimiento.» | «Desde movimiento, un módulo de velocidad de 2 cm/s produce estacionario tras tres evaluaciones; 2,01 cm/s conserva movimiento.» | Evita confundir el valor de entrada del ensayo con una salida del sistema; conserva la frontera y el estado previo. |

Resumen: dos recomendaciones de estructura, una preservación explícita y dos ajustes de prosa. Reducción propuesta: 0 palabras (0 % de 4372); la mejora buscada es navegación y claridad. No se proporcionó una extensión objetivo. No se proponen cortes de contenido ni sacrificios de comprensión. El PRD no fue modificado.

## Resolución del autor
Aplicados los dos ajustes de prosa. Se conserva el orden de secciones para mantener configuración, comportamiento y verificación juntos antes del cierre. Se conservan las pruebas en tabla para facilitar comparación y trazabilidad en este documento; un procedimiento de laboratorio posterior puede desglosarlas sin alterar sus criterios.
