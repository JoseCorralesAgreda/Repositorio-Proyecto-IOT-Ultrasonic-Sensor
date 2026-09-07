#include "systemConfig.h"
#include <cmath>
const systemConfig defaultConfig{};
// Comprueba una cantidad finita dentro de límites inclusivos.
bool profileValidator::inRange(double value, double minimum, double maximum) const {
    return std::isfinite(value) && value >= minimum && value <= maximum;
}
// Comprueba tiempo y distancia antes de efectuar cálculos derivados.
configError profileValidator::validateMeasurement(const systemConfig& p) const {
    if (!inRange(p.samplePeriodMs, 80, 200)) return configError::period;
    if (p.sampleToleranceMs == 0 || p.sampleToleranceMs > p.samplePeriodMs * 0.1)
        return configError::tolerance;
    if (!inRange(p.minDistanceCm, 10, 200) ||
        !inRange(p.maxDistanceCm, 10, 200) || p.minDistanceCm >= p.maxDistanceCm)
        return configError::distance;
    if (!inRange(p.distanceScale, 0.9F, 1.1F) ||
        !inRange(p.distanceOffsetCm, -5, 5)) return configError::correction;
    if (p.echoTimeoutUs <= p.maxDistanceCm * echoUsPerCm ||
        p.echoTimeoutUs >= (p.samplePeriodMs - p.sampleToleranceMs) * microsecondsPerMs)
        return configError::echoTimeout;
    return configError::none;
}
// Comprueba que los umbrales de histéresis sean finitos y ordenados.
bool profileValidator::validThresholds(const systemConfig& p) const {
    const double values[]{p.motionExitCmPerSec, p.motionEnterCmPerSec,
        p.mediumExitCmPerSec, p.mediumEnterCmPerSec, p.highExitCmPerSec, p.highEnterCmPerSec};
    for (double value : values) {
        if (!std::isfinite(value)) return false;
    }
    return values[0] >= 0 && values[0] < values[1] && values[1] < values[2] &&
        values[2] < values[3] && values[3] < values[4] && values[4] < values[5];
}
// Comprueba ventana, contadores, caducidad y frecuencias de indicación.
configError profileValidator::validateBehavior(const systemConfig& p) const {
    if (!inRange(p.analysisLagSamples, 3, 8)) return configError::window;
    if (!inRange(p.confirmationCount, 2, 5) || !inRange(p.invalidLimit, 1, 5))
        return configError::counters;
    if (!validThresholds(p)) return configError::thresholds;
    if (p.staleTimeoutMs <= p.samplePeriodMs + p.sampleToleranceMs || p.staleTimeoutMs > 1000)
        return configError::stale;
    if (!inRange(p.lowBlinkHz, 0.5, 5) || !inRange(p.mediumBlinkHz, 0.5, 5) ||
        !inRange(p.highBlinkHz, 0.5, 5) || !inRange(p.statusBlinkHz, 0.5, 5) ||
        !inRange(p.errorBlinkHz, 0.5, 5) || p.lowBlinkHz >= p.mediumBlinkHz || p.mediumBlinkHz >= p.highBlinkHz)
        return configError::frequency;
    return configError::none;
}
// Aplica la política conservadora de pines expuestos por la placa para el rol.
bool profileValidator::validPin(std::int32_t pin, bool output) const {
    const bool exists = (pin >= 0 && pin <= 19) || (pin >= 21 && pin <= 23) ||
        (pin >= 25 && pin <= 27) || (pin >= 32 && pin <= 36) || pin == 39;
    const bool reserved = (pin >= 6 && pin <= 11) || pin == 0 || pin == 2 ||
        pin == 5 || pin == 12 || pin == 15;
    return exists && !reserved && (!output || pin < 34);
}
// Exige pines distintos con capacidad de salida salvo para ECHO.
bool profileValidator::validPins(const systemConfig& p) const {
    const std::int32_t pins[]{p.triggerPin, p.echoPin, p.stationaryLedPin,
        p.approachingLedPin, p.recedingLedPin};
    constexpr unsigned pinCount = sizeof(pins) / sizeof(pins[0]);
    constexpr unsigned echoIndex = 1;
    for (unsigned i = 0; i < pinCount; ++i) {
        if (!validPin(pins[i], i != echoIndex)) return false;
        for (unsigned j = 0; j < i; ++j) {
            if (pins[i] == pins[j]) return false;
        }
    }
    return true;
}
// Devuelve el primer error tipado y calcula la cota después de comprobar los rangos.
validationResult profileValidator::validate(const systemConfig& p) const {
    const configError measurementError = validateMeasurement(p);
    if (measurementError != configError::none) return {measurementError, 0.0};
    const configError behaviorError = validateBehavior(p);
    if (behaviorError != configError::none) return {behaviorError, 0.0};
    if (!validPins(p)) return {configError::gpio, 0.0};
    const double boundMs = (p.analysisLagSamples + p.confirmationCount + responseExtraPeriods) *
        (p.samplePeriodMs + p.sampleToleranceMs) + p.echoTimeoutUs / microsecondsPerMs +
        processingBudgetMs + outputBudgetMs;
    return {boundMs > maximumResponseMs ? configError::responseBound : configError::none,
        boundMs};
}
