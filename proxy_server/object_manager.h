// proxy_server/object_manager.h
#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <set> // Using set for simpler existence check and retrieval

class ObjectManager {
public:
    // Get the singleton instance
    static ObjectManager& instance();

    // Register an object name. Returns true on success, false if name already exists.
    bool register_object(const std::string& name);

    // Unregister an object name. Returns true if object existed, false otherwise.
    bool unregister_object(const std::string& name);

    // Check if an object name is registered.
    bool is_registered(const std::string& name);

    // Get a list of registered object names.
    std::vector<std::string> get_registered_objects();

    // Delete copy constructor and assignment operator
    ObjectManager(const ObjectManager&) = delete;
    ObjectManager& operator=(const ObjectManager&) = delete;

private:
    // Private constructor for singleton
    ObjectManager() = default;

    std::set<std::string> registered_object_names_;
    std::mutex mutex_; // Mutex to protect access to the set
};

#endif // OBJECT_MANAGER_H