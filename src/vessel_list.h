#ifndef VESSEL_LIST_H
#define VESSEL_LIST_H

#include "glvessels.h"
#include "state_notifier.h"
#include <vector>

class VesselListNotifier : public StateNotifier<std::vector<Vessel>> {
public:
    VesselListNotifier();
    void updateVessels(const std::vector<Vessel>& vessels);
};

#endif // VESSEL_LIST_H