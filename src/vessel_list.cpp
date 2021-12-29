#include "vessel_list.h"

VesselListNotifier::VesselListNotifier() 
    : StateNotifier<std::vector<Vessel>>({}) 
{}

void VesselListNotifier::updateVessels(const std::vector<Vessel> &vessels) {
    value = vessels;
    notify();
}