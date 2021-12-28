#include "sea_appearance.h"

namespace
{
const Material default_material = {
    {0, 0.0388058, 0.123756},           // sea ambient
    {0.281572, 0.442459, 0.850248},     // sea diffuse
    {0, 0, 0},                          // sea specular
    256,                                // sea shininess
};
}

SeaAppearanceNotifier::SeaAppearanceNotifier() 
    : StateNotifier<SeaAppearance>({default_material})
{}

void SeaAppearanceNotifier::updateMaterial(const Material& material) {
    value.material = material;
    notify();
}