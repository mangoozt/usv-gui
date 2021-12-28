#ifndef PROVIDER_H
#define PROVIDER_H

#include <memory>
#include <utility>
#include <cassert>

template <typename T>
class Provider
{
public:
    static T& of();

    template <typename... Args>
    static void create(Args&&... args);
private:
    static std::unique_ptr<T> instance;
};

template <typename T>
std::unique_ptr<T> Provider<T>::instance = nullptr;

template <typename T>
T& Provider<T>::of() {
    assert(((void)"instance is NULL", instance.get() != nullptr));
    return *instance;
}

template <typename T>
template <typename... Args>
void Provider<T>::create(Args&&... args) {
    instance = std::make_unique<T>(std::forward<Args>(args)...);
}

#endif // PROVIDER_H