#ifndef USV_GUI_USVRUN_H
#define USV_GUI_USVRUN_H

#include "InputTypes.h"

namespace USV {
    class USVRunner {
        std::filesystem::path executable;
    public:
        explicit USVRunner(std::filesystem::path executable);

    public:
        int run(const std::filesystem::path& directory, const InputTypes::DataFilenames* data_filenames);
    };

}

#endif //USV_GUI_USVRUN_H
