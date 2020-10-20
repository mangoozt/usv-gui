#include "InputUtils.h"
#include <string>
#include <stdexcept>
#include <filesystem>

namespace USV {
    namespace InputUtils {

        void fillFromJson(const char* fieldName, std::string& field, const rapidjson::Value& doc) {
            if (!doc.HasMember(fieldName)) {
                throw std::runtime_error(strErrMissingField + std::string(fieldName));
            }
            if (doc.GetObject()[fieldName].IsString())
                field = doc.GetObject()[fieldName].GetString();
            else
                throw std::runtime_error(strErrWrongFieldType + std::string(fieldName));
        }

        void fillFromJsonVolountary(const char* fieldName, std::string& field, const rapidjson::Value& doc) {
            if (doc.HasMember(fieldName)) {
                if (doc.GetObject()[fieldName].IsString())
                    field = doc.GetObject()[fieldName].GetString();
                else
                    throw std::runtime_error(strErrWrongFieldType + std::string(fieldName));
            }
        }

        InputData loadInputData(const std::filesystem::path & data_directory){
            namespace fs = std::filesystem;
            InputData inputData;
            load_from_json_file(inputData.navigationParameters, fs::path(data_directory)/"nav-data.json");
            load_from_json_file(inputData.navigationProblem, fs::path(data_directory)/"target-data.json");
            load_from_json_file(inputData.featureCollection, fs::path(data_directory)/"constraints.json");
            load_from_json_file(inputData.hydrometeorology, fs::path(data_directory)/"hmi-data.json");
            load_from_json_file(inputData.route, fs::path(data_directory)/"route-data.json");
            load_from_json_file(inputData.settings, fs::path(data_directory)/"settings.json");
            return inputData;
        }

    }
}
