#ifndef USV_INPUTUTILS_H
#define USV_INPUTUTILS_H

#include "InputTypes.h"
#include <string>
#include <stdexcept>
#include <sstream>

namespace USV::InputUtils {

InputTypes::InputData loadInputData(const std::string & data_directory);

template<typename T>
void load_from_json_file(std::unique_ptr<T>& data, const std::string& filename) {
    using namespace spotify::json;

    if (filename.empty()) return;
    std::ifstream ifs(filename);
    if(!ifs.good()) return;
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    data = std::make_unique<T>();
    if(!try_decode<T>(*data,buffer.str()))
        throw std::runtime_error("Failed to parse "+filename);
}

}

#endif //USV_INPUTUTILS_H
