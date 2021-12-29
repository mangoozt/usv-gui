#ifndef CHANGE_NOTIFIER_H
#define CHANGE_NOTIFIER_H

#include <functional>
#include <list>

template<typename T>
using NotifierCallback = std::function<void(const T& value)>;

using ListenerRemoveFunc = std::function<void()>;

template<typename T>
class StateNotifier
{
public:
    [nodiscard] ListenerRemoveFunc addListener(const NotifierCallback<T>& callback,
                                               bool fire_immediately=true);
    T value;
protected:
    StateNotifier(T initial_value);
    void notify();
private:
    std::list<NotifierCallback<T>> callbacks;
};

template <typename T>
StateNotifier<T>::StateNotifier(T initial_value) : value(initial_value) {}

template <typename T>
ListenerRemoveFunc StateNotifier<T>::addListener(const NotifierCallback<T> &callback,
                                                 bool fire_immediately) {
    if (fire_immediately)
    {
        callback(value);
    }

    const auto it = callbacks.insert(callbacks.cend(), callback);

    return [this, it] { this->callbacks.erase(it); };
}

template <typename T>
void StateNotifier<T>::notify()
{
    for (const auto& callback : callbacks)
    {
        callback(value);
    }
}

#endif // CHANGE_NOTIFIER_H