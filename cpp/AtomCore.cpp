#include "AtomCore.hpp"

namespace nitrostate {

AtomCore::AtomCore(jsi::Runtime& rt, const jsi::Value& initialValue)
    : value_(std::make_shared<jsi::Value>(rt, initialValue)) {}

jsi::Value AtomCore::get(jsi::Runtime& rt) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (value_) {
        // Deep copy the value for safe return
        if (value_->isUndefined()) return jsi::Value::undefined();
        if (value_->isNull()) return jsi::Value::null();
        if (value_->isBool()) return jsi::Value(value_->getBool());
        if (value_->isNumber()) return jsi::Value(value_->getNumber());
        if (value_->isString()) return jsi::Value(rt, value_->getString(rt));
        if (value_->isObject()) {
            // For objects/arrays, we need to return the same reference
            return jsi::Value(rt, value_->getObject(rt));
        }
    }
    return jsi::Value::undefined();
}

void AtomCore::set(jsi::Runtime& rt, const jsi::Value& value) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = std::make_shared<jsi::Value>(rt, value);
        dirty_ = true;
    }
    // Notify outside lock to prevent deadlocks
    notify();
}

AtomCore::SubscriberId AtomCore::subscribe(Callback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    SubscriberId id = nextId_++;
    subscribers_.emplace_back(id, std::move(callback));
    return id;
}

void AtomCore::unsubscribe(SubscriberId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    subscribers_.erase(
        std::remove_if(subscribers_.begin(), subscribers_.end(),
            [id](const auto& pair) { return pair.first == id; }),
        subscribers_.end()
    );
}

void AtomCore::notify() {
    std::vector<Callback> callbacksCopy;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!dirty_) return;
        
        callbacksCopy.reserve(subscribers_.size());
        for (const auto& [_, callback] : subscribers_) {
            callbacksCopy.push_back(callback);
        }
    }
    
    // Call subscribers outside lock
    for (const auto& callback : callbacksCopy) {
        callback();
    }
}

} // namespace nitrostate
