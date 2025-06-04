// proxy_server/object_manager.cpp
#include "../include/object_manager.h"
#include <vector>
#include <algorithm> // For std::copy

// Singleton instance getter
ObjectManager& ObjectManager::instance() {
    static ObjectManager instance; // Guaranteed to be destroyed, instantiated on first use.
    return instance;
}

// Register an object name
bool ObjectManager::register_object(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
    // insert returns a pair: iterator and bool (true if inserted, false if already existed)
    return registered_object_names_.insert(name).second;
} // Mutex automatically unlocked here

// Unregister an object name
bool ObjectManager::unregister_object(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
    // erase returns the number of elements removed (0 or 1 for a set)
    return registered_object_names_.erase(name) > 0;
} // Mutex automatically unlocked here

// Check if an object name is registered
bool ObjectManager::is_registered(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
    // count returns 1 if the element exists, 0 otherwise
    return registered_object_names_.count(name) > 0;
} // Mutex automatically unlocked here

// Get a list of registered object names
std::vector<std::string> ObjectManager::get_registered_objects() {
    std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
    std::vector<std::string> objects;
    objects.reserve(registered_object_names_.size()); // Optimize allocation
    // Copy elements from the set to the vector
    std::copy(registered_object_names_.begin(), registered_object_names_.end(), std::back_inserter(objects));
    return objects;
} // Mutex automatically unlocked here
