#ifndef USV_GUI_UTILS_H
#define USV_GUI_UTILS_H

#include <vector>

template<typename T>
inline size_t data_sizeof(const typename std::vector<T>& vec) {
    return sizeof(T) * vec.size();
}

#endif //USV_GUI_UTILS_H
