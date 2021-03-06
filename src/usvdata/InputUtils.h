#ifndef USV_INPUTUTILS_H
#define USV_INPUTUTILS_H

#include "InputTypes.h"
#include <string>


namespace USV::InputUtils {

    InputTypes::InputData loadInputData(const std::string& data_directory);

}

#endif //USV_INPUTUTILS_H
