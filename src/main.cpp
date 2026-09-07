#include <Arduino.h>
#include "systemConfig.h"
// Check the central profile before any future hardware initialization.
void setup() {
    const validationResult result = validateProfile(defaultConfig);
    if (result.error != configError::none) return;
}
// Leave hardware inactive until the acquisition story is implemented.
void loop() {}

