#ifndef USV_INPUTUTILS_H
#define USV_INPUTUTILS_H

#include <string>
#include <stdexcept>
#include <filesystem>
#include "include/rapidjson/document.h"
#include "InputTypes.h"

namespace USV {
    namespace InputUtils {

        typedef std::string strType;

        const strType strErrDocNotAnObject{"Document should be an object"};
        const strType strErrDocNotAnArray{"Document should be an array"};
        const strType strErrMissingField{"Missing field "};
        const strType strErrWrongFieldType{"Wrong type of field "};

        const strType sColon{": "};
        const strType sParenthesisL{"("};
        const strType sParenthesisR{")"};
        const strType sParenthesisRComma{"),"};

        // Helping function for filling fields from rapidjson::Document
        template<typename T>
        void fillFromJson(const char* fieldName, T& field, const rapidjson::Value& doc) {
            if (!doc.HasMember(fieldName)) {
                throw std::runtime_error(strErrMissingField + std::string(fieldName));
            }
            if (doc.GetObject()[fieldName].Is<T>() ||
                (doc.GetObject()[fieldName].IsNumber() && std::is_floating_point<T>::value))
                field = doc.GetObject()[fieldName].Get<T>();
            else
                throw std::runtime_error(strErrWrongFieldType + std::string(fieldName));
        }

        void fillFromJson(const char* fieldName, std::string& field, const rapidjson::Value& doc);

        template<typename T>
        void fillFromJsonVolountary(const char* fieldName, T& field, const rapidjson::Value& doc) {
            if (doc.HasMember(fieldName)) {
                if (doc.GetObject()[fieldName].Is<T>() ||
                    (doc.GetObject()[fieldName].IsNumber() && std::is_floating_point<T>::value))
                    field = doc.GetObject()[fieldName].Get<T>();
                else
                    throw std::runtime_error(strErrWrongFieldType + std::string(fieldName));
            }
        }

        void fillFromJsonVolountary(const char* fieldName, std::string& field, const rapidjson::Value& doc);

        InputData loadInputData(const std::filesystem::path & data_directory);

        template<typename T>
        void load_from_json_file(std::unique_ptr<T>& data, const std::filesystem::path& filename) {
            if (filename.empty()) return;
            std::ifstream ifs(filename);
            if (ifs.is_open()){
                rapidjson::IStreamWrapper isw(ifs);
                rapidjson::Document JsonDoc;

                JsonDoc.ParseStream(isw);
                data = std::make_unique<T>(JsonDoc);
            }
        }

    }
}

#endif //USV_INPUTUTILS_H
