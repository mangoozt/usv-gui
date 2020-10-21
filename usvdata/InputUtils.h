#ifndef USV_INPUTUTILS_H
#define USV_INPUTUTILS_H

#include "InputTypes.h"
#include <string>
#include <stdexcept>
#include <filesystem>
#include <sstream>

namespace USV::InputUtils {

InputTypes::InputData loadInputData(const std::filesystem::path & data_directory);

template<typename T>
void load_from_json_file(std::unique_ptr<T>& data, const std::filesystem::path& filename) {
    using namespace spotify::json;

    if (filename.empty()) return;
    std::ifstream ifs(filename);
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    data = std::make_unique<T>();
    if(!try_decode<T>(*data,buffer.str()))
        throw std::runtime_error("Failed to parse "+filename.filename().string());
}

}

#endif //USV_INPUTUTILS_H
