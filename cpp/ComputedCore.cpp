#include "ComputedCore.hpp"

namespace nitrostate {

ComputedCore::ComputedCore(ComputeFn compute)
    : compute_(std::move(compute)) {}

ComputedCore::~ComputedCore() {
    // Unsubscribe from all dependencies
    for (size_t i = 0; i < dependencies_.size(); ++i) {
        if (i < subscriptionIds_.size()) {
            dependencies_[i]->unsubscribe(subscriptionIds_[i]);
        }
    }
}

jsi::Value ComputedCore::get(jsi::Runtime& rt) {
    if (dirty_ || !cachedValue_) {
        // Recompute
        jsi::Value result = compute_(rt);
        cachedValue_ = std::make_shared<jsi::Value>(rt, result);
        dirty_ = false;
    }
    
    // Return cached value
    if (cachedValue_) {
        if (cachedValue_->isUndefined()) return jsi::Value::undefined();
        if (cachedValue_->isNull()) return jsi::Value::null();
        if (cachedValue_->isBool()) return jsi::Value(cachedValue_->getBool());
        if (cachedValue_->isNumber()) return jsi::Value(cachedValue_->getNumber());
        if (cachedValue_->isString()) return jsi::Value(rt, cachedValue_->getString(rt));
        if (cachedValue_->isObject()) {
            return jsi::Value(rt, cachedValue_->getObject(rt));
        }
    }
    return jsi::Value::undefined();
}

void ComputedCore::addDependency(AtomCore* atom) {
    dependencies_.push_back(atom);
    
    // Subscribe to dependency changes
    auto id = atom->subscribe([this]() {
        this->markDirty();
    });
    subscriptionIds_.push_back(id);
}

void ComputedCore::markDirty() {
    dirty_ = true;
}

} // namespace nitrostate
