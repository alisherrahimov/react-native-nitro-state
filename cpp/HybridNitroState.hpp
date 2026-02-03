#pragma once

#include <NitroModules/HybridObject.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include "AtomCore.hpp"
#include "ComputedCore.hpp"
#include "BatchManager.hpp"

namespace nitrostate {

using namespace margelo::nitro;

/**
 * HybridNitroState - Main JSI binding for the state management library
 * 
 * Exposes atom/computed operations to JavaScript via Nitro Modules.
 */
class HybridNitroState : public HybridObject {
public:
    static constexpr auto kJavaDescriptor = "Lcom/margelo/nitro/nitrostate/HybridNitroState;";
    static constexpr auto kObjCDescriptor = "HybridNitroState";

    explicit HybridNitroState();
    ~HybridNitroState() override = default;

    // ----- Atom Operations -----
    
    /**
     * Create a new atom with initial value
     */
    void createAtom(const std::string& key, jsi::Runtime& rt, const jsi::Value& initialValue);
    
    /**
     * Get current atom value
     */
    jsi::Value getAtomValue(const std::string& key, jsi::Runtime& rt);
    
    /**
     * Set atom value
     */
    void setAtomValue(const std::string& key, jsi::Runtime& rt, const jsi::Value& value);
    
    /**
     * Subscribe to atom changes
     * @return Unsubscribe function
     */
    jsi::Function subscribeAtom(
        const std::string& key, 
        jsi::Runtime& rt, 
        jsi::Function callback
    );

    /**
     * Delete an atom
     */
    void deleteAtom(const std::string& key);

    // ----- Computed Operations -----
    
    /**
     * Create a computed value from dependencies
     */
    void createComputed(
        const std::string& key,
        jsi::Runtime& rt,
        const std::vector<std::string>& dependencies,
        jsi::Function compute
    );
    
    /**
     * Get computed value
     */
    jsi::Value getComputedValue(const std::string& key, jsi::Runtime& rt);

    /**
     * Delete a computed value
     */
    void deleteComputed(const std::string& key);

    // ----- Batch Operations -----
    
    void startBatch();
    void endBatch();

    // ----- Utility -----
    
    /**
     * Check if an atom exists
     */
    bool hasAtom(const std::string& key) const;

    /**
     * Get all atom keys
     */
    std::vector<std::string> getAtomKeys() const;

protected:
    void loadHybridMethods() override;

private:
    std::unordered_map<std::string, std::unique_ptr<AtomCore>> atoms_;
    std::unordered_map<std::string, std::unique_ptr<ComputedCore>> computed_;
    std::unordered_map<std::string, std::shared_ptr<jsi::Function>> computeFns_;
    mutable std::mutex mutex_;
};

} // namespace nitrostate
