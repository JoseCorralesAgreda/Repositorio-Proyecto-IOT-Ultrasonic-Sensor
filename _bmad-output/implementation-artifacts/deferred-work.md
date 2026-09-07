- source_spec: `spec-1-1-validar-perfil.md`
  summary: Contrastar la cobertura de timeout con la transformación inversa de calibración antes de integrar perfiles calibrados; impacto medio sin verificar.
  evidence: BH-3; la historia 1.1 exige timeout mayor que maxDistanceCm × 58 y el código lo cumple. Con escala 0,9 y offset −5, una futura medición corregida de 200 cm podría requerir aproximadamente 13211 µs. Resolver contra contratos y pruebas del procesador/adquisición; todavía no hay consumidor físico implementado que permita verificar el efecto.

- source_spec: `spec-1-1-validar-perfil.md`
  summary: Precisar en el contexto de épica que analysisLagSamples=5 significa separación por cinco posiciones y requiere seis salidas filtradas almacenadas.
  evidence: BH-9; epic-1-context.md usa la frase abreviada «ventana de cinco salidas filtradas». La especificación y epics.md contienen la relación correcta; el contexto fue creado antes de esta continuación y debe aclararse antes de implementar 1.3.
