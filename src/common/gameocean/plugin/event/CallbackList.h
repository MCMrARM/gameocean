#pragma once

#include <memory>
#include <set>
#include <functional>

template <typename T>
using Callback = std::function<void (T&)>;

template <typename T>
class CallbackList {

private:
    std::set<Callback<T>*> callbacks;

public:
    void add(Callback<T>* callback) {
        callbacks.insert(callback);
    }
    void remove(Callback<T>* callback) {
        callbacks.erase(callback);
    }
    void call(T& event) {
        for (Callback<T>* callback : callbacks) {
            (*callback)(event);
        }
    }

};

template <typename T>
class CallbackAuto {
public:
    Callback<T> callback;
    CallbackAuto(Callback<T> const& callback) : callback(callback) {
        T::callbacks.add(&this->callback);
    }
    ~CallbackAuto() {
        T::callbacks.remove(&callback);
    }
};