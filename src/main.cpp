#include <Arduino.h>
#include "systemConfig.h"

// Comprueba el perfil central antes de cualquier futura inicialización del hardware.
void setup() {
    const profileValidator validator{};
    const validationResult result = validator.validate(defaultConfig);
    if (result.error != configError::none) return;
}

// Mantiene el hardware inactivo hasta implementar la adquisición.
void loop() {}
