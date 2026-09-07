#pragma once
#include <cstdint>
inline constexpr std::uint32_t medianFilterSize = 3;
inline constexpr std::uint32_t sampleCapacity = 9;
inline constexpr std::uint32_t triggerPulseUs = 10;
inline constexpr double echoUsPerCm = 58.0;
inline constexpr double microsecondsPerMs = 1000.0;
inline constexpr double processingBudgetMs = 2.0;
inline constexpr double outputBudgetMs = 20.0;
inline constexpr double maximumResponseMs = 2000.0;
inline constexpr std::uint32_t responseExtraPeriods = 2;
struct systemConfig {
    std::uint32_t samplePeriodMs{100};
    std::uint32_t sampleToleranceMs{10};
    std::uint32_t echoTimeoutUs{25000};
    float minDistanceCm{10.0F};
    float maxDistanceCm{200.0F};
    float distanceScale{1.0F};
    float distanceOffsetCm{0.0F};
    std::uint32_t analysisLagSamples{5};
    float motionEnterCmPerSec{4.0F};
    float motionExitCmPerSec{2.0F};
    float mediumEnterCmPerSec{12.0F};
    float mediumExitCmPerSec{10.0F};
    float highEnterCmPerSec{25.0F};
    float highExitCmPerSec{22.0F};
    std::uint32_t confirmationCount{3};
    std::uint32_t invalidLimit{3};
    std::uint32_t staleTimeoutMs{350};
    float lowBlinkHz{1.0F};
    float mediumBlinkHz{2.0F};
    float highBlinkHz{4.0F};
    float statusBlinkHz{1.0F};
    float errorBlinkHz{2.0F};
    std::int32_t triggerPin{25};
    std::int32_t echoPin{26};
    std::int32_t stationaryLedPin{27};
    std::int32_t approachingLedPin{32};
    std::int32_t recedingLedPin{33};
    bool activeHigh{true};
};
enum class configError {
    none, period, tolerance, distance, correction, echoTimeout, window,
    counters, thresholds, stale, frequency, gpio, responseBound
};
struct validationResult {
    configError error{configError::none};
    double responseBoundMs{0.0};
};
extern const systemConfig defaultConfig;
// Comprueba el perfil sin hardware, asignaciones ni estado mutable.
class profileValidator {
public:
    validationResult validate(const systemConfig& profile) const;

private:
    bool inRange(double value, double minimum, double maximum) const;
    configError validateMeasurement(const systemConfig& profile) const;
    bool validThresholds(const systemConfig& profile) const;
    configError validateBehavior(const systemConfig& profile) const;
    bool validPin(std::int32_t pin, bool output) const;
    bool validPins(const systemConfig& profile) const;
};
