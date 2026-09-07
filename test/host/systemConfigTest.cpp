#include "systemConfig.h"
#include <cmath>
#include <iostream>
#include <limits>

struct testState { unsigned checks{0}; unsigned failures{0}; };
// Record an explicit check even when NDEBUG is enabled.
void expect(testState& state, bool condition, const char* name) {
    ++state.checks;
    if (!condition) { ++state.failures; std::cerr << "FAIL: " << name << '\n'; }
}
// Exercise one field on a fresh production profile.
template<typename valueType, typename inputType>
void field(testState& state, valueType systemConfig::*member, inputType value,
           configError expected, const char* name) {
    systemConfig p{defaultConfig};
    p.*member = static_cast<valueType>(value);
    expect(state, validateProfile(p).error == expected, name);
}
// Cover integer endpoints, adjacent failures, and extreme inputs.
void integers(testState& s) {
    struct rangeCase { std::uint32_t systemConfig::*member; std::uint32_t low;
        std::uint32_t high; configError error; const char* name; };
    const rangeCase cases[]{

        {&systemConfig::sampleToleranceMs,1,10,configError::tolerance,"tolerance"},
        {&systemConfig::echoTimeoutUs,11601,89999,configError::echoTimeout,"timeout"},
        {&systemConfig::analysisLagSamples,3,8,configError::window,"window"},
        {&systemConfig::confirmationCount,2,5,configError::counters,"confirmation"},
        {&systemConfig::invalidLimit,1,5,configError::counters,"invalid limit"},
        {&systemConfig::staleTimeoutMs,111,1000,configError::stale,"stale"}};
    for (const auto& c : cases) {
        field(s,c.member,c.low,configError::none,c.name);
        field(s,c.member,c.high,configError::none,c.name);
        field(s,c.member,c.low-1,c.error,c.name);
        field(s,c.member,c.high+1,c.error,c.name);
        field(s,c.member,std::numeric_limits<std::uint32_t>::max(),c.error,c.name);
    }
}
// Cover finite ranges and nonfinite values for every floating-point field.
void floating(testState& s) {
    struct rangeCase { float systemConfig::*member; float low; float high;
        configError error; const char* name; };
    const rangeCase cases[]{
        {&systemConfig::minDistanceCm,10,199,configError::distance,"min distance"},
        {&systemConfig::maxDistanceCm,11,200,configError::distance,"max distance"},
        {&systemConfig::distanceScale,.9F,1.1F,configError::correction,"scale"},
        {&systemConfig::distanceOffsetCm,-5,5,configError::correction,"offset"},
        {&systemConfig::motionExitCmPerSec,0,3.9F,configError::thresholds,"motion exit"},
        {&systemConfig::motionEnterCmPerSec,2.1F,9.9F,configError::thresholds,"motion entry"},
        {&systemConfig::mediumExitCmPerSec,4.1F,11.9F,configError::thresholds,"medium exit"},
        {&systemConfig::mediumEnterCmPerSec,10.1F,21.9F,configError::thresholds,"medium entry"},
        {&systemConfig::highExitCmPerSec,12.1F,24.9F,configError::thresholds,"high exit"},
        {&systemConfig::highEnterCmPerSec,22.1F,100,configError::thresholds,"high entry"},
        {&systemConfig::lowBlinkHz,.5F,1.9F,configError::frequency,"low Hz"},
        {&systemConfig::mediumBlinkHz,1.1F,3.9F,configError::frequency,"medium Hz"},
        {&systemConfig::highBlinkHz,2.1F,5,configError::frequency,"high Hz"},
        {&systemConfig::statusBlinkHz,.5F,5,configError::frequency,"status Hz"},
        {&systemConfig::errorBlinkHz,.5F,5,configError::frequency,"error Hz"}};
    for (const auto& c : cases) {
        field(s,c.member,c.low,configError::none,c.name);
        field(s,c.member,c.high,configError::none,c.name);
        field(s,c.member,std::numeric_limits<double>::quiet_NaN(),c.error,c.name);
        field(s,c.member,std::numeric_limits<double>::infinity(),c.error,c.name);
        field(s,c.member,-std::numeric_limits<double>::infinity(),c.error,c.name);
    }
    field(s,&systemConfig::minDistanceCm,9.99F,configError::distance,"distance below");
    field(s,&systemConfig::maxDistanceCm,200.01F,configError::distance,"distance above");
    field(s,&systemConfig::minDistanceCm,200.0F,configError::distance,"distance equal");
    field(s,&systemConfig::maxDistanceCm,9.0F,configError::distance,"distance inverted");
    field(s,&systemConfig::distanceScale,.899F,configError::correction,"scale below");
    field(s,&systemConfig::distanceScale,1.101F,configError::correction,"scale above");
    field(s,&systemConfig::distanceOffsetCm,-5.01F,configError::correction,"offset below");
    field(s,&systemConfig::distanceOffsetCm,5.01F,configError::correction,"offset above");
}
// Reject all adjacent threshold equalities and inversions.
void ordering(testState& s) {


    float systemConfig::*members[]{&systemConfig::motionExitCmPerSec,
        &systemConfig::motionEnterCmPerSec,&systemConfig::mediumExitCmPerSec,
        &systemConfig::mediumEnterCmPerSec,&systemConfig::highExitCmPerSec,
        &systemConfig::highEnterCmPerSec};
    for (unsigned i=0;i<5;++i) {
        field(s,members[i],defaultConfig.*members[i+1],configError::thresholds,"threshold equal");
        field(s,members[i],defaultConfig.*members[i+1]+1,configError::thresholds,"threshold inversion");
    }
    field(s,members[0],-0.01F,configError::thresholds,"negative exit");
    float systemConfig::*rates[]{&systemConfig::lowBlinkHz,&systemConfig::mediumBlinkHz,
        &systemConfig::highBlinkHz,&systemConfig::statusBlinkHz,&systemConfig::errorBlinkHz};
    for (auto member : rates) {
        field(s,member,.49F,configError::frequency,"Hz below");
        field(s,member,5.01F,configError::frequency,"Hz above");
    }
    field(s,rates[0],2.0F,configError::frequency,"low medium equal");
    field(s,rates[0],3.0F,configError::frequency,"low medium inversion");
    field(s,rates[1],4.0F,configError::frequency,"medium high equal");
    field(s,rates[1],4.5F,configError::frequency,"medium high inversion");
}
// Exercise pin roles, duplicates, unavailable pins, and input-only GPIOs.
void pins(testState& s) {
    std::int32_t systemConfig::*members[]{&systemConfig::triggerPin,&systemConfig::echoPin,
        &systemConfig::stationaryLedPin,&systemConfig::approachingLedPin,&systemConfig::recedingLedPin};
    const std::int32_t invalid[]{-1,0,2,5,6,7,8,9,10,11,12,15,20,24,28,29,30,31,40,
        std::numeric_limits<std::int32_t>::min(),std::numeric_limits<std::int32_t>::max()};
    for (unsigned i=0;i<5;++i) {
        for (auto pin : invalid) field(s,members[i],pin,configError::gpio,"invalid GPIO");
        for (std::int32_t pin=34;pin<=39;++pin)
            field(s,members[i],pin,i==1?configError::none:configError::gpio,"input-only GPIO");
        for (unsigned j=0;j<5;++j) {
            if (i!=j) field(s,members[i],defaultConfig.*members[j],configError::gpio,"duplicate GPIO");
        }
        field(s,members[i],std::int32_t{18},configError::none,"valid alternative GPIO");
    }
}
// Check baseline, alternative, fractional and response-limit behavior.
void profiles(testState& s) {
    expect(s,validateProfile(defaultConfig).error==configError::none,"default valid");
    expect(s,validateProfile(defaultConfig).responseBoundMs==1147.0F,"default bound");
    systemConfig p{defaultConfig};
    p.motionEnterCmPerSec=5; p.motionExitCmPerSec=3;
    expect(s,validateProfile(p).error==configError::none,"alternative valid");
    p.echoTimeoutUs=25001;
    expect(s,std::abs(validateProfile(p).responseBoundMs-1147.001)<1e-9,"fraction preserved");
    p=defaultConfig; p.samplePeriodMs=200; p.sampleToleranceMs=20; p.analysisLagSamples=8; p.confirmationCount=5;
    expect(s,validateProfile(p).error==configError::responseBound,"excess bound");
    p=defaultConfig; p.samplePeriodMs=190; p.sampleToleranceMs=5; p.echoTimeoutUs=28000;
    expect(s,validateProfile(p).responseBoundMs==2000 && validateProfile(p).error==configError::none,"bound equality");
    p.echoTimeoutUs=28001;
    expect(s,validateProfile(p).error==configError::responseBound,"bound fractional excess");
    field(s,&systemConfig::samplePeriodMs,std::uint32_t{79},configError::period,"period below");
    field(s,&systemConfig::samplePeriodMs,std::uint32_t{201},configError::period,"period above");
    field(s,&systemConfig::samplePeriodMs,std::numeric_limits<std::uint32_t>::max(),configError::period,"period extreme");
    p=defaultConfig; p.samplePeriodMs=200; p.sampleToleranceMs=20; p.analysisLagSamples=3; p.confirmationCount=2;
    expect(s,validateProfile(p).error==configError::none,"period upper");
    p=defaultConfig; p.samplePeriodMs=80; p.sampleToleranceMs=8;
    expect(s,validateProfile(p).error==configError::none,"period lower tolerance boundary");
    p.sampleToleranceMs=9;
    expect(s,validateProfile(p).error==configError::tolerance,"period relative tolerance");
}
// Run all families and propagate failure to the host process.
int main() {
    testState state{};
    integers(state); floating(state); ordering(state); pins(state); profiles(state);
    std::cout << state.checks << " checks, " << state.failures << " failures\n";
    return state.failures==0 ? 0 : 1;
}




