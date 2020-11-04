#ifndef USV_INPUTUTILS_H
#define USV_INPUTUTILS_H

#include "InputTypes.h"
#include <string>
#include <stdexcept>
#include <filesystem>
#include <sstream>

namespace USV::InputUtils {

namespace fs=std::filesystem;

InputTypes::InputData loadInputData(const fs::path & data_directory);

template<typename T>
void load_from_json_file(std::unique_ptr<T>& data, const fs::path& filename) {
    using namespace spotify::json;

    if (filename.empty()) return;
    std::ifstream ifs(filename);
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    data = std::make_unique<T>();
    *data = decode<T>(buffer.str());
//    if(!try_decode<T>(*data,buffer.str()))
//        throw std::runtime_error("Failed to parse "+filename.filename().string());
}

}

#endif //USV_INPUTUTILS_H
