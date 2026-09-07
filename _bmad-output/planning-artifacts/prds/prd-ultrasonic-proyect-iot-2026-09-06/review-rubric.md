# Revisión de calidad del PRD — Sistema inteligente de detección de movimiento ultrasónico

## Veredicto general

El PRD define una capacidad embebida coherente, con comportamiento determinista, tratamiento de errores y trazabilidad suficientes para iniciar diseño e implementación. Antes de cerrar la aceptación conviene resolver la relación entre pruebas del perfil inicial y calibrado, y acotar la promesa de latencia frente a candidatos variables. No se encontraron errores aritméticos en las ocho muestras para primera evaluación ni en las diez para primera publicación.

## Preparación para decidir — sólida

§1 delimita el prototipo y §2 expresa claramente las limitaciones del movimiento radial y reflector dominante. §11 asigna las decisiones eléctricas pendientes y distingue sus bloqueos del diseño lógico. El coste de sensibilidad, ruido y latencia está expresado en §10 y no se promete identificar objetos.

## Sustancia sobre formalismo — sólida

Los límites de memoria, tiempos de espera, clasificación e intensidad corresponden al hardware y a salidas observables. No hay personajes, innovación ni conectividad añadidos para llenar una plantilla. El entorno existente está reconocido, y los valores propuestos no se presentan como rendimiento demostrado.

## Coherencia estratégica — sólida

La tesis de variación temporal está presente en el propósito, fórmula, criterios CA-04/05 y métricas. ME-02 y ME-03 contrapesan sensibilidad y filtrado; el alcance completo sirve a la cadena medición–LEDs.

## Claridad de finalización — adecuada

Los RF tienen consecuencias comprobables y referencias de aceptación. La secuencia de inicialización es matemáticamente correcta: tres muestras producen la primera mediana, ocho producen seis medianas separadas por cinco posiciones, y diez permiten tres evaluaciones concordantes. Se precisan los siguientes ajustes de aceptación.

### Hallazgos

- **Media — Perfil calibrado frente a fronteras fijas** (§3, §8, CA-06, CA-10 y §10): se admite cambiar umbrales y se exige aprobar todos los CA aplicables al perfil final, pero CA-06 exige movimiento a 4 cm/s y fronteras 12/25/10/22. Un perfil explícitamente permitido de entrada/salida 5/3 no cumple la primera condición aunque su comportamiento sea correcto. *Corrección:* identificar CA-06 como prueba numérica del perfil inicial y exigir una versión paramétrica equivalente con los umbrales del perfil final; aplicar la misma separación a tiempos y recuentos que dependan de configuración.
- **Media — Garantía de respuesta sin definir el estímulo sostenido** (RNF-02, C-02.5, CA-08): «inicio, detención o inversión sostenida» no especifica velocidad constante, separación de umbrales ni intensidad estable. Movimiento continuo puede alternar candidatas de intensidad e impedir tres evaluaciones concordantes, por lo que no existe una cota universal de 1,5 s para todo movimiento sostenido. *Corrección:* vincular esa latencia a cambios escalón entre reposo y rampas de referencia constantes, o definir márgenes y permanencia de la candidata; enumerar las entradas de CA-08. Distinguir además el cumplimiento determinista de lógica de la tasa física de 9/10 de CA-05.

## Honestidad de alcance — sólida

§1 excluye funciones no justificadas, §7 documenta límites físicos y §11 indexa los cuatro supuestos utilizados. Los elementos eléctricos necesarios se justifican como parte del montaje y no como nuevas funcionalidades. La aceptación física está pendiente y no se confunde con finalizar este documento.

## Utilidad posterior — adecuada

Glosario, C-01 a C-04, RF-01 a RF-10, RNF-01 a RNF-08 y CA-01 a CA-14 permiten extraer historias y pruebas. Las filas de trazabilidad mantienen referencias explícitas. Los dos hallazgos de aceptación deben resolverse antes de derivar pruebas normativas del perfil calibrado.

## Adecuación de estructura — sólida

La forma de especificación de capacidad es apropiada para un prototipo de un operador. Las tablas de estados, umbrales, patrones LED y aceptación aportan información concreta; no hacen falta recorridos de múltiples personajes ni secciones comerciales.

## Notas mecánicas

- No se observan saltos ni duplicados en las series de identificadores.
- Los cuatro supuestos tienen aparición e índice correspondientes. La etiqueta española SUPUESTO es apropiada para la petición de español.
- No existe addendum.md en la carpeta revisada.
- El frontmatter conserva status: draft; debe reflejar el estado final cuando se integren las revisiones y se cierre el workflow.

## Resolución del autor
Ambos hallazgos medios resueltos: §9 parametriza pruebas tras calibración y limita tiempos finales a 2 s; RNF-02 y CA-08 delimitan los estímulos de referencia con velocidades y duración. Documento finalizado; esto no certifica ensayos físicos.
