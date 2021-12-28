#ifndef SEA_APPEARANCE
#define SEA_APPEARANCE

#include "Defines.h"
#include "state_notifier.h"

struct SeaAppearance 
{
    Material material;
};

class SeaAppearanceNotifier : public StateNotifier<SeaAppearance> 
{
public:
    SeaAppearanceNotifier();
    void updateMaterial(const Material& material);
};

#endif // SEA_APPEARANCE